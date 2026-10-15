//! Applies the NML_Visuals combat-footage look to the local camera (ADR 0007). Client only; nothing is replicated.
//! Started from the modded ArmaReforgerScripted.OnWorldPostProcess and polled on the game call queue.
//!
//! Stack (own materials, own slots after vanilla's 5-8: Colors 5, RadialBlur 6, GaussFilter 7, DOF 8):
//!   10 ColorGrading  LUT: tone curve + colour (Day, or LowLight with no black lift)
//!   11 Colors        live saturation trim, driven by preset x weather x time (tone lives in the LUT:
//!                    Colors contrast/brightness act on linear values and crush blacks)
//!   13 ChromAber     edge fringing
//!   15 GaussFilter   softness
//!   16 RotBlur       motion blur on fast turns
//!   17 FilmGrain     sensor noise, stronger in the dark
//! plus vignette and grime overlays under the HUD. OFF clears our slots only, restoring whatever is underneath.
class NML_VisualsManager
{
	static const string SETTINGS_MODULE = "NML_VisualsSettings";

	static const int PRIORITY_LUT = 10;
	static const int PRIORITY_TRIM = 11;
	static const int PRIORITY_FRINGE = 13;
	static const int PRIORITY_SOFT = 15;
	static const int PRIORITY_MOTION = 16;
	static const int PRIORITY_GRAIN = 17;

	// Camera, world, menu and respawn changes are picked up within POLL_MS; weather/time is sampled every
	// WEATHER_POLLS polls and blended towards its target by BLEND per sample (about 5 s to settle).
	protected static const int POLL_MS = 100;
	protected static const int WEATHER_POLLS = 5;
	protected static const float BLEND = 0.12;

	// Warfare-Colormod (Myrove dependency). Its ScreenEffects_ColorPP.emat override fails to parse on game 1.8.0.13
	// ("Unknown keyword Offset"), so it currently changes nothing and both trim materials hold the same values.
	// If Colormod is fixed, retune NML_Visuals_Grade_Colormod.emat against it (ADR 0007).
	protected static const string COLORMOD_GUID = "65A2EA40DC9E632A";

	protected static const ResourceName LUT_DAY = "{6A7CD3BFB176F91B}Materials/NML/Visuals/NML_Visuals_Lut_Day.emat";
	protected static const ResourceName LUT_LOWLIGHT = "{14900DAD97099BE4}Materials/NML/Visuals/NML_Visuals_Lut_LowLight.emat";
	protected static const ResourceName TRIM_OVER_COLORMOD = "{086A86D87F5B514C}Materials/NML/Visuals/NML_Visuals_Grade_Colormod.emat";
	protected static const ResourceName TRIM_OVER_VANILLA = "{0DAD025814CB44FC}Materials/NML/Visuals/NML_Visuals_Grade_Vanilla.emat";
	protected static const ResourceName FRINGE = "{D6019ABBD9351FA0}Materials/NML/Visuals/NML_Visuals_Fringe.emat";
	protected static const ResourceName SOFT = "{784C8E5A0144062A}Materials/NML/Visuals/NML_Visuals_Soft.emat";
	protected static const ResourceName MOTION = "{181864A5BB164FC8}Materials/NML/Visuals/NML_Visuals_Motion.emat";
	protected static const ResourceName GRAIN = "{8E07B73376891360}Materials/NML/Visuals/NML_Visuals_Grain.emat";
	protected static const ResourceName OVERLAY_LAYOUT = "{7963C16C11AEF0F5}UI/Layouts/NML/Visuals/NML_VisualsOverlay.layout";

#ifdef ENABLE_DIAG
	// DiagMenu ids are indexes below 512; vanilla script ids use roughly 0-265, NML_Tests uses 508-511.
	protected static const int DIAG_MENU = 500;
	protected static const int DIAG_ENABLED = 501;
	protected static const int DIAG_PRESET = 502;
	protected static const int DIAG_GRAIN = 503;
	protected static const int DIAG_VIGNETTE = 504;
	protected static const int DIAG_LENS = 505;
	protected static const string DIAG_MENU_NAME = "NML Visuals";
	protected bool m_bToggleKeyDown;
	protected bool m_bGrimeKeyDown;
	protected bool m_bGrimeReview;		// F6: show grime even when the preset has none (review only)
	protected string m_sDiagOnly;		// -nmlVisualsOnly <layer>: apply just one layer (diagnostics)
#endif

	protected static ref NML_VisualsManager s_Instance;
	protected static bool s_bDisabled;

	// Settings
	protected bool m_bEnabled = true;
	protected NML_EVisualsPreset m_ePreset = NML_EVisualsPreset.AUTHENTIC;
	protected NML_EVisualsPreset m_eLastActivePreset = NML_EVisualsPreset.AUTHENTIC;
	protected bool m_bGrain = true;
	protected bool m_bVignette = true;
	protected bool m_bLens = true;
	protected bool m_bColormodLoaded;

	// Apply state
	protected bool m_bDirty = true;
	protected BaseWorld m_pWorld;
	protected int m_iCamera = -1;
	protected IEntity m_pControlled;
	protected BaseWorld m_pSlotsWorld;
	protected int m_iSlotsCamera = -1;
	protected bool m_bLowLightLut;

	// Weather/time blend (0..1), smoothed
	protected int m_iPollCount;
	protected float m_fNight;
	protected float m_fDusk;
	protected float m_fOvercast;
	protected float m_fFog;
	protected float m_fRain;
	protected ref LocalWeatherSituation m_Weather = new LocalWeatherSituation();

	// Last values pushed to the materials, to skip redundant SetParam calls
	protected float m_fSetSaturation = -1;
	protected float m_fSetGrain = -1;
	protected float m_fSetFringe = -1;
	protected float m_fSetSoft = -1;
	protected float m_fSetMotion = -1;

	// Overlay
	protected Widget m_wOverlay;
	protected Widget m_wOverlayParent;
	protected bool m_bOverlayOnWorkspace;
	protected ImageWidget m_wVignette;
	protected ImageWidget m_wGrime;

	//------------------------------------------------------------------------------------------------
	//! Called when a world has loaded. Creates the manager once and (re)starts its poll.
	static void Start()
	{
		if (s_bDisabled)
			return;

		if (!s_Instance)
		{
			if (System.IsConsoleApp())
			{
				s_bDisabled = true; // dedicated server / headless: no rendering
				return;
			}

			s_Instance = new NML_VisualsManager();
		}

		s_Instance.m_bDirty = true;
		GetGame().GetCallqueue().Remove(s_Instance.Poll);
		GetGame().GetCallqueue().CallLater(s_Instance.Poll, POLL_MS, true);
	}

	//------------------------------------------------------------------------------------------------
	void NML_VisualsManager()
	{
		array<string> addons = {};
		GameProject.GetLoadedAddons(addons);
		m_bColormodLoaded = addons.Contains(COLORMOD_GUID);

		LoadSettings();
		GetGame().OnUserSettingsChangedInvoker().Insert(OnUserSettingsChanged);

#ifdef ENABLE_DIAG
		RegisterDiag();

		// Verification aid: -nmlVisualsCycle <seconds> flips the master switch on that interval without
		// saving, so on/off can be compared on the same frame.
		string cycle;
		if (!System.GetCLIParam("nmlVisualsCycle", cycle))
			System.GetCLIParam("nmlvisualscycle", cycle);

		// -nmlVisualsOnly lut|trim|fringe|soft|motion|grain|overlay applies only that layer (diagnostics).
		System.GetCLIParam("nmlVisualsOnly", m_sDiagOnly);
		if (m_sDiagOnly.IsEmpty())
			System.GetCLIParam("nmlvisualsonly", m_sDiagOnly);

		if (!m_sDiagOnly.IsEmpty())
			PrintFormat("[NML] Visuals: diag only layer %1", m_sDiagOnly);

		if (!cycle.IsEmpty() && cycle.ToInt() > 0) // ToInt() throws on an empty string
		{
			GetGame().GetCallqueue().CallLater(DiagCycle, cycle.ToInt() * 1000, true);
			PrintFormat("[NML] Visuals: diag cycle every %1 s", cycle.ToInt());
		}
#endif

		PrintFormat("[NML] Visuals: started (Colormod %1, enabled %2, preset %3)", m_bColormodLoaded, m_bEnabled, typename.EnumToString(NML_EVisualsPreset, m_ePreset));
	}

	//------------------------------------------------------------------------------------------------
	void ~NML_VisualsManager()
	{
		if (GetGame())
		{
			GetGame().GetCallqueue().Remove(Poll);
#ifdef ENABLE_DIAG
			GetGame().GetCallqueue().Remove(DiagCycle);
#endif
			if (GetGame().OnUserSettingsChangedInvoker())
				GetGame().OnUserSettingsChangedInvoker().Remove(OnUserSettingsChanged);
		}

		DestroyOverlay();
	}

	//------------------------------------------------------------------------------------------------
	protected void OnUserSettingsChanged()
	{
		LoadSettings();
	}

	//------------------------------------------------------------------------------------------------
	protected void LoadSettings()
	{
		BaseContainer module = GetGame().GetGameUserSettings().GetModule(SETTINGS_MODULE);
		if (!module)
		{
			Print("[NML] Visuals: settings module not found; using defaults", LogLevel.WARNING);
			m_bDirty = true;
			return;
		}

		int preset;
		module.Get("m_bEnabled", m_bEnabled);
		module.Get("m_iPreset", preset);
		module.Get("m_bGrain", m_bGrain);
		module.Get("m_bVignette", m_bVignette);
		module.Get("m_bLens", m_bLens);

		// Unknown values (e.g. a later preset read by an older build) fall back to AUTHENTIC.
		if (preset < NML_EVisualsPreset.OFF || preset > NML_EVisualsPreset.EXTREME)
			preset = NML_EVisualsPreset.AUTHENTIC;

		m_ePreset = preset;
		if (m_ePreset != NML_EVisualsPreset.OFF)
			m_eLastActivePreset = m_ePreset;

		m_bDirty = true;

#ifdef ENABLE_DIAG
		PushDiag(); // settings changed elsewhere: the diag menu follows, it never overrides them
#endif
	}

	//------------------------------------------------------------------------------------------------
	protected void SaveSettings()
	{
		BaseContainer module = GetGame().GetGameUserSettings().GetModule(SETTINGS_MODULE);
		if (!module)
			return;

		module.Set("m_bEnabled", m_bEnabled);
		module.Set("m_iPreset", m_ePreset);
		module.Set("m_bGrain", m_bGrain);
		module.Set("m_bVignette", m_bVignette);
		module.Set("m_bLens", m_bLens);
		GetGame().UserSettingsChanged();
		GetGame().SaveUserSettings();
	}

	//------------------------------------------------------------------------------------------------
	protected void Poll()
	{
#ifdef ENABLE_DIAG
		PollDiag();
		PollKeys();
#endif

		BaseWorld world = GetGame().GetWorld();
		if (world != m_pWorld)
		{
			// New world or back to the main menu: the old world's slots went with it; rebuild the overlay.
			m_pWorld = world;
			m_iCamera = -1;
			m_pSlotsWorld = null;
			m_iSlotsCamera = -1;
			DestroyOverlay();
			m_bDirty = true;
		}

		if (!world)
			return;

		NML_VisualsPreset preset = GetActivePreset();
		UpdateOverlay(preset);

		int camera = world.GetCurrentCameraId();
		if (camera < 0)
			return; // keep m_bDirty; apply once a camera exists

		m_iPollCount++;
		if (preset && (m_bDirty || m_iPollCount % WEATHER_POLLS == 0))
			SampleWeather(world);

		// A respawn may reuse the camera index with a fresh post-process stack: re-apply on a new controlled entity.
		IEntity controlled;
		PlayerController playerController = GetGame().GetPlayerController();
		if (playerController)
			controlled = playerController.GetControlledEntity();

		bool lowLight = m_fNight > 0.5;
		if (m_bLowLightLut && m_fNight > 0.4)
			lowLight = true; // hysteresis: no flicker around dusk

		if (m_bDirty || camera != m_iCamera || controlled != m_pControlled || lowLight != m_bLowLightLut)
		{
			m_iCamera = camera;
			m_pControlled = controlled;
			m_bLowLightLut = lowLight;
			m_bDirty = false;
			Apply(preset);
		}

		if (preset)
			UpdateParams(preset);
	}

	//------------------------------------------------------------------------------------------------
	protected NML_VisualsPreset GetActivePreset()
	{
		if (!m_bEnabled)
			return null;

		return NML_VisualsPreset.Get(m_ePreset);
	}

	//------------------------------------------------------------------------------------------------
	protected bool IsActive()
	{
		return GetActivePreset() != null;
	}

	//------------------------------------------------------------------------------------------------
	//! Reads weather and time and moves the blend factors towards them.
	protected void SampleWeather(BaseWorld world)
	{
		ChimeraWorld chimeraWorld = ChimeraWorld.CastFrom(world);
		if (!chimeraWorld)
			return;

		TimeAndWeatherManagerEntity weatherManager = chimeraWorld.GetTimeAndWeatherManager();
		if (!weatherManager)
			return;

		vector cameraPos;
		CameraManager cameraManager = GetGame().GetCameraManager();
		if (cameraManager && cameraManager.CurrentCamera())
			cameraPos = cameraManager.CurrentCamera().GetOrigin();

		float overcast, fog, rain;
		if (weatherManager.TryGetCompleteLocalWeather(m_Weather, 0, cameraPos))
		{
			overcast = m_Weather.GetOvercast();
			fog = m_Weather.GetFog();
			rain = m_Weather.GetRainIntensity();
		}

		// Night/dusk from the sun times: full night 45 min after sunset until 45 min before sunrise,
		// dusk/dawn peaks at sunset/sunrise and fades over 1.5 h either side.
		float hour = weatherManager.GetTimeOfTheDay();
		float sunrise = 6, sunset = 20;
		weatherManager.GetSunriseHour(sunrise);
		weatherManager.GetSunsetHour(sunset);
		float night = 0;
		if (hour > sunset + 0.75 || hour < sunrise - 0.75)
			night = 1;
		else if (hour > sunset)
			night = (hour - sunset) / 0.75;
		else if (hour < sunrise)
			night = (sunrise - hour) / 0.75;

		float duskDistance = Math.Min(Math.AbsFloat(hour - sunset), Math.AbsFloat(hour - sunrise));
		float dusk = Math.Clamp(1 - duskDistance / 1.5, 0, 1) * (1 - night);

		float blend = BLEND;
		if (m_bDirty)
			blend = 1; // first sample after a change: jump, no fade

		m_fNight = Math.Lerp(m_fNight, night, blend);
		m_fDusk = Math.Lerp(m_fDusk, dusk, blend);
		m_fOvercast = Math.Lerp(m_fOvercast, Math.Clamp(overcast, 0, 1), blend);
		m_fFog = Math.Lerp(m_fFog, Math.Clamp(fog, 0, 1), blend);
		m_fRain = Math.Lerp(m_fRain, Math.Clamp(rain, 0, 1), blend);
	}

	//------------------------------------------------------------------------------------------------
	//! Sets or clears our slots. Material parameters are pushed separately by UpdateParams.
	protected void Apply(NML_VisualsPreset preset)
	{
		// Always clear the camera we last set first (also covers camera switches).
		ClearSlots();
		ResetParamCache();

		if (!preset)
		{
			PrintFormat("[NML] Visuals: cleared on camera %1", m_iCamera);
			return;
		}

		ResourceName lut = LUT_DAY;
		if (m_bLowLightLut)
			lut = LUT_LOWLIGHT;

		ResourceName trim = TRIM_OVER_VANILLA;
		if (m_bColormodLoaded)
			trim = TRIM_OVER_COLORMOD;

		if (UseLayer("lut"))
			m_pWorld.SetCameraPostProcessEffect(m_iCamera, PRIORITY_LUT, PostProcessEffectType.ColorGrading, lut);

		if (UseLayer("trim"))
			m_pWorld.SetCameraPostProcessEffect(m_iCamera, PRIORITY_TRIM, PostProcessEffectType.Colors, trim);

		if (m_bLens)
		{
			if (UseLayer("fringe"))
				m_pWorld.SetCameraPostProcessEffect(m_iCamera, PRIORITY_FRINGE, PostProcessEffectType.ChromAber, FRINGE);

			if (preset.m_fSoft > 0 && UseLayer("soft"))
				m_pWorld.SetCameraPostProcessEffect(m_iCamera, PRIORITY_SOFT, PostProcessEffectType.GaussFilter, SOFT);

			if (preset.m_fMotion > 0 && UseLayer("motion"))
				m_pWorld.SetCameraPostProcessEffect(m_iCamera, PRIORITY_MOTION, PostProcessEffectType.RotBlur, MOTION);
		}

		if (m_bGrain && UseLayer("grain"))
			m_pWorld.SetCameraPostProcessEffect(m_iCamera, PRIORITY_GRAIN, PostProcessEffectType.FilmGrain, GRAIN);

		m_pSlotsWorld = m_pWorld;
		m_iSlotsCamera = m_iCamera;
		PrintFormat("[NML] Visuals: applied %1 on camera %2 (lut %3, grain %4, overlays %5, lens %6)", typename.EnumToString(NML_EVisualsPreset, m_ePreset), m_iCamera, lut, m_bGrain, m_bVignette, m_bLens);
	}

	//------------------------------------------------------------------------------------------------
	//! Always true in release builds; in diag builds -nmlVisualsOnly limits the stack to one layer.
	protected bool UseLayer(string layer)
	{
#ifdef ENABLE_DIAG
		return m_sDiagOnly.IsEmpty() || m_sDiagOnly == layer;
#else
		return true;
#endif
	}

	//------------------------------------------------------------------------------------------------
	//! Clears only NML slots on the camera they were set on, which restores whatever is underneath.
	protected void ClearSlots()
	{
		if (!m_pSlotsWorld || m_iSlotsCamera < 0)
			return;

		m_pSlotsWorld.SetCameraPostProcessEffect(m_iSlotsCamera, PRIORITY_LUT, PostProcessEffectType.ColorGrading, string.Empty);
		m_pSlotsWorld.SetCameraPostProcessEffect(m_iSlotsCamera, PRIORITY_TRIM, PostProcessEffectType.Colors, string.Empty);
		m_pSlotsWorld.SetCameraPostProcessEffect(m_iSlotsCamera, PRIORITY_FRINGE, PostProcessEffectType.ChromAber, string.Empty);
		m_pSlotsWorld.SetCameraPostProcessEffect(m_iSlotsCamera, PRIORITY_SOFT, PostProcessEffectType.GaussFilter, string.Empty);
		m_pSlotsWorld.SetCameraPostProcessEffect(m_iSlotsCamera, PRIORITY_MOTION, PostProcessEffectType.RotBlur, string.Empty);
		m_pSlotsWorld.SetCameraPostProcessEffect(m_iSlotsCamera, PRIORITY_GRAIN, PostProcessEffectType.FilmGrain, string.Empty);
		m_pSlotsWorld = null;
		m_iSlotsCamera = -1;
	}

	//------------------------------------------------------------------------------------------------
	protected void ResetParamCache()
	{
		m_fSetSaturation = -1;
		m_fSetGrain = -1;
		m_fSetFringe = -1;
		m_fSetSoft = -1;
		m_fSetMotion = -1;
	}

	//------------------------------------------------------------------------------------------------
	//! Preset x weather x time -> material parameters. Fairness: brightness is never raised; the dark
	//! uses the LowLight LUT (no black lift) and only more noise.
	protected void UpdateParams(NML_VisualsPreset preset)
	{
		float saturation = preset.m_fTrimSaturation;
		saturation *= Math.Lerp(1, 0.85, m_fOvercast);
		saturation *= Math.Lerp(1, 0.80, m_fRain);
		saturation *= Math.Lerp(1, 0.85, m_fDusk);		// mute warm dusk light, no orange cinema look

		float grain = preset.m_fGrain * (1 + 1.2 * m_fNight + 0.4 * m_fDusk);
		float soft = preset.m_fSoft;
		if (soft > 0)
			soft += 0.1 * m_fRain;

		Material trim = Material.GetOrLoadMaterial(TRIM_OVER_VANILLA, 0);
		if (m_bColormodLoaded)
			trim = Material.GetOrLoadMaterial(TRIM_OVER_COLORMOD, 0);

		if (trim)
		{
			m_fSetSaturation = PushParam(trim, "Saturation", saturation, m_fSetSaturation);
		}

		Material grainMaterial = Material.GetOrLoadMaterial(GRAIN, 0);
		if (grainMaterial)
			m_fSetGrain = PushParam(grainMaterial, "Intensity", grain, m_fSetGrain);

		Material fringe = Material.GetOrLoadMaterial(FRINGE, 0);
		if (fringe && Math.AbsFloat(preset.m_fFringe - m_fSetFringe) > 0.00005)
		{
			fringe.SetParam("PowerX", preset.m_fFringe);
			fringe.SetParam("PowerY", preset.m_fFringe);
			m_fSetFringe = preset.m_fFringe;
		}

		Material softMaterial = Material.GetOrLoadMaterial(SOFT, 0);
		if (softMaterial)
			m_fSetSoft = PushParam(softMaterial, "Intensity", soft, m_fSetSoft);

		Material motion = Material.GetOrLoadMaterial(MOTION, 0);
		if (motion)
			m_fSetMotion = PushParam(motion, "Power", preset.m_fMotion, m_fSetMotion);

		if (m_wVignette)
			m_wVignette.SetOpacity(preset.m_fVignette);

		if (m_wGrime)
		{
			float grime = preset.m_fGrime * (1 + 0.5 * m_fRain);
#ifdef ENABLE_DIAG
			if (m_bGrimeReview)
				grime = Math.Max(grime, 0.6);
#endif
			m_wGrime.SetOpacity(Math.Clamp(grime, 0, 1));
		}
	}

	//------------------------------------------------------------------------------------------------
	protected float PushParam(Material material, string name, float value, float lastValue)
	{
		if (Math.AbsFloat(value - lastValue) < 0.002)
			return lastValue;

		material.SetParam(name, value);
		return value;
	}

	//------------------------------------------------------------------------------------------------
	//! Overlays live under the HUD (HUD root, lowest z-order) so they never dim the HUD. Without a HUD
	//! (e.g. main menu edge cases) they fall back to the workspace and hide while a menu is open.
	protected void UpdateOverlay(NML_VisualsPreset preset)
	{
		Widget parent;
		SCR_HUDManagerComponent hud = SCR_HUDManagerComponent.GetHUDManager();
		if (hud)
			parent = hud.GetHUDRootWidget();

		bool onWorkspace = parent == null;
		if (m_wOverlay && (parent != m_wOverlayParent || onWorkspace != m_bOverlayOnWorkspace))
			DestroyOverlay(); // HUD replaced (respawn) or moved: rebuild under the new parent

		bool show = preset && m_bVignette && UseLayer("overlay");
		if (show && onWorkspace)
		{
			MenuManager menuManager = GetGame().GetMenuManager();
			show = !(menuManager && menuManager.IsAnyMenuOpen());
		}

		if (!show)
		{
			if (m_wOverlay && m_wOverlay.IsVisible())
				m_wOverlay.SetVisible(false);

			return;
		}

		if (!m_wOverlay)
		{
			WorkspaceWidget workspace = GetGame().GetWorkspace();
			if (!workspace)
				return;

			m_wOverlay = workspace.CreateWidgets(OVERLAY_LAYOUT, parent);
			if (!m_wOverlay)
			{
				Print("[NML] Visuals: overlay layout failed to load", LogLevel.WARNING);
				return;
			}

			m_wOverlayParent = parent;
			m_bOverlayOnWorkspace = onWorkspace;
			if (!onWorkspace)
				m_wOverlay.SetZOrder(-1000); // first child drawn: under every HUD element

			m_wVignette = ImageWidget.Cast(m_wOverlay.FindAnyWidget("Vignette"));
			m_wGrime = ImageWidget.Cast(m_wOverlay.FindAnyWidget("Grime"));
			if (m_wGrime)
				m_wGrime.SetOpacity(0);
		}

		if (!m_wOverlay.IsVisible())
			m_wOverlay.SetVisible(true);
	}

	//------------------------------------------------------------------------------------------------
	protected void DestroyOverlay()
	{
		if (m_wOverlay)
			m_wOverlay.RemoveFromHierarchy();

		m_wOverlay = null;
		m_wOverlayParent = null;
		m_wVignette = null;
		m_wGrime = null;
	}

#ifdef ENABLE_DIAG
	//------------------------------------------------------------------------------------------------
	//! Prototype controls (diag builds only; no settings dialog yet): diag menu "NML Visuals". Changes are saved.
	protected void RegisterDiag()
	{
		DiagMenu.RegisterMenu(DIAG_MENU, DIAG_MENU_NAME, "");
		DiagMenu.RegisterBool(DIAG_ENABLED, "", "Enabled", DIAG_MENU_NAME);
		DiagMenu.RegisterItem(DIAG_PRESET, "", "Preset", DIAG_MENU_NAME, "OFF,LOW,AUTHENTIC,HEAVY,EXTREME");
		DiagMenu.RegisterBool(DIAG_GRAIN, "", "Grain", DIAG_MENU_NAME);
		DiagMenu.RegisterBool(DIAG_VIGNETTE, "", "Vignette + grime", DIAG_MENU_NAME);
		DiagMenu.RegisterBool(DIAG_LENS, "", "Fringe + softness + motion", DIAG_MENU_NAME);
		PushDiag();
	}

	//------------------------------------------------------------------------------------------------
	protected void PushDiag()
	{
		DiagMenu.SetValue(DIAG_ENABLED, m_bEnabled);
		DiagMenu.SetValue(DIAG_PRESET, m_ePreset);
		DiagMenu.SetValue(DIAG_GRAIN, m_bGrain);
		DiagMenu.SetValue(DIAG_VIGNETTE, m_bVignette);
		DiagMenu.SetValue(DIAG_LENS, m_bLens);
	}

	//------------------------------------------------------------------------------------------------
	protected void DiagCycle()
	{
		m_bEnabled = !m_bEnabled;
		m_bDirty = true;
		PushDiag(); // keep the diag menu in sync so PollDiag does not save this as a user change
		PrintFormat("[NML] Visuals: cycle -> enabled %1", m_bEnabled);
	}

	//------------------------------------------------------------------------------------------------
	//! Test hotkeys (diag builds only, until the settings dialog exists):
	//! F7 OFF <-> last preset, Shift+F7 next preset, F6 grime review on/off. F7 changes are saved.
	protected void PollKeys()
	{
		bool toggleDown = Debug.KeyState(KeyCode.KC_F7) != 0;
		if (toggleDown != m_bToggleKeyDown)
		{
			m_bToggleKeyDown = toggleDown;
			if (toggleDown)
			{
				bool shift = Debug.KeyState(KeyCode.KC_LSHIFT) != 0 || Debug.KeyState(KeyCode.KC_RSHIFT) != 0;
				if (shift)
				{
					int next = m_eLastActivePreset + 1;
					if (next > NML_EVisualsPreset.EXTREME)
						next = NML_EVisualsPreset.LOW;

					m_ePreset = next;
					m_eLastActivePreset = next;
					m_bEnabled = true;
				}
				else if (IsActive())
				{
					m_bEnabled = false;
				}
				else
				{
					m_bEnabled = true;
					m_ePreset = m_eLastActivePreset;
				}

				m_bDirty = true;
				SaveSettings(); // triggers LoadSettings -> PushDiag via the settings-changed invoker
				ShowState();
			}
		}

		bool grimeDown = Debug.KeyState(KeyCode.KC_F6) != 0;
		if (grimeDown != m_bGrimeKeyDown)
		{
			m_bGrimeKeyDown = grimeDown;
			if (grimeDown)
			{
				m_bGrimeReview = !m_bGrimeReview;
				m_bDirty = true;
				string grimeState = "off";
				if (m_bGrimeReview)
					grimeState = "on";

				SCR_HintManagerComponent.ShowCustomHint("NML Visuals: grime review " + grimeState, "NML Visuals", 3);
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	protected void ShowState()
	{
		string state = "OFF";
		if (IsActive())
			state = typename.EnumToString(NML_EVisualsPreset, m_ePreset);

		SCR_HintManagerComponent.ShowCustomHint("NML Visuals: " + state, "NML Visuals", 3);
		PrintFormat("[NML] Visuals: key -> %1", state);
	}

	//------------------------------------------------------------------------------------------------
	//! A diag-menu edit is a user change: apply and save it.
	protected void PollDiag()
	{
		bool enabled = DiagMenu.GetBool(DIAG_ENABLED);
		int preset = DiagMenu.GetValue(DIAG_PRESET);
		bool grain = DiagMenu.GetBool(DIAG_GRAIN);
		bool vignette = DiagMenu.GetBool(DIAG_VIGNETTE);
		bool lens = DiagMenu.GetBool(DIAG_LENS);

		if (enabled == m_bEnabled && preset == m_ePreset && grain == m_bGrain && vignette == m_bVignette && lens == m_bLens)
			return;

		if (preset < NML_EVisualsPreset.OFF || preset > NML_EVisualsPreset.EXTREME)
			preset = NML_EVisualsPreset.AUTHENTIC;

		m_bEnabled = enabled;
		m_ePreset = preset;
		if (m_ePreset != NML_EVisualsPreset.OFF)
			m_eLastActivePreset = m_ePreset;

		m_bGrain = grain;
		m_bVignette = vignette;
		m_bLens = lens;
		m_bDirty = true;
		SaveSettings();
		PrintFormat("[NML] Visuals: settings changed (enabled %1, preset %2, grain %3, overlays %4, lens %5)", m_bEnabled, typename.EnumToString(NML_EVisualsPreset, m_ePreset), m_bGrain, m_bVignette, m_bLens);
	}
#endif
}

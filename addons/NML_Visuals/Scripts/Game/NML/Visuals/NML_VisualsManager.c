//! Applies the NML_Visuals combat-footage look to the local camera (ADR 0007). Client only; nothing is replicated.
//! Started from the modded ArmaReforgerScripted.OnWorldPostProcess and polled on the game call queue.
//!
//! Stack (own materials, own slots after vanilla's 5-8: Colors 5, RadialBlur 6, GaussFilter 7, DOF 8):
//!   10 ColorGrading  LUT: tone curve + colour (preset day LUT, or the shared LowLight LUT with no black lift)
//!   11 Colors        live saturation trim, driven by preset x weather x time (tone lives in the LUT:
//!                    Colors contrast/brightness act on linear values and crush blacks)
//!   13 ChromAber     edge fringing
//! plus vignette and grime overlays under the HUD. OFF clears our slots only, restoring whatever is underneath.
//! Not used: GaussFilter (uniform blur; its Mask gave no edge-only softness), RotBlur (blacked out the
//! frame) on game 1.8.0.13, and FilmGrain (reads as flat film grain, removed at the lead's request).
//! Players change settings in the pause-menu panel (NML_VisualsPanel).
class NML_VisualsManager
{
	static const string SETTINGS_MODULE = "NML_VisualsSettings";

	static const int PRIORITY_LUT = 10;
	static const int PRIORITY_TRIM = 11;
	static const int PRIORITY_FRINGE = 13;
	static const int PRIORITY_FRINGE_ALT = 17;	// -nmlvSlots alt (Stage 1): no script/config writer found for 17

	// Camera, world, menu and respawn changes are picked up within POLL_MS; weather/time is sampled every
	// WEATHER_POLLS polls and blended towards its target by BLEND per sample (about 5 s to settle).
	protected static const int POLL_MS = 100;
	protected static const int WEATHER_POLLS = 5;
	protected static const float BLEND = 0.12;

	// Warfare-Colormod (Myrove dependency). Its ScreenEffects_ColorPP.emat override fails to parse on game 1.8.0.13
	// ("Unknown keyword Offset"), so it currently changes nothing and both trim materials hold the same values.
	// If Colormod is fixed, retune NML_Visuals_Grade_Colormod.emat against it (ADR 0007).
	protected static const string COLORMOD_GUID = "65A2EA40DC9E632A";

	protected static const ResourceName LUT_LOWLIGHT = "{14900DAD97099BE4}Materials/NML/Visuals/NML_Visuals_Lut_LowLight.emat";
	protected static const ResourceName TRIM_OVER_COLORMOD = "{086A86D87F5B514C}Materials/NML/Visuals/NML_Visuals_Grade_Colormod.emat";
	protected static const ResourceName TRIM_OVER_VANILLA = "{0DAD025814CB44FC}Materials/NML/Visuals/NML_Visuals_Grade_Vanilla.emat";
	protected static const ResourceName FRINGE = "{D6019ABBD9351FA0}Materials/NML/Visuals/NML_Visuals_Fringe.emat";
	protected static const ResourceName OVERLAY_LAYOUT = "{7963C16C11AEF0F5}UI/Layouts/NML/Visuals/NML_VisualsOverlay.layout";

#ifdef ENABLE_DIAG
	// DiagMenu ids are indexes below 512; vanilla script ids use roughly 0-265, NML_Tests uses 508-511.
	protected static const int DIAG_MENU = 500;
	protected static const int DIAG_ENABLED = 501;
	protected static const int DIAG_PRESET = 502;
	protected static const int DIAG_VIGNETTE = 504;
	protected static const int DIAG_LENS = 505;
	protected static const string DIAG_MENU_NAME = "NML Visuals";
	protected bool m_bToggleKeyDown;
	protected bool m_bGrimeKeyDown;
	protected bool m_bGrimeReview;		// F6: show grime even when the preset has none (review only)
	protected string m_sDiagOnly;		// -nmlVisualsOnly <layer>: apply just one layer (diagnostics)
	protected int m_iDiagPreset = -1;	// -nmlVisualsPreset <name>: session-only preset (diagnostics)
#endif

	protected static ref NML_VisualsManager s_Instance;
	protected static bool s_bDisabled;

	// Settings
	protected bool m_bEnabled = true;
	protected NML_EVisualsPreset m_ePreset = NML_EVisualsPreset.AUTHENTIC;
	protected NML_EVisualsPreset m_eLastActivePreset = NML_EVisualsPreset.AUTHENTIC;
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
	protected float m_fSetFringe = -1;

	// Overlay
	protected Widget m_wOverlay;
	protected Widget m_wOverlayParent;
	protected bool m_bOverlayOnWorkspace;
	protected ImageWidget m_wVignette;
	protected ImageWidget m_wGrime;
	protected ImageWidget m_wGrimeHeavy;

	// Player controls in the pause menu
	protected ref NML_VisualsPanel m_Panel;

	// Stage 1 measurement (docs/visuals/v2/STAGE1-PLAN.md). With no -nmlv* parameter all of this keeps v1 behaviour.
	protected NML_VisualsTestConfig m_Config;
	protected ref NML_VisualsProbe m_Probe;
	protected NML_EVisualsApplyReason m_eDirtyReason = NML_EVisualsApplyReason.WORLD;
	protected int m_iSlotFringe = PRIORITY_FRINGE;
	protected ResourceName m_sSetLut;		// material currently in each NML slot on m_iSlotsCamera
	protected ResourceName m_sSetTrim;
	protected ResourceName m_sSetFringe;
	protected int m_iApplySets;
	protected int m_iApplyClears;
	protected int m_iApplyReasserts;
	protected int m_iCyclePhase = -1;
	protected bool m_bCycleOff;
	protected int m_iCycleLayers = -1;
	protected bool m_bCycleSaveWarned;
	protected float m_fOpVignette = -1;		// last pushed overlay opacities (-nmlvHideZero)
	protected float m_fOpGrime = -1;
	protected float m_fOpGrimeHeavy = -1;

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

		s_Instance.MarkDirty(NML_EVisualsApplyReason.WORLD);
		if (s_Instance.m_Probe)
			s_Instance.m_Probe.OnStart(); // H7: more than one start per world load means extra worlds re-apply

		GetGame().GetCallqueue().Remove(s_Instance.Poll);
		GetGame().GetCallqueue().CallLater(s_Instance.Poll, POLL_MS, true);
	}

	//------------------------------------------------------------------------------------------------
	void NML_VisualsManager()
	{
		m_Config = NML_VisualsTestConfig.Get();
		if (m_Config.m_bAltSlots)
			m_iSlotFringe = PRIORITY_FRINGE_ALT;

		array<string> addons = {};
		GameProject.GetLoadedAddons(addons);
		m_bColormodLoaded = addons.Contains(COLORMOD_GUID);

		LoadSettings();
		GetGame().OnUserSettingsChangedInvoker().Insert(OnUserSettingsChanged);
		m_Panel = new NML_VisualsPanel(this);

#ifdef ENABLE_DIAG
		RegisterDiag();

		// Verification aid: -nmlVisualsCycle <seconds> flips the master switch on that interval without
		// saving, so on/off can be compared on the same frame.
		string cycle;
		if (!System.GetCLIParam("nmlVisualsCycle", cycle))
			System.GetCLIParam("nmlvisualscycle", cycle);

		// -nmlVisualsOnly lut|trim|fringe|overlay applies only that layer (diagnostics).
		System.GetCLIParam("nmlVisualsOnly", m_sDiagOnly);
		if (m_sDiagOnly.IsEmpty())
			System.GetCLIParam("nmlvisualsonly", m_sDiagOnly);

		if (!m_sDiagOnly.IsEmpty())
			PrintFormat("[NML] Visuals: diag only layer %1", m_sDiagOnly);

		// -nmlVisualsPreset LOW|AUTHENTIC|HEAVY|EXTREME forces a preset for this session without saving it.
		string forced;
		if (!System.GetCLIParam("nmlVisualsPreset", forced))
			System.GetCLIParam("nmlvisualspreset", forced);

		int forcedPreset = -1;
		if (!forced.IsEmpty())
			forcedPreset = typename.StringToEnum(NML_EVisualsPreset, forced);

		if (forcedPreset > NML_EVisualsPreset.OFF)
		{
			m_iDiagPreset = forcedPreset;
			ApplyDiagPreset();
			PrintFormat("[NML] Visuals: diag preset %1", forced);
		}

		// -nmlVisualsPauseAt <seconds> opens the pause menu once (to check the player panel without input).
		string pauseAt;
		if (!System.GetCLIParam("nmlVisualsPauseAt", pauseAt))
			System.GetCLIParam("nmlvisualspauseat", pauseAt);

		if (!pauseAt.IsEmpty() && pauseAt.ToInt() > 0)
			GetGame().GetCallqueue().CallLater(ArmaReforgerScripted.OpenPauseMenu, pauseAt.ToInt() * 1000, false, true, false);

		if (!cycle.IsEmpty() && cycle.ToInt() > 0) // ToInt() throws on an empty string
		{
			GetGame().GetCallqueue().CallLater(DiagCycle, cycle.ToInt() * 1000, true);
			PrintFormat("[NML] Visuals: diag cycle every %1 s", cycle.ToInt());
		}
#endif

		if (m_Config.m_iProbeSeconds > 0)
			m_Probe = new NML_VisualsProbe(this, m_Config.m_iProbeSeconds);

		if (m_Config.m_iCycleSeconds > 0)
		{
			CycleTick(); // phase A now
			GetGame().GetCallqueue().CallLater(CycleTick, m_Config.m_iCycleSeconds * 1000, true);
		}

		PrintFormat("[NML] Visuals: started (Colormod %1, enabled %2, preset %3)", m_bColormodLoaded, m_bEnabled, typename.EnumToString(NML_EVisualsPreset, m_ePreset));
	}

	//------------------------------------------------------------------------------------------------
	void ~NML_VisualsManager()
	{
		if (GetGame())
		{
			GetGame().GetCallqueue().Remove(Poll);
			GetGame().GetCallqueue().Remove(CycleTick);
#ifdef ENABLE_DIAG
			GetGame().GetCallqueue().Remove(DiagCycle);
#endif
			if (GetGame().OnUserSettingsChangedInvoker())
				GetGame().OnUserSettingsChangedInvoker().Remove(OnUserSettingsChanged);
		}

		DestroyOverlay();
		if (m_Panel)
			m_Panel.Destroy();
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
			MarkDirty(NML_EVisualsApplyReason.SETTINGS_OTHER);
			return;
		}

		// Stage 1: tell saves that change our values apart from saves by other menus or mods (H5).
		bool oldEnabled = m_bEnabled;
		NML_EVisualsPreset oldPreset = m_ePreset;
		bool oldVignette = m_bVignette;
		bool oldLens = m_bLens;

		int preset;
		module.Get("m_bEnabled", m_bEnabled);
		module.Get("m_iPreset", preset);
		module.Get("m_bVignette", m_bVignette);
		module.Get("m_bLens", m_bLens);

		// Unknown values (e.g. a later preset read by an older build) fall back to AUTHENTIC.
		if (preset < NML_EVisualsPreset.OFF || preset > NML_EVisualsPreset.EXTREME)
			preset = NML_EVisualsPreset.AUTHENTIC;

		m_ePreset = preset;
		if (m_Config.m_iForcedPreset > NML_EVisualsPreset.OFF)
		{
			// -nmlvPreset: session-only, survives settings reloads
			m_bEnabled = true;
			m_ePreset = m_Config.m_iForcedPreset;
		}

		if (m_ePreset != NML_EVisualsPreset.OFF)
			m_eLastActivePreset = m_ePreset;

		bool ours = m_bEnabled != oldEnabled || m_ePreset != oldPreset || m_bVignette != oldVignette || m_bLens != oldLens;
		if (m_Probe)
			m_Probe.OnSettings(ours);

		if (ours)
			MarkDirty(NML_EVisualsApplyReason.SETTINGS_OURS);
		else if (!m_Config.m_bSettingsFilter)
			MarkDirty(NML_EVisualsApplyReason.SETTINGS_OTHER); // v1 behaviour: any settings save re-applies

#ifdef ENABLE_DIAG
		ApplyDiagPreset(); // a -nmlVisualsPreset session override survives settings reloads
		PushDiag(); // settings changed elsewhere: the diag menu follows, it never overrides them
#endif
	}

	//------------------------------------------------------------------------------------------------
	protected void SaveSettings()
	{
		if (m_Config.m_iCycleSeconds > 0 || m_Config.m_iForcedPreset > NML_EVisualsPreset.OFF)
		{
			// -nmlvCycle / -nmlvPreset are session-only test states; never persist them to the player's profile.
			if (!m_bCycleSaveWarned)
				Print("[NML] Visuals: settings not saved while -nmlvCycle or -nmlvPreset is active", LogLevel.WARNING);

			m_bCycleSaveWarned = true;
			return;
		}

		BaseContainer module = GetGame().GetGameUserSettings().GetModule(SETTINGS_MODULE);
		if (!module)
			return;

		module.Set("m_bEnabled", m_bEnabled);
		module.Set("m_iPreset", m_ePreset);
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

		m_Panel.Update();

		BaseWorld world = GetGame().GetWorld();
		if (world != m_pWorld)
		{
			// New world or back to the main menu: the old world's slots went with it; rebuild the overlay.
			m_pWorld = world;
			m_iCamera = -1;
			m_pSlotsWorld = null;
			m_iSlotsCamera = -1;
			m_sSetLut = string.Empty;
			m_sSetTrim = string.Empty;
			m_sSetFringe = string.Empty;
			DestroyOverlay();
			MarkDirty(NML_EVisualsApplyReason.WORLD);
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
			NML_EVisualsApplyReason reason = m_eDirtyReason;
			if (!m_bDirty)
			{
				if (camera != m_iCamera)
					reason = NML_EVisualsApplyReason.CAMERA;
				else if (controlled != m_pControlled)
					reason = NML_EVisualsApplyReason.CONTROLLED;
				else
					reason = NML_EVisualsApplyReason.LOWLIGHT;
			}

			if (m_Probe)
			{
				if (camera != m_iCamera)
					m_Probe.OnCameraChanged();

				if (controlled != m_pControlled)
					m_Probe.OnControlledChanged();
			}

			// changed mode: a respawn may reuse the camera index with a fresh stack, even when another reason
			// (settings, cycle) is pending in the same poll.
			bool reassert = controlled != m_pControlled || reason == NML_EVisualsApplyReason.WORLD;

			m_iCamera = camera;
			m_pControlled = controlled;
			m_bLowLightLut = lowLight;
			m_bDirty = false;
			Apply(preset, reason, reassert);
		}

		if (preset)
			UpdateParams(preset);
	}

	//------------------------------------------------------------------------------------------------
	protected NML_VisualsPreset GetActivePreset()
	{
		if (!m_bEnabled || m_bCycleOff)
			return null;

		return NML_VisualsPreset.Get(m_ePreset);
	}

	//------------------------------------------------------------------------------------------------
	//! Player controls (NML_VisualsPanel). Each change is saved to the player profile.
	string GetPresetLabel()
	{
		if (!IsActive())
			return "OFF";

		return typename.EnumToString(NML_EVisualsPreset, m_ePreset);
	}

	bool IsOverlaysOn() { return m_bVignette; }
	bool IsFringeOn() { return m_bLens; }

	//------------------------------------------------------------------------------------------------
	//! OFF -> LOW -> AUTHENTIC -> HEAVY -> EXTREME -> OFF
	void CyclePreset()
	{
		if (!IsActive())
		{
			m_bEnabled = true;
			m_ePreset = NML_EVisualsPreset.LOW;
		}
		else if (m_ePreset >= NML_EVisualsPreset.EXTREME)
		{
			m_bEnabled = false;
		}
		else
		{
			int next = m_ePreset + 1;
			m_ePreset = next;
		}

		if (IsActive())
			m_eLastActivePreset = m_ePreset;

		ApplyUserChange();
	}

	void ToggleOverlays() { m_bVignette = !m_bVignette; ApplyUserChange(); }
	void ToggleFringe() { m_bLens = !m_bLens; ApplyUserChange(); }

	//------------------------------------------------------------------------------------------------
	protected void ApplyUserChange()
	{
		MarkDirty(NML_EVisualsApplyReason.USER);
		SaveSettings(); // also refreshes the diag menu through the settings-changed invoker
		PrintFormat("[NML] Visuals: player set %1 (overlays %2, fringe %3)", GetPresetLabel(), m_bVignette, m_bLens);
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
	//! legacy (v1, default): clear every NML slot, then set the wanted ones.
	//! changed (-nmlvApply changed): on the same camera only slots whose material changed are written; unchanged
	//! slots are re-asserted on a controlled-entity or world change (a respawn may reuse the camera index with a
	//! fresh stack); slots are cleared only when their layer turns off.
	protected void Apply(NML_VisualsPreset preset, NML_EVisualsApplyReason reason, bool reassert)
	{
		m_iApplySets = 0;
		m_iApplyClears = 0;
		m_iApplyReasserts = 0;

		// Wanted material per slot; empty = not wanted.
		ResourceName lut;
		ResourceName trim;
		ResourceName fringe;
		int layers = GetLayers();
		if (preset)
		{
			if ((layers & NML_VisualsTestConfig.LAYER_LUT) != 0 && UseLayer("lut"))
			{
				lut = preset.m_sDayLut;
				if (m_bLowLightLut)
					lut = LUT_LOWLIGHT;
			}

			if ((layers & NML_VisualsTestConfig.LAYER_TRIM) != 0 && UseLayer("trim"))
			{
				trim = TRIM_OVER_VANILLA;
				if (m_bColormodLoaded)
					trim = TRIM_OVER_COLORMOD;
			}

			if ((layers & NML_VisualsTestConfig.LAYER_FRINGE) != 0 && UseLayer("fringe"))
				fringe = FRINGE;
		}

		bool sameCamera = m_pSlotsWorld && m_pSlotsWorld == m_pWorld && m_iSlotsCamera == m_iCamera;
		if (!m_Config.m_bApplyChanged || !sameCamera)
		{
			// v1: always clear the camera we last set first (also covers camera switches).
			ClearSlots();
			if (preset)
			{
				m_sSetLut = SetSlot(PRIORITY_LUT, PostProcessEffectType.ColorGrading, lut, m_sSetLut, false);
				m_sSetTrim = SetSlot(PRIORITY_TRIM, PostProcessEffectType.Colors, trim, m_sSetTrim, false);
				m_sSetFringe = SetSlot(m_iSlotFringe, PostProcessEffectType.ChromAber, fringe, m_sSetFringe, false);
			}
		}
		else
		{
			m_sSetLut = SetSlot(PRIORITY_LUT, PostProcessEffectType.ColorGrading, lut, m_sSetLut, reassert);
			m_sSetTrim = SetSlot(PRIORITY_TRIM, PostProcessEffectType.Colors, trim, m_sSetTrim, reassert);
			m_sSetFringe = SetSlot(m_iSlotFringe, PostProcessEffectType.ChromAber, fringe, m_sSetFringe, reassert);
		}

		ResetParamCache();

		if (m_sSetLut.IsEmpty() && m_sSetTrim.IsEmpty() && m_sSetFringe.IsEmpty())
		{
			m_pSlotsWorld = null;
			m_iSlotsCamera = -1;
		}
		else
		{
			m_pSlotsWorld = m_pWorld;
			m_iSlotsCamera = m_iCamera;
		}

		string mode = "legacy";
		if (m_Config.m_bApplyChanged)
			mode = "changed";

		string stage1 = string.Format(" reason=%1 layers=%2 mode=%3 fringeSlot=%4 sets=%5 clears=%6 reasserts=%7",
			typename.EnumToString(NML_EVisualsApplyReason, reason), NML_VisualsTestConfig.LayersToString(layers), mode, m_iSlotFringe,
			m_iApplySets, m_iApplyClears, m_iApplyReasserts);

		if (!preset)
			Print(string.Format("[NML] Visuals: cleared on camera %1", m_iCamera) + stage1);
		else
			Print(string.Format("[NML] Visuals: applied %1 on camera %2 (lut %3, overlays %4, lens %5)", typename.EnumToString(NML_EVisualsPreset, m_ePreset), m_iCamera, lut, m_bVignette, m_bLens) + stage1);

		if (m_Probe)
			m_Probe.OnApply(reason, m_iApplySets, m_iApplyClears, m_iApplyReasserts);
	}

	//------------------------------------------------------------------------------------------------
	//! Writes one NML slot on the current camera when the wanted material differs from what is there, or when
	//! asked to re-assert it. Returns the material now in the slot.
	protected ResourceName SetSlot(int priority, PostProcessEffectType type, ResourceName wanted, ResourceName current, bool reassert)
	{
		if (wanted == current)
		{
			if (!reassert || wanted.IsEmpty())
				return current;

			m_iApplyReasserts++;
		}

		m_pWorld.SetCameraPostProcessEffect(m_iCamera, priority, type, wanted);
		if (wanted.IsEmpty())
			m_iApplyClears++;
		else
			m_iApplySets++;

		return wanted;
	}

	//------------------------------------------------------------------------------------------------
	//! A reason is kept until the next Apply; the first one recorded wins (a user change that also saves
	//! settings is reported as USER, not as a settings reload).
	protected void MarkDirty(NML_EVisualsApplyReason reason)
	{
		if (!m_bDirty)
			m_eDirtyReason = reason;

		m_bDirty = true;
	}

	//------------------------------------------------------------------------------------------------
	//! Layers in use: -nmlvCycle layer sets, else -nmlvLayers, else the player's settings (v1).
	protected int GetLayers()
	{
		if (m_Config.m_bCycleMasks && m_iCycleLayers >= 0)
			return m_iCycleLayers;

		if (m_Config.m_bLayersSet)
			return m_Config.m_iLayers;

		int layers = NML_VisualsTestConfig.LAYER_LUT | NML_VisualsTestConfig.LAYER_TRIM;
		if (m_bLens)
			layers |= NML_VisualsTestConfig.LAYER_FRINGE;

		if (m_bVignette)
			layers |= NML_VisualsTestConfig.LAYERS_OVERLAY;

		return layers;
	}

	//------------------------------------------------------------------------------------------------
	//! -nmlvCycle: phases in ABBA order (A B B A A B B A ...) so slow drift cancels out.
	//! OFF/ON form: A = OFF, B = ON. Layer-set form: A and B are the two layer sets.
	protected void CycleTick()
	{
		m_iCyclePhase++;
		int step = m_iCyclePhase % 4;
		bool phaseA = step == 0 || step == 3;

		int layers = m_Config.m_iCycleLayersB;
		if (phaseA)
			layers = m_Config.m_iCycleLayersA;

		string label;
		if (m_Config.m_bCycleMasks)
		{
			label = "B:";
			if (phaseA)
				label = "A:";

			label += NML_VisualsTestConfig.LayersToString(layers);
		}
		else
		{
			label = "B:on";
			if (phaseA)
				label = "A:off";
		}

		// Close the running probe interval while the old phase's state is still in place.
		if (m_Probe)
			m_Probe.OnPhase(label);

		if (m_Config.m_bCycleMasks)
			m_iCycleLayers = layers;
		else
			m_bCycleOff = phaseA;

		MarkDirty(NML_EVisualsApplyReason.CYCLE);

		PrintFormat("[NML] Visuals: cycle phase %1 %2", m_iCyclePhase, label);
	}

	//------------------------------------------------------------------------------------------------
	//! Summary-time state for the probe line (never called per frame).
	string GetProbeState()
	{
		string view = "none";
		ChimeraCharacter character = ChimeraCharacter.Cast(m_pControlled);
		if (character)
		{
			view = "1P";
			CharacterControllerComponent controller = character.GetCharacterController();
			if (controller && controller.IsInThirdPersonView())
				view = "3P";

			if (controller && controller.IsWeaponADS())
				view += "+ads";

			if (character.IsInVehicle())
				view += "+veh";
		}
		else if (m_pControlled)
		{
			view = "other:" + m_pControlled.ClassName();
		}

		if (SCR_2DPIPSightsComponent.IsPIPActive())
			view += "+pip";

		string mode = "legacy";
		if (m_Config.m_bApplyChanged)
			mode = "changed";

		return string.Format("layers=%1 mode=%2 fringeSlot=%3 filter=%4 hideZero=%5 preset=%6 night=%7 view=%8 players=%9",
			NML_VisualsTestConfig.LayersToString(GetLayers()), mode, m_iSlotFringe, m_Config.m_bSettingsFilter, m_Config.m_bHideZero,
			GetPresetLabel(), m_fNight, view, GetGame().GetPlayerManager().GetPlayerCount());
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
		m_pSlotsWorld.SetCameraPostProcessEffect(m_iSlotsCamera, m_iSlotFringe, PostProcessEffectType.ChromAber, string.Empty);
		m_iApplyClears += 3;
		m_pSlotsWorld = null;
		m_iSlotsCamera = -1;
		m_sSetLut = string.Empty;
		m_sSetTrim = string.Empty;
		m_sSetFringe = string.Empty;
	}

	//------------------------------------------------------------------------------------------------
	protected void ResetParamCache()
	{
		m_fSetSaturation = -1;
		m_fSetFringe = -1;
	}

	//------------------------------------------------------------------------------------------------
	//! Preset x weather x time -> material parameters. Fairness: brightness is never raised; the dark
	//! uses the LowLight LUT, which only darkens.
	protected void UpdateParams(NML_VisualsPreset preset)
	{
		// Stronger presets react more to weather and time (m_fWeather), never by adding brightness.
		float w = preset.m_fWeather;
		float saturation = preset.m_fTrimSaturation;
		saturation *= 1 - Math.Clamp(0.15 * w * m_fOvercast, 0, 0.5);
		saturation *= 1 - Math.Clamp(0.20 * w * m_fRain, 0, 0.5);
		saturation *= 1 - Math.Clamp(0.15 * w * m_fDusk, 0, 0.5);		// mute warm dusk light, no orange cinema look

		Material trim = Material.GetOrLoadMaterial(TRIM_OVER_VANILLA, 0);
		if (m_bColormodLoaded)
			trim = Material.GetOrLoadMaterial(TRIM_OVER_COLORMOD, 0);

		if (trim)
		{
			m_fSetSaturation = PushParam(trim, "Saturation", saturation, m_fSetSaturation);
		}

		Material fringe = Material.GetOrLoadMaterial(FRINGE, 0);
		if (fringe && Math.AbsFloat(preset.m_fFringe - m_fSetFringe) > 0.00005)
		{
			fringe.SetParam("PowerX", preset.m_fFringe);
			fringe.SetParam("PowerY", preset.m_fFringe);
			m_fSetFringe = preset.m_fFringe;
		}

		int layers = GetLayers();
		if (m_wVignette)
			m_fOpVignette = PushOpacity(m_wVignette, layers & NML_VisualsTestConfig.LAYER_VIGNETTE, preset.m_fVignette, m_fOpVignette);

		if (m_wGrime)
		{
			// Grime is lighter than a night scene, so it fades out in the dark (night fairness).
			float grime = preset.m_fGrime * (1 + 0.5 * w * m_fRain) * (1 - m_fNight);
#ifdef ENABLE_DIAG
			if (m_bGrimeReview)
				grime = Math.Max(grime, 0.6);
#endif
			m_fOpGrime = PushOpacity(m_wGrime, layers & NML_VisualsTestConfig.LAYER_GRIME, Math.Clamp(grime, 0, 1), m_fOpGrime);
		}

		if (m_wGrimeHeavy)
		{
			// Same night fade and rain response as the base grime.
			float grimeHeavy = preset.m_fGrimeHeavy * (1 + 0.5 * w * m_fRain) * (1 - m_fNight);
#ifdef ENABLE_DIAG
			if (m_bGrimeReview)
				grimeHeavy = Math.Max(grimeHeavy, 0.6);
#endif
			m_fOpGrimeHeavy = PushOpacity(m_wGrimeHeavy, layers & NML_VisualsTestConfig.LAYER_GRIMEHEAVY, Math.Clamp(grimeHeavy, 0, 1), m_fOpGrimeHeavy);
		}
	}

	//------------------------------------------------------------------------------------------------
	//! v1 (default): SetOpacity every poll, widget always visible. A layer turned off by -nmlvLayers/-nmlvCycle
	//! is hidden. -nmlvHideZero hides a widget below 0.005 opacity and pushes opacity only when it changed.
	//! Returns the opacity last pushed.
	protected float PushOpacity(ImageWidget widget, int layerOn, float opacity, float last)
	{
		bool visible = layerOn != 0;
		if (visible && m_Config.m_bHideZero)
			visible = opacity >= 0.005;

		if (widget.IsVisible() != visible)
			widget.SetVisible(visible);

		if (!visible)
			return last;

		if (m_Config.m_bHideZero && Math.AbsFloat(opacity - last) < 0.002)
			return last;

		widget.SetOpacity(opacity);
		return opacity;
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

		bool show = preset && (GetLayers() & NML_VisualsTestConfig.LAYERS_OVERLAY) != 0 && UseLayer("overlay");
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

			m_wGrimeHeavy = ImageWidget.Cast(m_wOverlay.FindAnyWidget("GrimeHeavy"));
			if (m_wGrimeHeavy)
				m_wGrimeHeavy.SetOpacity(0);
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
		m_wGrimeHeavy = null;
		m_fOpVignette = -1;
		m_fOpGrime = -1;
		m_fOpGrimeHeavy = -1;
	}

#ifdef ENABLE_DIAG
	//------------------------------------------------------------------------------------------------
	//! Prototype controls (diag builds only; no settings dialog yet): diag menu "NML Visuals". Changes are saved.
	protected void RegisterDiag()
	{
		DiagMenu.RegisterMenu(DIAG_MENU, DIAG_MENU_NAME, "");
		DiagMenu.RegisterBool(DIAG_ENABLED, "", "Enabled", DIAG_MENU_NAME);
		DiagMenu.RegisterItem(DIAG_PRESET, "", "Preset", DIAG_MENU_NAME, "OFF,LOW,AUTHENTIC,HEAVY,EXTREME");
		DiagMenu.RegisterBool(DIAG_VIGNETTE, "", "Vignette + grime", DIAG_MENU_NAME);
		DiagMenu.RegisterBool(DIAG_LENS, "", "Chromatic fringe", DIAG_MENU_NAME);
		PushDiag();
	}

	//------------------------------------------------------------------------------------------------
	protected void PushDiag()
	{
		DiagMenu.SetValue(DIAG_ENABLED, m_bEnabled);
		DiagMenu.SetValue(DIAG_PRESET, m_ePreset);
		DiagMenu.SetValue(DIAG_VIGNETTE, m_bVignette);
		DiagMenu.SetValue(DIAG_LENS, m_bLens);
	}

	//------------------------------------------------------------------------------------------------
	protected void ApplyDiagPreset()
	{
		if (m_iDiagPreset <= NML_EVisualsPreset.OFF)
			return;

		m_ePreset = m_iDiagPreset;
		m_eLastActivePreset = m_ePreset;
		m_bDirty = true;
		PushDiag(); // PollDiag would otherwise read the menu and revert the forced preset
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
		bool vignette = DiagMenu.GetBool(DIAG_VIGNETTE);
		bool lens = DiagMenu.GetBool(DIAG_LENS);

		if (enabled == m_bEnabled && preset == m_ePreset && vignette == m_bVignette && lens == m_bLens)
			return;

		if (preset < NML_EVisualsPreset.OFF || preset > NML_EVisualsPreset.EXTREME)
			preset = NML_EVisualsPreset.AUTHENTIC;

		m_bEnabled = enabled;
		m_ePreset = preset;
		if (m_ePreset != NML_EVisualsPreset.OFF)
			m_eLastActivePreset = m_ePreset;

		m_bVignette = vignette;
		m_bLens = lens;
		m_bDirty = true;
		SaveSettings();
		PrintFormat("[NML] Visuals: settings changed (enabled %1, preset %2, overlays %3, lens %4)", m_bEnabled, typename.EnumToString(NML_EVisualsPreset, m_ePreset), m_bVignette, m_bLens);
	}
#endif
}

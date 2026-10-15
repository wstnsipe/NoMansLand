//! Applies the NML_Visuals look to the local camera (ADR 0007). Client only; nothing is replicated.
//! Started from the modded ArmaReforgerScripted.OnWorldPostProcess; polls at POLL_MS on the game call queue
//! and only re-applies effects when the settings, the camera, the world or the controlled entity change.
//! Own post-process materials in own slots after vanilla's 5-8 (Colors 5, RadialBlur 6, GaussFilter 7, DOF 8),
//! so the grade sits on top of whatever is underneath (vanilla, or Warfare-Colormod). OFF clears our slots only.
class NML_VisualsManager
{
	static const string SETTINGS_MODULE = "NML_VisualsSettings";

	// Post-process priorities (0-19, lower renders earlier). ADR 0007 slot table.
	static const int PRIORITY_GRADE = 10;
	static const int PRIORITY_GRAIN = 17;

	// Camera, world, menu and respawn changes are picked up within this interval.
	protected static const int POLL_MS = 100;

	// Warfare-Colormod (Myrove dependency). Its ScreenEffects_ColorPP.emat override fails to parse on game 1.8.0.13
	// ("Unknown keyword Offset"), so it currently changes nothing and both grade materials hold the same values.
	// If Colormod is fixed, retune NML_Visuals_Grade_Colormod.emat against it (ADR 0007).
	protected static const string COLORMOD_GUID = "65A2EA40DC9E632A";

	protected static const ResourceName GRADE_OVER_COLORMOD = "{086A86D87F5B514C}Materials/NML/Visuals/NML_Visuals_Grade_Colormod.emat";
	protected static const ResourceName GRADE_OVER_VANILLA = "{0DAD025814CB44FC}Materials/NML/Visuals/NML_Visuals_Grade_Vanilla.emat";
	protected static const ResourceName GRAIN = "{8E07B73376891360}Materials/NML/Visuals/NML_Visuals_Grain.emat";
	protected static const ResourceName OVERLAY_LAYOUT = "{7963C16C11AEF0F5}UI/Layouts/NML/Visuals/NML_VisualsOverlay.layout";

	protected static const float VIGNETTE_OPACITY_AUTHENTIC = 0.5;

#ifdef ENABLE_DIAG
	// DiagMenu ids are indexes below 512; vanilla script ids use roughly 0-265, NML_Tests uses 508-511.
	protected static const int DIAG_MENU = 500;
	protected static const int DIAG_ENABLED = 501;
	protected static const int DIAG_PRESET = 502;
	protected static const int DIAG_GRAIN = 503;
	protected static const int DIAG_VIGNETTE = 504;
	protected static const string DIAG_MENU_NAME = "NML Visuals";
#endif

	protected static ref NML_VisualsManager s_Instance;
	protected static bool s_bDisabled;

	protected bool m_bEnabled = true;
	protected NML_EVisualsPreset m_ePreset = NML_EVisualsPreset.AUTHENTIC;
	protected bool m_bGrain = true;
	protected bool m_bVignette = true;
	protected bool m_bColormodLoaded;

	protected bool m_bDirty = true;
	protected BaseWorld m_pWorld;
	protected int m_iCamera = -1;
	protected IEntity m_pControlled;

	// World and camera our slots were last set on (-1: none), so OFF and camera switches clear the right camera.
	protected BaseWorld m_pSlotsWorld;
	protected int m_iSlotsCamera = -1;

	protected Widget m_wOverlay;
	protected ImageWidget m_wVignette;

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

		if (cycle.ToInt() > 0)
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

		// Unknown values (e.g. a later preset read by an older build) fall back to AUTHENTIC.
		if (preset < NML_EVisualsPreset.OFF || preset > NML_EVisualsPreset.AUTHENTIC)
			preset = NML_EVisualsPreset.AUTHENTIC;

		m_ePreset = preset;
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
		GetGame().UserSettingsChanged();
		GetGame().SaveUserSettings();
	}

	//------------------------------------------------------------------------------------------------
	protected void Poll()
	{
#ifdef ENABLE_DIAG
		PollDiag();
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

		// The overlay draws above the HUD, so it is hidden while any menu (pause, map, inventory...) is open.
		MenuManager menuManager = GetGame().GetMenuManager();
		bool menuOpen = menuManager && menuManager.IsAnyMenuOpen();
		ShowOverlay(IsActive() && m_bVignette && !menuOpen);

		int camera = world.GetCurrentCameraId();
		if (camera < 0)
			return; // keep m_bDirty; apply once a camera exists

		// A respawn may reuse the camera index with a fresh post-process stack: re-apply on a new controlled entity.
		IEntity controlled;
		PlayerController playerController = GetGame().GetPlayerController();
		if (playerController)
			controlled = playerController.GetControlledEntity();

		if (!m_bDirty && camera == m_iCamera && controlled == m_pControlled)
			return;

		m_iCamera = camera;
		m_pControlled = controlled;
		m_bDirty = false;
		Apply();
	}

	//------------------------------------------------------------------------------------------------
	protected bool IsActive()
	{
		return m_bEnabled && m_ePreset != NML_EVisualsPreset.OFF;
	}

	//------------------------------------------------------------------------------------------------
	protected void Apply()
	{
		// Always clear the camera we last set first (also covers camera switches).
		ClearSlots();

		if (!IsActive())
		{
			PrintFormat("[NML] Visuals: cleared on camera %1", m_iCamera);
			return;
		}

		ResourceName grade = GRADE_OVER_VANILLA;
		if (m_bColormodLoaded)
			grade = GRADE_OVER_COLORMOD;

		m_pWorld.SetCameraPostProcessEffect(m_iCamera, PRIORITY_GRADE, PostProcessEffectType.Colors, grade);
		if (m_bGrain)
			m_pWorld.SetCameraPostProcessEffect(m_iCamera, PRIORITY_GRAIN, PostProcessEffectType.FilmGrain, GRAIN);

		m_pSlotsWorld = m_pWorld;
		m_iSlotsCamera = m_iCamera;
		PrintFormat("[NML] Visuals: applied %1 on camera %2 (grain %3, vignette %4)", typename.EnumToString(NML_EVisualsPreset, m_ePreset), m_iCamera, m_bGrain, m_bVignette);
	}

	//------------------------------------------------------------------------------------------------
	//! Clears only NML slots on the camera they were set on, which restores whatever is underneath.
	protected void ClearSlots()
	{
		if (!m_pSlotsWorld || m_iSlotsCamera < 0)
			return;

		m_pSlotsWorld.SetCameraPostProcessEffect(m_iSlotsCamera, PRIORITY_GRADE, PostProcessEffectType.Colors, string.Empty);
		m_pSlotsWorld.SetCameraPostProcessEffect(m_iSlotsCamera, PRIORITY_GRAIN, PostProcessEffectType.FilmGrain, string.Empty);
		m_pSlotsWorld = null;
		m_iSlotsCamera = -1;
	}

	//------------------------------------------------------------------------------------------------
	protected void ShowOverlay(bool show)
	{
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

			m_wOverlay = workspace.CreateWidgets(OVERLAY_LAYOUT);
			if (!m_wOverlay)
			{
				Print("[NML] Visuals: overlay layout failed to load", LogLevel.WARNING);
				return;
			}

			// Default z-order: above the game view and HUD. A negative z-order hides it under the game view.
			m_wVignette = ImageWidget.Cast(m_wOverlay.FindAnyWidget("Vignette"));
			if (m_wVignette)
				m_wVignette.SetOpacity(VIGNETTE_OPACITY_AUTHENTIC);
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
		m_wVignette = null;
	}

#ifdef ENABLE_DIAG
	//------------------------------------------------------------------------------------------------
	//! Prototype controls (diag builds only; no settings dialog yet): diag menu "NML Visuals". Changes are saved.
	protected void RegisterDiag()
	{
		DiagMenu.RegisterMenu(DIAG_MENU, DIAG_MENU_NAME, "");
		DiagMenu.RegisterBool(DIAG_ENABLED, "", "Enabled", DIAG_MENU_NAME);
		DiagMenu.RegisterItem(DIAG_PRESET, "", "Preset", DIAG_MENU_NAME, "OFF,AUTHENTIC");
		DiagMenu.RegisterBool(DIAG_GRAIN, "", "Grain", DIAG_MENU_NAME);
		DiagMenu.RegisterBool(DIAG_VIGNETTE, "", "Vignette", DIAG_MENU_NAME);
		PushDiag();
	}

	//------------------------------------------------------------------------------------------------
	protected void PushDiag()
	{
		DiagMenu.SetValue(DIAG_ENABLED, m_bEnabled);
		DiagMenu.SetValue(DIAG_PRESET, m_ePreset);
		DiagMenu.SetValue(DIAG_GRAIN, m_bGrain);
		DiagMenu.SetValue(DIAG_VIGNETTE, m_bVignette);
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
	//! A diag-menu edit is a user change: apply and save it.
	protected void PollDiag()
	{
		bool enabled = DiagMenu.GetBool(DIAG_ENABLED);
		int preset = DiagMenu.GetValue(DIAG_PRESET);
		bool grain = DiagMenu.GetBool(DIAG_GRAIN);
		bool vignette = DiagMenu.GetBool(DIAG_VIGNETTE);

		if (enabled == m_bEnabled && preset == m_ePreset && grain == m_bGrain && vignette == m_bVignette)
			return;

		if (preset < NML_EVisualsPreset.OFF || preset > NML_EVisualsPreset.AUTHENTIC)
			preset = NML_EVisualsPreset.AUTHENTIC;

		m_bEnabled = enabled;
		m_ePreset = preset;
		m_bGrain = grain;
		m_bVignette = vignette;
		m_bDirty = true;
		SaveSettings();
		PrintFormat("[NML] Visuals: settings changed (enabled %1, preset %2, grain %3, vignette %4)", m_bEnabled, typename.EnumToString(NML_EVisualsPreset, m_ePreset), m_bGrain, m_bVignette);
	}
#endif
}

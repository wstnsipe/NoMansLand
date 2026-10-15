//! Player-facing NML_Visuals controls (ADR 0007): a small panel shown inside the vanilla pause menu.
//! Built from NML_Visuals' own layout and attached as a child of the pause menu's root widget, so no
//! vanilla layout or menu config is overridden. Each click changes and saves the player's own settings.
class NML_VisualsPanel : ScriptedWidgetEventHandler
{
	protected static const ResourceName LAYOUT = "{31AC59F49850A51D}UI/Layouts/NML/Visuals/NML_VisualsPanel.layout";

	protected NML_VisualsManager m_Manager;
	protected Widget m_wRoot;
	protected MenuBase m_PauseMenu;

	//------------------------------------------------------------------------------------------------
	void NML_VisualsPanel(NML_VisualsManager manager)
	{
		m_Manager = manager;
	}

	//------------------------------------------------------------------------------------------------
	void ~NML_VisualsPanel()
	{
		Destroy();
	}

	//------------------------------------------------------------------------------------------------
	//! Shows the panel while the pause menu is the top menu, removes it otherwise.
	void Update()
	{
		MenuManager menuManager = GetGame().GetMenuManager();
		MenuBase top;
		if (menuManager)
			top = menuManager.GetTopMenu();

		if (!PauseMenuUI.Cast(top))
		{
			Destroy();
			return;
		}

		if (m_wRoot && m_PauseMenu == top)
			return;

		Destroy();
		Widget parent = top.GetRootWidget();
		WorkspaceWidget workspace = GetGame().GetWorkspace();
		if (!parent || !workspace)
			return;

		m_wRoot = workspace.CreateWidgets(LAYOUT, parent);
		if (!m_wRoot)
		{
			Print("[NML] Visuals: panel layout failed to load", LogLevel.WARNING);
			return;
		}

		m_wRoot.AddHandler(this);
		m_PauseMenu = top;
		Refresh();
	}

	//------------------------------------------------------------------------------------------------
	void Destroy()
	{
		if (m_wRoot)
			m_wRoot.RemoveFromHierarchy();

		m_wRoot = null;
		m_PauseMenu = null;
	}

	//------------------------------------------------------------------------------------------------
	protected void Refresh()
	{
		if (!m_wRoot)
			return;

		SetLabel("Preset", "Look: " + m_Manager.GetPresetLabel());
		SetLabel("Overlays", "Vignette and grime: " + OnOff(m_Manager.IsOverlaysOn()));
		SetLabel("Fringe", "Lens fringing: " + OnOff(m_Manager.IsFringeOn()));
	}

	//------------------------------------------------------------------------------------------------
	protected void SetLabel(string buttonName, string text)
	{
		Widget button = m_wRoot.FindAnyWidget(buttonName);
		if (!button)
			return;

		TextWidget label = TextWidget.Cast(button.FindAnyWidget("Label"));
		if (label)
			label.SetText(text);
	}

	//------------------------------------------------------------------------------------------------
	protected string OnOff(bool on)
	{
		if (on)
			return "ON";

		return "OFF";
	}

	//------------------------------------------------------------------------------------------------
	override bool OnClick(Widget w, int x, int y, int button)
	{
		// The click may arrive from the button's label; walk up to the named button.
		string name;
		while (w && w != m_wRoot)
		{
			if (ButtonWidget.Cast(w))
			{
				name = w.GetName();
				break;
			}

			w = w.GetParent();
		}

		switch (name)
		{
			case "Preset": m_Manager.CyclePreset(); break;
			case "Overlays": m_Manager.ToggleOverlays(); break;
			case "Fringe": m_Manager.ToggleFringe(); break;
			default: return false;
		}

		Refresh();
		return true;
	}
}

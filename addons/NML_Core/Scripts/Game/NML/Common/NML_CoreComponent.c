[ComponentEditorProps(category: "NML/Core", description: "NML core lifecycle host. Attach to the game mode entity. Holds no gameplay rules.")]
class NML_CoreComponentClass : SCR_BaseGameModeComponentClass
{
}

//! Technical host for NML systems on the engine game mode entity.
//! It only hooks the game mode lifecycle and exposes the core config to other NML systems.
//! It deliberately has no score, objective, round, victory or territory logic.
class NML_CoreComponent : SCR_BaseGameModeComponent
{
	[Attribute("{E4A2632941E707C7}Configs/NML/Core/NML_CoreConfig.conf", desc: "NML core config.", params: "conf class=NML_CoreConfig")]
	protected ResourceName m_sConfig;

	protected ref NML_CoreConfig m_pConfig;

	//------------------------------------------------------------------------------------------------
	//! \return the NML core component on the current game mode, or null if there is none.
	static NML_CoreComponent GetInstance()
	{
		BaseGameMode gameMode = GetGame().GetGameMode();
		if (!gameMode)
			return null;

		return NML_CoreComponent.Cast(gameMode.FindComponent(NML_CoreComponent));
	}

	//------------------------------------------------------------------------------------------------
	//! \return the loaded core config, or null if it failed to load.
	NML_CoreConfig GetConfig()
	{
		return m_pConfig;
	}

	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		LoadConfig();
	}

	//------------------------------------------------------------------------------------------------
	override void OnGameModeStart()
	{
		super.OnGameModeStart();

		if (m_pConfig && m_pConfig.IsVerboseLogging())
			NML_Log.Info("Game mode started.");
	}

	//------------------------------------------------------------------------------------------------
	//! Server-only (engine contract).
	override void OnPlayerConnected(int playerId)
	{
		super.OnPlayerConnected(playerId);

		if (m_pConfig && m_pConfig.IsVerboseLogging())
			NML_Log.Info(string.Format("Player connected: %1", playerId));
	}

	//------------------------------------------------------------------------------------------------
	protected void LoadConfig()
	{
		if (m_sConfig.IsEmpty())
		{
			NML_Log.Error("NML_CoreComponent has no config assigned.");
			return;
		}

		Resource holder = BaseContainerTools.LoadContainer(m_sConfig);
		if (!holder || !holder.IsValid())
		{
			NML_Log.Error(string.Format("NML core config could not be loaded: %1", m_sConfig));
			return;
		}

		m_pConfig = NML_CoreConfig.Cast(BaseContainerTools.CreateInstanceFromContainer(holder.GetResource().ToBaseContainer()));
		if (!m_pConfig)
			NML_Log.Error(string.Format("NML core config has the wrong class: %1", m_sConfig));
	}
}

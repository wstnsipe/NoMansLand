//! Minimal NML core configuration, loaded by NML_CoreComponent from Configs/NML/Core/NML_CoreConfig.conf.
//! Ships with the mod (visible to clients): never put secrets or exploit-sensitive values here.
[BaseContainerProps(configRoot: true)]
class NML_CoreConfig
{
	[Attribute("0", desc: "Log NML lifecycle events (game mode start, player connected) to the console.")]
	protected bool m_bVerboseLogging;

	[Attribute("", desc: "Arsenal curation policy (ADR 0005). An empty policy disables curation.", params: "conf class=NML_ArsenalPolicy")]
	protected ResourceName m_sArsenalPolicy;

	//------------------------------------------------------------------------------------------------
	bool IsVerboseLogging()
	{
		return m_bVerboseLogging;
	}

	//------------------------------------------------------------------------------------------------
	ResourceName GetArsenalPolicy()
	{
		return m_sArsenalPolicy;
	}
}

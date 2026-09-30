//! Minimal NML core configuration, loaded by NML_CoreComponent from Configs/NML/Core/NML_CoreConfig.conf.
//! Ships with the mod (visible to clients): never put secrets or exploit-sensitive values here.
[BaseContainerProps(configRoot: true)]
class NML_CoreConfig
{
	[Attribute("0", desc: "Log NML lifecycle events (game mode start, player connected) to the console.")]
	protected bool m_bVerboseLogging;

	//------------------------------------------------------------------------------------------------
	bool IsVerboseLogging()
	{
		return m_bVerboseLogging;
	}
}

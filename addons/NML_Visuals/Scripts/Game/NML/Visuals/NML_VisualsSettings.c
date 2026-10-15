//! Player-local NML_Visuals settings (ADR 0007). Stored in the player's own profile by the engine;
//! never sent to the server. Read and written through GetGame().GetGameUserSettings().GetModule("NML_VisualsSettings").
class NML_VisualsSettings : ModuleGameSettings
{
	[Attribute("1", desc: "Master switch for all NML_Visuals effects")]
	bool m_bEnabled;

	[Attribute("1", desc: "Look preset", enums: ParamEnumArray.FromEnum(NML_EVisualsPreset))]
	int m_iPreset;

	[Attribute("1", desc: "Film grain")]
	bool m_bGrain;

	[Attribute("1", desc: "Vignette overlay")]
	bool m_bVignette;
}

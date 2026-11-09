//! Why NML_VisualsManager re-applied its post-process slots (Stage 1 measurement, docs/visuals/v2/STAGE1-PLAN.md).
enum NML_EVisualsApplyReason
{
	WORLD,			// manager (re)started for a world, or the world changed
	CAMERA,			// current camera id changed
	CONTROLLED,		// controlled entity changed (respawn, drone, vehicle seat ...)
	LOWLIGHT,		// day/night LUT switch
	SETTINGS_OURS,	// a settings save changed NML_Visuals' own values
	SETTINGS_OTHER,	// a settings save that left NML_Visuals' values unchanged (another menu or mod)
	USER,			// pause-menu panel / diag input
	CYCLE			// -nmlvCycle phase switch
}

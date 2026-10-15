//! NML_Visuals entry point (ADR 0007): the game class exists on every machine for the whole session,
//! independent of game mode and camera. Each loaded world starts the manager; dedicated servers return
//! immediately inside it.
modded class ArmaReforgerScripted
{
	//------------------------------------------------------------------------------------------------
	override protected void OnWorldPostProcess(World world)
	{
		super.OnWorldPostProcess(world);
		NML_VisualsManager.Start();
	}
}

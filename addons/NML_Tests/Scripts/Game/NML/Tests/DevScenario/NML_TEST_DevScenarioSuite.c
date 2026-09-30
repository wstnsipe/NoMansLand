//! Smoke test for the DEV scenario (NML_Scenario_Dev): the world loads and has the
//! minimal free-for-all setup needed for players to spawn.
//! Run: tools/autotest-dev.ps1 (or -autotest NML_TEST_DevScenarioSuite on the diag game exe).
//! NML_Tests depends on NML_Scenario_Dev (.gproj) because these tests load its world and mission header.
class NML_TEST_DevScenarioSuite : SCR_AutotestSuiteBase
{
	static const ResourceName WORLD = "{6D161EF909ABF4A7}Worlds/NML/Dev/NML_Dev_Everon.ent";
	static const ResourceName MISSION_HEADER = "{C36902459603B85D}Missions/NML/NML_Dev_Everon.conf";

	override ResourceName GetWorldFile()
	{
		return WORLD;
	}
}

[Test(suite: NML_TEST_DevScenarioSuite)]
class NML_TEST_DevScenario_MissionHeader_PointsToDevWorld : SCR_AutotestCaseBase
{
	[TestStep(TestStage.Main)]
	bool Execute()
	{
		SCR_MissionHeader header = SCR_MissionHeader.Cast(SCR_MissionHeader.ReadMissionHeader(NML_TEST_DevScenarioSuite.MISSION_HEADER));
		AssertTrue(header != null, "Mission header loads as SCR_MissionHeader");
		if (header)
			AssertTrue(header.GetWorldResourceName() == NML_TEST_DevScenarioSuite.WORLD, "Mission header references the DEV world");

		return true;
	}
}

[Test(suite: NML_TEST_DevScenarioSuite)]
class NML_TEST_DevScenario_WorldLoaded_HasFreeForAllSetup : SCR_AutotestCaseBase
{
	[TestStep(TestStage.Main)]
	bool Execute()
	{
		AssertTrue(SCR_BaseGameMode.Cast(GetGame().GetGameMode()) != null, "World has an SCR_BaseGameMode");

		FactionManager factionManager = GetGame().GetFactionManager();
		AssertTrue(factionManager != null, "World has a faction manager");
		if (factionManager)
			AssertTrue(factionManager.GetFactionByKey("FFA") != null, "Faction manager provides the FFA faction");

		AssertTrue(GetGame().GetLoadoutManager() != null, "World has a loadout manager");

		array<SCR_SpawnPoint> spawnPoints = SCR_SpawnPoint.GetSpawnPoints();
		AssertTrue(spawnPoints && !spawnPoints.IsEmpty(), "World has at least one spawn point");

		return true;
	}
}

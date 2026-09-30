//! Tests for the NML_Core foundation (Stage 5.1), run in the DEV world.
//! Run: tools/autotest-dev.ps1 -Test NML_TEST_CoreSuite
class NML_TEST_CoreSuite : SCR_AutotestSuiteBase
{
	override ResourceName GetWorldFile()
	{
		return NML_TEST_DevScenarioSuite.WORLD;
	}
}

[Test(suite: NML_TEST_CoreSuite)]
class NML_TEST_Core_Component_IsOnGameMode : SCR_AutotestCaseBase
{
	[TestStep(TestStage.Main)]
	bool Execute()
	{
		NML_CoreComponent core = NML_CoreComponent.GetInstance();
		AssertTrue(core != null, "NML_CoreComponent is attached to the game mode");

		return true;
	}
}

[Test(suite: NML_TEST_CoreSuite)]
class NML_TEST_Core_Config_Loads : SCR_AutotestCaseBase
{
	[TestStep(TestStage.Main)]
	bool Execute()
	{
		NML_CoreComponent core = NML_CoreComponent.GetInstance();
		AssertTrue(core != null, "NML_CoreComponent is attached to the game mode");
		if (core)
			AssertTrue(core.GetConfig() != null, "NML_CoreConfig loaded from the assigned .conf");

		return true;
	}
}

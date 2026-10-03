//! Arsenal curation v1 tests (ADR 0005), run in the NML_Tests arsenal world (vanillaItems US/USSR, curation active).
//! The server-side RPC gate's wiring is proven by the dedicated-server + client forged-request run
//! (NML_TEST_ArsenalForgeComponent); these cases cover the policy, the gate decision and the client filter.
//! Run: tools/autotest-dev.ps1 -Test NML_TEST_ArsenalPolicySuite
class NML_TEST_ArsenalPolicySuite : SCR_AutotestSuiteBase
{
	static const ResourceName WORLD = "{C1CEEB690FC718DF}Worlds/NML/Tests/NML_Test_Arsenal.ent";

	static const ResourceName POLICY_SHIPPED_EMPTY = "{CCEBBE8E28920A10}Configs/NML/Arsenal/NML_ArsenalPolicy.conf";
	static const ResourceName POLICY_US_ONLY = "{DDCF7805AF4DDFD4}Configs/NML/Tests/Arsenal/NML_TEST_ArsenalPolicy_USOnly.conf";

	static const ResourceName US_RIFLE = "{3E413771E1834D2F}Prefabs/Weapons/Rifles/M16/Rifle_M16A2.et";
	static const ResourceName US_MAGAZINE = "{2EBF60EF24B108FC}Prefabs/Weapons/Magazines/Magazine_556x45_STANAG_30rnd_M855_Ball.et";
	static const ResourceName US_BANNED = "{9C5C20FB0E01E64F}Prefabs/Weapons/Launchers/M72/Launcher_M72A3.et";
	static const ResourceName USSR_RIFLE = "{FA5C25BF66A53DCF}Prefabs/Weapons/Rifles/AK74/Rifle_AK74.et";
	static const ResourceName USSR_BANNED = "{7A82FE978603F137}Prefabs/Weapons/Launchers/RPG7/Launcher_RPG7.et";

	static const string ARSENAL_US = "NML_TEST_Arsenal_US";
	static const string ARSENAL_USSR = "NML_TEST_Arsenal_USSR";
	static const string ARSENAL_NO_FACTION = "NML_TEST_Arsenal_NoFaction";

	//------------------------------------------------------------------------------------------------
	override ResourceName GetWorldFile()
	{
		return WORLD;
	}

	//------------------------------------------------------------------------------------------------
	static SCR_ArsenalComponent GetArsenal(string entityName)
	{
		IEntity entity = GetGame().GetWorld().FindEntityByName(entityName);
		if (!entity)
			return null;

		return SCR_ArsenalComponent.FindArsenalComponent(entity, false);
	}

	//------------------------------------------------------------------------------------------------
	static bool Contains(notnull array<SCR_ArsenalItem> items, ResourceName prefab)
	{
		foreach (SCR_ArsenalItem item : items)
		{
			if (item && item.GetItemResourceName() == prefab)
				return true;
		}

		return false;
	}

	//------------------------------------------------------------------------------------------------
	static array<SCR_ArsenalItem> FilteredItems(string arsenalName)
	{
		array<SCR_ArsenalItem> items = {};
		SCR_ArsenalComponent arsenal = GetArsenal(arsenalName);
		if (arsenal)
			arsenal.GetFilteredArsenalItems(items);

		return items;
	}

	//------------------------------------------------------------------------------------------------
	static SCR_Faction GetFaction(FactionKey key)
	{
		FactionManager factionManager = GetGame().GetFactionManager();
		if (!factionManager)
			return null;

		return SCR_Faction.Cast(factionManager.GetFactionByKey(key));
	}
}

[Test(suite: NML_TEST_ArsenalPolicySuite)]
class NML_TEST_Arsenal_TestWorld_PolicyActive : SCR_AutotestCaseBase
{
	[TestStep(TestStage.Main)]
	bool Execute()
	{
		NML_ArsenalPolicy policy = NML_ArsenalPolicy.GetActive();
		AssertTrue(policy != null, "Test world loads an arsenal policy through NML_CoreComponent");
		if (policy)
			AssertTrue(policy.IsCurationEnabled(), "Test policy has faction entries, so curation is active");

		AssertTrue(NML_TEST_ArsenalPolicySuite.GetArsenal(NML_TEST_ArsenalPolicySuite.ARSENAL_US) != null, "US test arsenal exists");
		AssertTrue(NML_TEST_ArsenalPolicySuite.GetArsenal(NML_TEST_ArsenalPolicySuite.ARSENAL_USSR) != null, "USSR test arsenal exists");
		AssertTrue(NML_TEST_ArsenalPolicySuite.GetArsenal(NML_TEST_ArsenalPolicySuite.ARSENAL_NO_FACTION) != null, "Factionless test arsenal exists");
		return true;
	}
}

[Test(suite: NML_TEST_ArsenalPolicySuite)]
class NML_TEST_Arsenal_EmptyPolicy_PreservesVanilla : SCR_AutotestCaseBase
{
	[TestStep(TestStage.Main)]
	bool Execute()
	{
		NML_ArsenalPolicy policy = NML_ArsenalPolicy.Load(NML_TEST_ArsenalPolicySuite.POLICY_SHIPPED_EMPTY);
		AssertTrue(policy != null, "Shipped (empty) policy loads");
		if (!policy)
			return true;

		AssertTrue(!policy.IsCurationEnabled(), "Empty policy disables curation");
		AssertTrue(policy.IsAllowed("US", NML_TEST_ArsenalPolicySuite.US_BANNED), "Empty policy allows any prefab");
		AssertTrue(policy.IsAllowed("", NML_TEST_ArsenalPolicySuite.US_RIFLE), "Empty policy allows factionless arsenals");

		array<SCR_ArsenalItem> vanillaItems = {};
		SCR_EntityCatalogManagerComponent catalogManager = SCR_EntityCatalogManagerComponent.GetInstance();
		SCR_Faction us = NML_TEST_ArsenalPolicySuite.GetFaction("US");
		AssertTrue(catalogManager != null && us != null, "Catalog manager and US faction exist");
		if (!catalogManager || !us)
			return true;

		catalogManager.GetFactionArsenalItems(vanillaItems, us);
		array<SCR_ArsenalItem> filtered = {};
		filtered.Copy(vanillaItems);
		policy.FilterItems("US", filtered);

		AssertTrue(!vanillaItems.IsEmpty(), "Vanilla US arsenal list is not empty");
		AssertTrue(filtered.Count() == vanillaItems.Count(), "Empty policy keeps every vanilla item");
		bool sameOrder = true;
		for (int i = 0; i < vanillaItems.Count() && i < filtered.Count(); i++)
		{
			if (vanillaItems[i] != filtered[i])
				sameOrder = false;
		}

		AssertTrue(sameOrder, "Empty policy keeps vanilla order");
		return true;
	}
}

[Test(suite: NML_TEST_ArsenalPolicySuite)]
class NML_TEST_Arsenal_AllowedItems_Visible : SCR_AutotestCaseBase
{
	[TestStep(TestStage.Main)]
	bool Execute()
	{
		array<SCR_ArsenalItem> us = NML_TEST_ArsenalPolicySuite.FilteredItems(NML_TEST_ArsenalPolicySuite.ARSENAL_US);
		AssertTrue(NML_TEST_ArsenalPolicySuite.Contains(us, NML_TEST_ArsenalPolicySuite.US_RIFLE), "US arsenal offers the allowed M16A2");
		AssertTrue(NML_TEST_ArsenalPolicySuite.Contains(us, NML_TEST_ArsenalPolicySuite.US_MAGAZINE), "US arsenal offers the allowed M855 magazine");
		return true;
	}
}

[Test(suite: NML_TEST_ArsenalPolicySuite)]
class NML_TEST_Arsenal_BannedItem_FilteredClientSide : SCR_AutotestCaseBase
{
	[TestStep(TestStage.Main)]
	bool Execute()
	{
		SCR_EntityCatalogManagerComponent catalogManager = SCR_EntityCatalogManagerComponent.GetInstance();
		SCR_Faction usFaction = NML_TEST_ArsenalPolicySuite.GetFaction("US");
		AssertTrue(catalogManager != null && usFaction != null, "Catalog manager and US faction exist");
		if (catalogManager && usFaction)
			AssertTrue(catalogManager.GetEntryWithPrefabFromFactionCatalog(EEntityCatalogType.ITEM, NML_TEST_ArsenalPolicySuite.US_BANNED, usFaction) != null, "Vanilla US catalog contains the M72 (so it is filtered, not absent)");

		array<SCR_ArsenalItem> us = NML_TEST_ArsenalPolicySuite.FilteredItems(NML_TEST_ArsenalPolicySuite.ARSENAL_US);
		AssertTrue(!NML_TEST_ArsenalPolicySuite.Contains(us, NML_TEST_ArsenalPolicySuite.US_BANNED), "US arsenal does not offer the banned M72");

		array<SCR_ArsenalItem> ussr = NML_TEST_ArsenalPolicySuite.FilteredItems(NML_TEST_ArsenalPolicySuite.ARSENAL_USSR);
		AssertTrue(!NML_TEST_ArsenalPolicySuite.Contains(ussr, NML_TEST_ArsenalPolicySuite.USSR_BANNED), "USSR arsenal does not offer the banned RPG-7");
		return true;
	}
}

[Test(suite: NML_TEST_ArsenalPolicySuite)]
class NML_TEST_Arsenal_ServerGate_RejectsBanned : SCR_AutotestCaseBase
{
	[TestStep(TestStage.Main)]
	bool Execute()
	{
		NML_ArsenalPolicy policy = NML_ArsenalPolicy.GetActive();
		AssertTrue(policy != null, "Active policy exists");
		if (!policy)
			return true;

		SCR_ArsenalComponent usArsenal = NML_TEST_ArsenalPolicySuite.GetArsenal(NML_TEST_ArsenalPolicySuite.ARSENAL_US);
		FactionKey key = NML_ArsenalPolicy.GetArsenalFactionKey(usArsenal);
		AssertTrue(key == "US", "US arsenal resolves to faction key US");
		AssertTrue(!policy.IsAllowed(key, NML_TEST_ArsenalPolicySuite.US_BANNED), "Gate decision rejects a direct request for the banned M72");
		AssertTrue(policy.IsAllowed(key, NML_TEST_ArsenalPolicySuite.US_MAGAZINE), "Gate decision lets an allowed request through to vanilla");
		return true;
	}
}

[Test(suite: NML_TEST_ArsenalPolicySuite)]
class NML_TEST_Arsenal_MissingOrUnlistedFaction_FailsClosed : SCR_AutotestCaseBase
{
	[TestStep(TestStage.Main)]
	bool Execute()
	{
		SCR_ArsenalComponent noFaction = NML_TEST_ArsenalPolicySuite.GetArsenal(NML_TEST_ArsenalPolicySuite.ARSENAL_NO_FACTION);
		AssertTrue(NML_ArsenalPolicy.GetArsenalFactionKey(noFaction).IsEmpty(), "Factionless arsenal has no faction key");
		array<SCR_ArsenalItem> items = NML_TEST_ArsenalPolicySuite.FilteredItems(NML_TEST_ArsenalPolicySuite.ARSENAL_NO_FACTION);
		AssertTrue(items.IsEmpty(), "Factionless arsenal offers nothing while curation is active");

		NML_ArsenalPolicy active = NML_ArsenalPolicy.GetActive();
		if (active)
			AssertTrue(!active.IsAllowed("", NML_TEST_ArsenalPolicySuite.US_RIFLE), "Requests at a factionless arsenal are rejected");

		NML_ArsenalPolicy usOnly = NML_ArsenalPolicy.Load(NML_TEST_ArsenalPolicySuite.POLICY_US_ONLY);
		AssertTrue(usOnly != null, "US-only policy loads");
		if (usOnly)
		{
			AssertTrue(!usOnly.IsFactionListed("USSR"), "USSR is unlisted in the US-only policy");
			AssertTrue(!usOnly.IsAllowed("USSR", NML_TEST_ArsenalPolicySuite.USSR_RIFLE), "Unlisted faction fails closed");
			AssertTrue(usOnly.IsAllowed("US", NML_TEST_ArsenalPolicySuite.US_RIFLE), "Listed faction still allows its items");
		}

		return true;
	}
}

[Test(suite: NML_TEST_ArsenalPolicySuite)]
class NML_TEST_Arsenal_FactionWarning_RateLimited : SCR_AutotestCaseBase
{
	[TestStep(TestStage.Main)]
	bool Execute()
	{
		AssertTrue(NML_ArsenalPolicy.NML_ShouldWarnFaction("NML_TEST_KEY_A"), "First warning for a key is allowed");
		AssertTrue(!NML_ArsenalPolicy.NML_ShouldWarnFaction("NML_TEST_KEY_A"), "Second warning for the same key is suppressed");
		AssertTrue(NML_ArsenalPolicy.NML_ShouldWarnFaction("NML_TEST_KEY_B"), "A different key warns independently");
		return true;
	}
}

[Test(suite: NML_TEST_ArsenalPolicySuite)]
class NML_TEST_Arsenal_USAndUSSR_Separated : SCR_AutotestCaseBase
{
	[TestStep(TestStage.Main)]
	bool Execute()
	{
		array<SCR_ArsenalItem> us = NML_TEST_ArsenalPolicySuite.FilteredItems(NML_TEST_ArsenalPolicySuite.ARSENAL_US);
		array<SCR_ArsenalItem> ussr = NML_TEST_ArsenalPolicySuite.FilteredItems(NML_TEST_ArsenalPolicySuite.ARSENAL_USSR);

		AssertTrue(!NML_TEST_ArsenalPolicySuite.Contains(us, NML_TEST_ArsenalPolicySuite.USSR_RIFLE), "US arsenal does not offer the AK-74");
		AssertTrue(NML_TEST_ArsenalPolicySuite.Contains(ussr, NML_TEST_ArsenalPolicySuite.USSR_RIFLE), "USSR arsenal offers the AK-74");
		AssertTrue(!NML_TEST_ArsenalPolicySuite.Contains(ussr, NML_TEST_ArsenalPolicySuite.US_RIFLE), "USSR arsenal does not offer the M16A2");

		NML_ArsenalPolicy policy = NML_ArsenalPolicy.GetActive();
		if (policy)
		{
			AssertTrue(!policy.IsAllowed("US", NML_TEST_ArsenalPolicySuite.USSR_RIFLE), "US arsenal requests for the AK-74 are rejected");
			AssertTrue(policy.IsAllowed("USSR", NML_TEST_ArsenalPolicySuite.USSR_RIFLE), "USSR arsenal requests for the AK-74 are allowed");
		}

		return true;
	}
}

[Test(suite: NML_TEST_ArsenalPolicySuite)]
class NML_TEST_Arsenal_ExactResourceNameMatch : SCR_AutotestCaseBase
{
	[TestStep(TestStage.Main)]
	bool Execute()
	{
		NML_ArsenalPolicy policy = NML_ArsenalPolicy.GetActive();
		AssertTrue(policy != null, "Active policy exists");
		if (!policy)
			return true;

		AssertTrue(policy.IsAllowed("US", NML_TEST_ArsenalPolicySuite.US_RIFLE), "Exact ResourceName is allowed");
		AssertTrue(!policy.IsAllowed("US", "Prefabs/Weapons/Rifles/M16/Rifle_M16A2.et"), "Path without GUID is rejected");
		AssertTrue(!policy.IsAllowed("US", "{3E413771E1834D2F}prefabs/weapons/rifles/m16/rifle_m16a2.et"), "Different case is rejected");
		AssertTrue(!policy.IsAllowed("US", NML_TEST_ArsenalPolicySuite.US_RIFLE + " "), "Trailing whitespace is rejected");
		AssertTrue(!policy.IsAllowed("us", NML_TEST_ArsenalPolicySuite.US_RIFLE), "Faction key match is exact");
		return true;
	}
}

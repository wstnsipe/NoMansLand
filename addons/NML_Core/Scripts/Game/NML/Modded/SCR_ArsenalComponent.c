//! Client-visible arsenal curation (ADR 0005). Runs on every machine, so it also limits server-side
//! consumers of the arsenal list (arsenal storage membership, resupply, AI takes, weapon racks).
//! Server authority for direct requests is in the modded SCR_ResourcePlayerControllerInventoryComponent.
modded class SCR_ArsenalComponent
{
	//------------------------------------------------------------------------------------------------
	override bool GetFilteredArsenalItems(out notnull array<SCR_ArsenalItem> filteredArsenalItems, EArsenalItemDisplayType requiresDisplayType = -1)
	{
		super.GetFilteredArsenalItems(filteredArsenalItems, requiresDisplayType);

		NML_ArsenalPolicy policy = NML_ArsenalPolicy.GetActive();
		if (!policy || !policy.IsCurationEnabled())
			return !filteredArsenalItems.IsEmpty();

		FactionKey factionKey = NML_ArsenalPolicy.GetArsenalFactionKey(this);
		if (!policy.IsFactionListed(factionKey) && NML_ArsenalPolicy.NML_ShouldWarnFaction(factionKey))
		{
			string factionLabel = factionKey;
			if (factionKey.IsEmpty())
				factionLabel = "none";

			NML_Log.Warning(string.Format("Arsenal faction '%1' has no arsenal policy entry; its arsenals offer nothing (fail closed).", factionLabel));
		}

		policy.FilterItems(factionKey, filteredArsenalItems);
		return !filteredArsenalItems.IsEmpty();
	}
}

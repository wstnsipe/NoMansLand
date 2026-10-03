//! Server-side arsenal curation gate (ADR 0005). Runs before vanilla handling of a client's arsenal request;
//! a request for a prefab the NML policy does not allow for the arsenal's faction is rejected and logged.
//! With no policy (or an empty one) every request goes to vanilla unchanged.
modded class SCR_ResourcePlayerControllerInventoryComponent
{
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	override protected void RpcAsk_ArsenalRequestItem_(RplId resourceComponentRplId, RplId storageComponentRplId, ResourceName resourceNameItem, EResourceType resourceType)
	{
		if (!NML_IsArsenalRequestAllowed(resourceComponentRplId, resourceNameItem))
			return;

		super.RpcAsk_ArsenalRequestItem_(resourceComponentRplId, storageComponentRplId, resourceNameItem, resourceType);
	}

	//------------------------------------------------------------------------------------------------
	//! \return false if the active NML arsenal policy forbids this prefab for the requested arsenal's faction.
	protected bool NML_IsArsenalRequestAllowed(RplId resourceComponentRplId, ResourceName prefab)
	{
		NML_ArsenalPolicy policy = NML_ArsenalPolicy.GetActive();
		if (!policy || !policy.IsCurationEnabled())
			return true;

		// Resolve the arsenal the same way vanilla does; invalid ids are left to vanilla, which rejects them.
		SCR_ResourceComponent resourceComponent = SCR_ResourceComponent.Cast(Replication.FindItem(resourceComponentRplId));
		if (!resourceComponent || !resourceComponent.GetOwner())
			return true;

		SCR_ArsenalComponent arsenal = SCR_ArsenalComponent.FindArsenalComponent(resourceComponent.GetOwner());
		if (!arsenal)
			return true;

		FactionKey factionKey = NML_ArsenalPolicy.GetArsenalFactionKey(arsenal);
		if (policy.IsAllowed(factionKey, prefab))
			return true;

		string factionLabel = factionKey;
		if (factionKey.IsEmpty())
			factionLabel = "none";

		if (!policy.IsFactionListed(factionKey) && NML_ArsenalPolicy.NML_ShouldWarnFaction(factionKey))
			NML_Log.Warning(string.Format("Arsenal faction '%1' has no arsenal policy entry; its arsenal requests are rejected (fail closed).", factionLabel));

		int playerId = -1;
		PlayerController controller = PlayerController.Cast(GetOwner());
		if (controller)
			playerId = controller.GetPlayerId();

		NML_Log.Warning(string.Format("Rejected arsenal request: player %1, arsenal faction %2, prefab %3", playerId, factionLabel, prefab));
		return false;
	}
}

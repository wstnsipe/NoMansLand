//! One faction's arsenal whitelist. Keyed by the arsenal's assigned FactionKey (ADR 0005).
[BaseContainerProps()]
class NML_ArsenalFactionPolicy
{
	[Attribute("", desc: "Exact FactionKey of the arsenal (e.g. US, USSR).")]
	FactionKey m_sFactionKey;

	[Attribute(desc: "Prefabs obtainable from arsenals of this faction. Exact ResourceName match ({GUID}path).", params: "et")]
	ref array<ResourceName> m_aAllowedPrefabs;
}

//! Arsenal curation policy (ADR 0005).
//! - No faction entries: curation is disabled and vanilla behaviour is unchanged.
//! - One or more entries: only listed prefabs are obtainable from arsenals of a listed faction;
//!   arsenals with a missing or unlisted faction fail closed.
//! Ships with the mod (visible to clients): clients filter their UI with the same policy the server enforces.
[BaseContainerProps(configRoot: true)]
class NML_ArsenalPolicy
{
	[Attribute(desc: "Per-faction whitelists. Leave empty to disable arsenal curation.")]
	protected ref array<ref NML_ArsenalFactionPolicy> m_aFactions;

	protected ref map<string, ref set<string>> m_mAllowed;

	//! Faction keys already warned about (once per key; "none" for a missing faction).
	protected static ref set<string> s_aWarnedFactionKeys = new set<string>();

	//------------------------------------------------------------------------------------------------
	//! \return the policy loaded from a NML_ArsenalPolicy .conf, or null if it cannot be loaded.
	static NML_ArsenalPolicy Load(ResourceName config)
	{
		if (config.IsEmpty())
			return null;

		Resource holder = BaseContainerTools.LoadContainer(config);
		if (!holder || !holder.IsValid())
		{
			NML_Log.Error(string.Format("Arsenal policy could not be loaded: %1", config));
			return null;
		}

		NML_ArsenalPolicy policy = NML_ArsenalPolicy.Cast(BaseContainerTools.CreateInstanceFromContainer(holder.GetResource().ToBaseContainer()));
		if (!policy)
		{
			NML_Log.Error(string.Format("Arsenal policy has the wrong class: %1", config));
			return null;
		}

		policy.Init();
		return policy;
	}

	//------------------------------------------------------------------------------------------------
	protected void Init()
	{
		m_mAllowed = new map<string, ref set<string>>();
		if (!m_aFactions)
			return;

		foreach (NML_ArsenalFactionPolicy faction : m_aFactions)
		{
			if (!faction || faction.m_sFactionKey.IsEmpty())
				continue;

			set<string> allowed = m_mAllowed.Get(faction.m_sFactionKey);
			if (!allowed)
			{
				allowed = new set<string>();
				m_mAllowed.Insert(faction.m_sFactionKey, allowed);
			}

			if (!faction.m_aAllowedPrefabs)
				continue;

			foreach (ResourceName prefab : faction.m_aAllowedPrefabs)
			{
				allowed.Insert(prefab);
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	//! \return true when the policy has at least one faction entry.
	bool IsCurationEnabled()
	{
		return m_mAllowed && !m_mAllowed.IsEmpty();
	}

	//------------------------------------------------------------------------------------------------
	//! \param[in] arsenalFactionKey the arsenal's assigned FactionKey; empty when the arsenal has no faction
	//! \param[in] prefab requested or listed prefab, compared exactly
	//! \return true if the prefab may be obtained from an arsenal of that faction
	bool IsAllowed(FactionKey arsenalFactionKey, ResourceName prefab)
	{
		if (!IsCurationEnabled())
			return true;

		if (arsenalFactionKey.IsEmpty())
			return false;

		set<string> allowed = m_mAllowed.Get(arsenalFactionKey);
		if (!allowed)
			return false;

		return allowed.Contains(prefab);
	}

	//------------------------------------------------------------------------------------------------
	//! \return true if an arsenal of this faction key has a policy entry (always true when curation is disabled).
	bool IsFactionListed(FactionKey arsenalFactionKey)
	{
		if (!IsCurationEnabled())
			return true;

		return !arsenalFactionKey.IsEmpty() && m_mAllowed.Contains(arsenalFactionKey);
	}

	//------------------------------------------------------------------------------------------------
	//! Removes items not allowed for the arsenal's faction, preserving order. No-op when curation is disabled.
	void FilterItems(FactionKey arsenalFactionKey, inout notnull array<SCR_ArsenalItem> items)
	{
		if (!IsCurationEnabled())
			return;

		for (int i = items.Count() - 1; i >= 0; i--)
		{
			SCR_ArsenalItem item = items[i];
			if (!item || !IsAllowed(arsenalFactionKey, item.GetItemResourceName()))
				items.RemoveOrdered(i);
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Rate limiter for the missing/unlisted-faction warning.
	//! \return true the first time it is called for a key, false afterwards.
	static bool NML_ShouldWarnFaction(string factionKey)
	{
		if (factionKey.IsEmpty())
			factionKey = "none";

		if (s_aWarnedFactionKeys.Contains(factionKey))
			return false;

		s_aWarnedFactionKeys.Insert(factionKey);
		return true;
	}

	//------------------------------------------------------------------------------------------------
	//! \return the arsenal's assigned FactionKey, or an empty key when it has no faction.
	static FactionKey GetArsenalFactionKey(SCR_ArsenalComponent arsenal)
	{
		if (!arsenal)
			return "";

		SCR_Faction faction = arsenal.GetAssignedFaction();
		if (!faction)
			return "";

		return faction.GetFactionKey();
	}

	//------------------------------------------------------------------------------------------------
	//! \return the active policy from the NML core component, or null when NML is not hosting the game mode.
	static NML_ArsenalPolicy GetActive()
	{
		NML_CoreComponent core = NML_CoreComponent.GetInstance();
		if (!core)
			return null;

		return core.GetArsenalPolicy();
	}
}

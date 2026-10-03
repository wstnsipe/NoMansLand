[ComponentEditorProps(category: "NML/Tests", description: "TEST ONLY. Sends forged arsenal requests from a client to prove NML server enforcement (ADR 0005).")]
class NML_TEST_ArsenalForgeComponentClass : SCR_BaseGameModeComponentClass
{
}

//! TEST ONLY (NML_Tests is never published). Placed only in the NML_Test_Arsenal world.
//! Diag menu "NML Tests" on a diag client:
//! - "Forge banned arsenal request": requests the nearest test arsenal's banned prefab, bypassing the UI filter.
//! - "Forge allowed arsenal request": requests its allowed prefab; vanilla handling must deliver it.
//! - "Dump nearest arsenal list": logs the list this client is shown for the nearest arsenal.
//! Also automatic, per character (re-armed on respawn): banned request 5 s after the character exists, allowed request at 10 s.
//! Standing within 3.5 m of an arsenal for 3 s runs the dump, then the banned request, then the allowed request 5 s later (once per arsenal).
//! The server log must show "[NML] Rejected arsenal request" for banned requests, and for every request at the factionless arsenal (fail closed).
class NML_TEST_ArsenalForgeComponent : SCR_BaseGameModeComponent
{
	// DiagMenu ids are indexes below 512; vanilla script ids (SCR_DebugMenuID) use roughly 0-265.
	protected static const int DIAG_MENU = 509;
	protected static const int DIAG_DUMP_LIST = 508;
	protected static const int DIAG_FORGE_BANNED = 510;
	protected static const int DIAG_FORGE_ALLOWED = 511;
	protected static const string DIAG_MENU_NAME = "NML Tests";

	protected static const ResourceName US_BANNED = "{9C5C20FB0E01E64F}Prefabs/Weapons/Launchers/M72/Launcher_M72A3.et";
	protected static const ResourceName US_ALLOWED = "{2EBF60EF24B108FC}Prefabs/Weapons/Magazines/Magazine_556x45_STANAG_30rnd_M855_Ball.et";
	protected static const ResourceName USSR_BANNED = "{7A82FE978603F137}Prefabs/Weapons/Launchers/RPG7/Launcher_RPG7.et";
	protected static const ResourceName USSR_ALLOWED = "{BBB50A815A2F916B}Prefabs/Weapons/Magazines/Magazine_545x39_AK_30rnd_Ball.et";

	protected static const float SEARCH_RADIUS = 50;
	protected static const float MARKER_RADIUS = 300;
	protected static const float MARKER_HEIGHT = 24;
	protected static const float MARKER_STEP = 4;

	// Client-side debug markers so the test arsenals are findable (diag build only; never published).
	protected ref array<ref Shape> m_aMarkerShapes = {};
	protected ref array<ref DebugTextWorldSpace> m_aMarkerTexts = {};
	protected ref set<IEntity> m_aMarked = new set<IEntity>();
	protected ref array<IEntity> m_aMarkFound = {};
	protected int m_iMarkTick;

	// Standing within DWELL_RADIUS of an arsenal for DWELL_MS runs the dump, then the banned and allowed forges, once per arsenal per character.
	protected static const float DWELL_RADIUS = 3.5;
	protected static const float DWELL_MS = 3000;

	protected IEntity m_pLastCharacter;
	protected IEntity m_pDwellEntity;
	protected float m_fDwellStart;
	protected ref set<IEntity> m_aDwellDone = new set<IEntity>();

	protected float m_fCharacterSeenAt = -1;
	protected IEntity m_pQueryNearest;
	protected float m_fQueryNearestDistSq;
	protected vector m_vQueryOrigin;
	protected int m_iAutoStep;

	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);

		if (System.IsConsoleApp() || SCR_Global.IsEditMode())
			return; // dedicated server or World Editor: nothing to forge

		DiagMenu.RegisterMenu(DIAG_MENU, DIAG_MENU_NAME, "");
		DiagMenu.RegisterBool(DIAG_DUMP_LIST, "", "Dump nearest arsenal list", DIAG_MENU_NAME);
		DiagMenu.RegisterBool(DIAG_FORGE_BANNED, "", "Forge banned arsenal request", DIAG_MENU_NAME);
		DiagMenu.RegisterBool(DIAG_FORGE_ALLOWED, "", "Forge allowed arsenal request", DIAG_MENU_NAME);
		GetGame().GetCallqueue().CallLater(Poll, 250, true);
	}

	//------------------------------------------------------------------------------------------------
	override void OnDelete(IEntity owner)
	{
		GetGame().GetCallqueue().Remove(Poll);
		super.OnDelete(owner);
	}

	//------------------------------------------------------------------------------------------------
	protected void Poll()
	{
		TrackCharacter();
		AutoForge();
		DwellForge();

		m_iMarkTick++;
		if (m_iMarkTick % 4 == 0)
			MarkArsenals();

		if (DiagMenu.GetBool(DIAG_DUMP_LIST))
		{
			DiagMenu.SetValue(DIAG_DUMP_LIST, 0);
			DumpNearestList();
		}

		if (DiagMenu.GetBool(DIAG_FORGE_BANNED))
		{
			DiagMenu.SetValue(DIAG_FORGE_BANNED, 0);
			Forge(true);
		}

		if (DiagMenu.GetBool(DIAG_FORGE_ALLOWED))
		{
			DiagMenu.SetValue(DIAG_FORGE_ALLOWED, 0);
			Forge(false);
		}
	}

	//------------------------------------------------------------------------------------------------
	//! A new controlled character (first spawn or respawn, e.g. after switching faction) re-arms the automatic forges.
	protected void TrackCharacter()
	{
		PlayerController controller = GetGame().GetPlayerController();
		if (!controller)
			return;

		IEntity character = controller.GetControlledEntity();
		if (character == m_pLastCharacter)
			return;

		m_pLastCharacter = character;
		m_fCharacterSeenAt = -1;
		m_iAutoStep = 0;
		m_pDwellEntity = null;
		m_aDwellDone.Clear();
	}

	//------------------------------------------------------------------------------------------------
	protected void DwellForge()
	{
		if (!m_pLastCharacter)
			return;

		string arsenalName;
		IEntity arsenalEntity = FindNearestArsenal(m_pLastCharacter, arsenalName);
		if (!arsenalEntity || vector.Distance(arsenalEntity.GetOrigin(), m_pLastCharacter.GetOrigin()) > DWELL_RADIUS)
		{
			m_pDwellEntity = null;
			return;
		}

		if (m_aDwellDone.Contains(arsenalEntity))
			return;

		float now = GetGame().GetWorld().GetWorldTime();
		if (arsenalEntity != m_pDwellEntity)
		{
			m_pDwellEntity = arsenalEntity;
			m_fDwellStart = now;
			return;
		}

		if (now - m_fDwellStart < DWELL_MS)
			return;

		m_aDwellDone.Insert(arsenalEntity);
		PrintFormat("[NML_TEST] Dwell at arsenal=%1: running dump and forges", arsenalName);
		DumpNearestList();
		Forge(true);
		GetGame().GetCallqueue().CallLater(Forge, 5000, false, false);
	}

	//------------------------------------------------------------------------------------------------
	protected void AutoForge()
	{
		if (m_iAutoStep >= 2)
			return;

		PlayerController controller = GetGame().GetPlayerController();
		if (!controller || !controller.GetControlledEntity())
			return;

		float now = GetGame().GetWorld().GetWorldTime();
		if (m_fCharacterSeenAt < 0)
			m_fCharacterSeenAt = now;

		if (m_iAutoStep == 0 && now - m_fCharacterSeenAt >= 5000)
		{
			m_iAutoStep = 1;
			Forge(true);
		}
		else if (m_iAutoStep == 1 && now - m_fCharacterSeenAt >= 10000)
		{
			m_iAutoStep = 2;
			Forge(false);
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Marks every arsenal within MARKER_RADIUS with a colored sphere pillar and a floating label (US blue, USSR red, no faction yellow),
	//! and logs where it is relative to the terrain and the local character.
	protected void MarkArsenals()
	{
		PlayerController controller = GetGame().GetPlayerController();
		if (!controller || !controller.GetControlledEntity())
			return;

		IEntity character = controller.GetControlledEntity();
		m_aMarkFound.Clear();
		m_vQueryOrigin = character.GetOrigin();
		GetGame().GetWorld().QueryEntitiesBySphere(m_vQueryOrigin, MARKER_RADIUS, QueryCollectArsenal);

		foreach (IEntity arsenalEntity : m_aMarkFound)
		{
			if (m_aMarked.Contains(arsenalEntity))
				continue;

			m_aMarked.Insert(arsenalEntity);
			string key = NML_ArsenalPolicy.GetArsenalFactionKey(SCR_ArsenalComponent.FindArsenalComponent(arsenalEntity, false));
			string label = key + " ARSENAL";
			int color = 0xFFFFFF00;
			if (key == "US")
				color = 0xFF3060FF;
			else if (key == "USSR")
				color = 0xFFFF3030;
			else
				label = "NO-FACTION ARSENAL";

			vector pos = arsenalEntity.GetOrigin();
			for (float h = 0; h <= MARKER_HEIGHT; h += MARKER_STEP)
			{
				m_aMarkerShapes.Insert(Shape.CreateSphere(color, ShapeFlags.NOZBUFFER, pos + Vector(0, h, 0), 0.6));
			}

			m_aMarkerTexts.Insert(DebugTextWorldSpace.Create(GetGame().GetWorld(), label, DebugTextFlags.CENTER | DebugTextFlags.FACE_CAMERA, pos[0], pos[1] + MARKER_HEIGHT + 2, pos[2], 40, color));
			PrintFormat("[NML_TEST] Marker: arsenal=%1 pos=%2 terrainY=%3 character=%4 dist=%5 visual=%6", key, pos, GetGame().GetWorld().GetSurfaceY(pos[0], pos[2]), m_vQueryOrigin, vector.Distance(pos, m_vQueryOrigin), arsenalEntity.GetVObject() != null);
		}
	}

	//------------------------------------------------------------------------------------------------
	protected bool QueryCollectArsenal(IEntity entity)
	{
		if (SCR_ArsenalComponent.FindArsenalComponent(entity, false))
			m_aMarkFound.Insert(entity);

		return true;
	}

	//------------------------------------------------------------------------------------------------
	//! Logs the item list this client is shown for the nearest arsenal (after NML's client filter), as objective evidence.
	protected void DumpNearestList()
	{
		PlayerController controller = GetGame().GetPlayerController();
		if (!controller || !controller.GetControlledEntity())
		{
			Print("[NML_TEST] Dump: no controlled character.", LogLevel.WARNING);
			return;
		}

		string arsenalName;
		IEntity arsenalEntity = FindNearestArsenal(controller.GetControlledEntity(), arsenalName);
		if (!arsenalEntity)
		{
			Print("[NML_TEST] Dump: no arsenal within 50 m.", LogLevel.WARNING);
			return;
		}

		SCR_ArsenalComponent arsenal = SCR_ArsenalComponent.FindArsenalComponent(arsenalEntity, false);
		array<SCR_ArsenalItem> items = {};
		arsenal.GetFilteredArsenalItems(items);

		string names;
		foreach (SCR_ArsenalItem item : items)
		{
			names += item.GetItemResourceName() + " ";
		}

		PrintFormat("[NML_TEST] Arsenal list: arsenal=%1 count=%2 items=%3", arsenalName, items.Count(), names);
	}

	//------------------------------------------------------------------------------------------------
	protected void Forge(bool banned)
	{
		PlayerController controller = GetGame().GetPlayerController();
		if (!controller || !controller.GetControlledEntity())
		{
			Print("[NML_TEST] Forge: no controlled character.", LogLevel.WARNING);
			return;
		}

		IEntity character = controller.GetControlledEntity();
		string arsenalName;
		IEntity arsenalEntity = FindNearestArsenal(character, arsenalName);
		if (!arsenalEntity)
		{
			Print("[NML_TEST] Forge: no arsenal within 50 m.", LogLevel.WARNING);
			return;
		}

		ResourceName prefab = PrefabFor(arsenalName, banned);
		SCR_ResourceComponent resourceComponent = SCR_ResourceComponent.FindResourceComponent(arsenalEntity);
		SCR_ResourcePlayerControllerInventoryComponent requester = SCR_ResourcePlayerControllerInventoryComponent.Cast(controller.FindComponent(SCR_ResourcePlayerControllerInventoryComponent));
		BaseInventoryStorageComponent storage = TargetStorage(character, prefab);
		if (!resourceComponent || !requester || !storage)
		{
			Print("[NML_TEST] Forge: missing resource, requester or storage component.", LogLevel.WARNING);
			return;
		}

		int before = CountPrefab(character, prefab);
		requester.RpcAsk_ArsenalRequestItem(Replication.FindItemId(resourceComponent), Replication.FindItemId(storage), prefab, EResourceType.SUPPLIES);
		PrintFormat("[NML_TEST] Forged arsenal request sent: banned=%1 arsenal=%2 prefab=%3 count before=%4", banned, arsenalName, prefab, before);
		GetGame().GetCallqueue().CallLater(LogCountAfter, 3000, false, character, prefab, banned, before);
	}

	//------------------------------------------------------------------------------------------------
	//! Logs whether the requested prefab reached the character's inventory (delivered = count increased).
	protected void LogCountAfter(IEntity character, ResourceName prefab, bool banned, int before)
	{
		if (!character)
			return;

		int after = CountPrefab(character, prefab);
		PrintFormat("[NML_TEST] Result: banned=%1 prefab=%2 count before=%3 after=%4 delivered=%5", banned, prefab, before, after, after > before);
	}

	//------------------------------------------------------------------------------------------------
	protected int CountPrefab(IEntity character, ResourceName prefab)
	{
		SCR_InventoryStorageManagerComponent manager = SCR_InventoryStorageManagerComponent.Cast(character.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!manager)
			return -1;

		array<IEntity> items = {};
		manager.GetItems(items);
		int count;
		foreach (IEntity item : items)
		{
			EntityPrefabData prefabData = item.GetPrefabData();
			if (prefabData && prefabData.GetPrefabName() == prefab)
				count++;
		}

		return count;
	}

	//------------------------------------------------------------------------------------------------
	//! Entity names are not available on clients, so search nearby entities for an arsenal component.
	//! \param[out] arsenalName the arsenal's faction key, or "none"
	protected IEntity FindNearestArsenal(IEntity character, out string arsenalName)
	{
		m_pQueryNearest = null;
		m_fQueryNearestDistSq = float.MAX;
		m_vQueryOrigin = character.GetOrigin();
		GetGame().GetWorld().QueryEntitiesBySphere(m_vQueryOrigin, SEARCH_RADIUS, QueryArsenal);
		if (!m_pQueryNearest)
			return null;

		arsenalName = NML_ArsenalPolicy.GetArsenalFactionKey(SCR_ArsenalComponent.FindArsenalComponent(m_pQueryNearest, false));
		if (arsenalName.IsEmpty())
			arsenalName = "none";

		return m_pQueryNearest;
	}

	//------------------------------------------------------------------------------------------------
	protected bool QueryArsenal(IEntity entity)
	{
		if (!SCR_ArsenalComponent.FindArsenalComponent(entity, false))
			return true;

		float distSq = vector.DistanceSq(m_vQueryOrigin, entity.GetOrigin());
		if (distSq < m_fQueryNearestDistSq)
		{
			m_pQueryNearest = entity;
			m_fQueryNearestDistSq = distSq;
		}

		return true;
	}

	//------------------------------------------------------------------------------------------------
	protected ResourceName PrefabFor(string arsenalName, bool banned)
	{
		if (arsenalName == "USSR")
		{
			if (banned)
				return USSR_BANNED;

			return USSR_ALLOWED;
		}

		if (banned)
			return US_BANNED;

		return US_ALLOWED;
	}

	//------------------------------------------------------------------------------------------------
	//! Same choice the inventory UI makes: a storage on the character that can take the prefab, else the root storage.
	protected BaseInventoryStorageComponent TargetStorage(IEntity character, ResourceName prefab)
	{
		SCR_CharacterInventoryStorageComponent root = SCR_CharacterInventoryStorageComponent.Cast(character.FindComponent(SCR_CharacterInventoryStorageComponent));
		SCR_InventoryStorageManagerComponent manager = SCR_InventoryStorageManagerComponent.Cast(character.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!root || !manager)
			return root;

		BaseInventoryStorageComponent storage = manager.FindActualStorageForItemResource(prefab, root);
		if (storage)
			return storage;

		return root;
	}
}

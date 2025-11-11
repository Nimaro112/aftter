#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"

#include "multiplayer/P2PMessages.h"
#include "ServerGameLogic.h"
#include "../EclipseStudio/Sources/ObjectsCode/weapons/WeaponArmory.h"

#include "sobj_DroppedItem.h"
#include "ObjectsCode/obj_ServerDropped.h"
#include "ObjectsCode/obj_ServerARDBOX.h"
#include "ObjectsCode/obj_ServerPlayer.h"

/*#ifdef ENABLE_GAMEBLOCKS
#include "GBClient/Inc/GBClient.h"
#include "GBClient/Inc/GBReservedEvents.h"

extern GameBlocks::GBClient* g_GameBlocks_Client;
extern GameBlocks::GBPublicSourceId g_GameBlocks_ServerID;
#endif //ENABLE_GAMEBLOCKS*/

const float DROPPED_ITEM_EXPIRE_TIME = 10.0f * 60.0f; // 10 min

IMPLEMENT_CLASS(obj_DroppedItem, "obj_DroppedItem", "Object");
AUTOREGISTER_CLASS(obj_DroppedItem);

extern wiInventoryItem RollItem(const LootBoxConfig* lootCfg, int depth);

obj_DroppedItem::obj_DroppedItem()
{
	srvObjParams_.ExpireTime = r3dGetTime() + DROPPED_ITEM_EXPIRE_TIME;	// setup here, as it can be overwritten
	AirDropPos = r3dPoint3D(0,0,0);
	AirBombPos = r3dPoint3D(0,0,0);
	m_IsOnTerrain = false;
	m_FirstTime = 0;
	m_FirstTimeBomb = 0;
	m_LootBoxID1 = 0;
	m_LootBoxID2 = 0;
	m_LootBoxID3 = 0;
	m_LootBoxID4 = 0;
	m_LootBoxID5 = 0;
	m_LootBoxID6 = 0;
	m_LootBoxID7 = 0;
	m_LootBoxID8 = 0;
	m_LootBoxID9 = 0;
	m_LootBoxID10 = 0;
	m_LootBoxID11 = 0;
	m_LootBoxID12 = 0;
	m_LootBoxID13 = 0;
	m_LootBoxID14 = 0;
	m_LootBoxID15 = 0;
	m_LootBoxID16 = 0;
	m_LootBoxID17 = 0;
	m_LootBoxID18 = 0;
	m_LootBoxID19 = 0;
	m_LootBoxID20 = 0;
	m_DefaultItems = 1;
	ExpireFirstTime = r3dGetTime();
}

obj_DroppedItem::~obj_DroppedItem()
{
}

BOOL obj_DroppedItem::OnCreate()
{
	r3dOutToLog("obj_DroppedItem %p created. %d, %f sec left\n", this, m_Item.itemID, srvObjParams_.ExpireTime - r3dGetTime());

	r3d_assert(NetworkLocal);
	r3d_assert(GetNetworkID());
	r3d_assert(m_Item.itemID);

	m_Item.ResetClipIfFull();

	//distToCreateSq = 130 * 130;
	//distToDeleteSq = 150 * 150;
	
	if (m_Item.itemID == 'ARDR')
	{
		expireAirDrop = r3dGetTime() + (5.0f * 60.0f);
		distToCreateSq = FLT_MAX;
		distToDeleteSq = FLT_MAX;
		// raycast down to earth in case world was changed or trying to spawn item in the air (player killed during jump)
		r3dPoint3D pos = gServerLogic.AdjustPositionToFloor(AirDropPos);
		SetPosition(AirDropPos);
	}
	else if (m_Item.itemID == 'ARBB')
	{
		expireAirBomb = r3dGetTime() + (5.0f * 60.0f);
		distToCreateSq = FLT_MAX;
		distToDeleteSq = FLT_MAX;
		// raycast down to earth in case world was changed or trying to spawn item in the air (player killed during jump)
		r3dPoint3D pos = gServerLogic.AdjustPositionToFloor(AirBombPos);
		SetPosition(AirBombPos);
	}
	else 
	{
		// overwrite object network visibility
		if (m_Item.itemID == 'FLPS' || m_Item.itemID == 'FLBB')
		{
			distToCreateSq = FLT_MAX;
			distToDeleteSq = FLT_MAX;
		}
		else
		{
			distToCreateSq = 130 * 130;
			distToDeleteSq = 150 * 150;
		}
		// raycast down to earth in case world was changed or trying to spawn item in the air (player killed during jump)
		r3dPoint3D pos = gServerLogic.AdjustPositionToFloor(GetPosition());
		SetPosition(pos);
	}

	gServerLogic.NetRegisterObjectToPeers(this);


	return parent::OnCreate();
}

BOOL obj_DroppedItem::OnDestroy()
{
	//r3dOutToLog("obj_DroppedItem %p destroyed\n", this);

	PKT_S2C_DestroyNetObject_s n;
	n.spawnID = toP2pNetId(GetNetworkID());
	gServerLogic.p2pBroadcastToActive(this, &n, sizeof(n));
	
	return parent::OnDestroy();
}

void obj_DroppedItem::OnExplode()
{
	for( GameObject* obj = GameWorld().GetFirstObject(); obj; obj = GameWorld().GetNextObject(obj))
	{
		if(obj->isObjType(OBJTYPE_Human))
		{
			float dist = (GetPosition() - obj->GetPosition()).LengthSq();
			if(dist < 80 ) // distance from bomb fall
			{
				obj_ServerPlayer* fromPlr = (obj_ServerPlayer*)obj;
				gServerLogic.ApplyDamage(fromPlr, obj, fromPlr->GetPosition()+r3dPoint3D(0,1,0), 100.0f, false, storecat_INVALID, 0);
				PKT_S2C_DropItemYPosition_s n;
				n.spawnID = toP2pNetId(GetNetworkID());
				gServerLogic.p2pBroadcastToActive(this, &n, sizeof(n));					
			}
			else if(dist < 300 ) // distance from bomb fall
			{
				obj_ServerPlayer* fromPlr = (obj_ServerPlayer*)obj;
				gServerLogic.ApplyDamage(fromPlr, obj, fromPlr->GetPosition()+r3dPoint3D(0,1,0), 30.0f, false, storecat_INVALID, 0);
				PKT_S2C_DropItemYPosition_s n;
				n.spawnID = toP2pNetId(GetNetworkID());
				gServerLogic.p2pBroadcastToActive(this, &n, sizeof(n));					
			}
			else if(dist < 600 ) // distance from bomb fall
			{
				obj_ServerPlayer* fromPlr = (obj_ServerPlayer*)obj;
				gServerLogic.ApplyDamage(fromPlr, obj, fromPlr->GetPosition()+r3dPoint3D(0,1,0), 15.0f, false, storecat_INVALID, 0);
				PKT_S2C_DropItemYPosition_s n;
				n.spawnID = toP2pNetId(GetNetworkID());
				gServerLogic.p2pBroadcastToActive(this, &n, sizeof(n));					
			}
		}
	}
}

BOOL obj_DroppedItem::Update()
{
	if(r3dGetTime() > srvObjParams_.ExpireTime)
	{
		setActiveFlag(0);
	}

	if (m_FirstTime == 1 && r3dGetTime()>ExpireFirstTime)
	{
		m_FirstTime = 0;
	}

	if (m_FirstTimeBomb == 0 && r3dGetTime()>ExpireFirstTime)
	{
		m_FirstTimeBomb = 0;
	}

	if (m_IsOnTerrain == false && m_Item.itemID == 'ARDR')
	{
		AirDropPos.y-=0.03f;
		SetPosition(AirDropPos);

		PKT_S2C_DropItemYPosition_s n;
		n.YPos = AirDropPos.y;
		n.spawnID = toP2pNetId(GetNetworkID());
		gServerLogic.p2pBroadcastToAll(&n, sizeof(n), true);

/////////////////////////////////
		R3DPROFILE_START("RayCast");
		PhysicsCallbackObject* target = NULL;
		const MaterialType *mt = 0;

		PxRaycastHit hit;
		PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK,0,0,0), PxSceneQueryFilterFlags(PxSceneQueryFilterFlag::eSTATIC|PxSceneQueryFilterFlag::eDYNAMIC));

		bool hitResult = g_pPhysicsWorld->raycastSingle(PxVec3(AirDropPos.x, AirDropPos.y + 0.5f, AirDropPos.z), PxVec3(0, -1, 0), 1.0f, PxSceneQueryFlags(PxSceneQueryFlag::eIMPACT), hit, filter);
		if( hitResult )
		{
			if( hit.shape && (target = static_cast<PhysicsCallbackObject*>(hit.shape->getActor().userData)))
			{
				r3dMaterial* material = 0;
				GameObject *gameObj = target->isGameObject();
				if(gameObj)
				{
					if(gameObj->isObjType(OBJTYPE_Terrain))
					{
						m_IsOnTerrain = true;
						int Items[20]; // amount of item dropped
						if (m_DefaultItems == 0)
						{
							int m_LootBoxID[20]; // amount of item dropped
							m_LootBoxID[0] = m_LootBoxID1;
							m_LootBoxID[1] = m_LootBoxID2;
							m_LootBoxID[2] = m_LootBoxID3;
							m_LootBoxID[3] = m_LootBoxID4;
							m_LootBoxID[4] = m_LootBoxID5;
							m_LootBoxID[5] = m_LootBoxID6;
							m_LootBoxID[6] = m_LootBoxID7;
							m_LootBoxID[7] = m_LootBoxID8;
							m_LootBoxID[8] = m_LootBoxID9;
							m_LootBoxID[9] = m_LootBoxID10;
							m_LootBoxID[10] = m_LootBoxID11;
							m_LootBoxID[11] = m_LootBoxID12;
							m_LootBoxID[12] = m_LootBoxID13;
							m_LootBoxID[13] = m_LootBoxID14;
							m_LootBoxID[14] = m_LootBoxID15;
							m_LootBoxID[15] = m_LootBoxID16;
							m_LootBoxID[16] = m_LootBoxID17;
							m_LootBoxID[17] = m_LootBoxID18;
							m_LootBoxID[18] = m_LootBoxID19;
							m_LootBoxID[19] = m_LootBoxID20;

							LootBoxConfig*	m_LootBoxConfig;
							for (int i = 0;i<20;i++) // amount of item dropped
							{
								m_LootBoxConfig = const_cast<LootBoxConfig*>(g_pWeaponArmory->getLootBoxConfig(m_LootBoxID[i]));
								if(m_LootBoxConfig != NULL)
								{
									if(m_LootBoxConfig->entries.size() != 0)
									{
										wiInventoryItem entrieID = RollItem(m_LootBoxConfig, 0);
										if (entrieID.itemID != NULL || entrieID.itemID != 0)
											Items[i] = entrieID.itemID;
										else
											Items[i] = GetItemDefault(i);
									}
									else {
										Items[i] = GetItemDefault(i);
									}
								}
								else {
									Items[i] = GetItemDefault(i);
								}
							}
						}
						

						if(m_Item.itemID == 'ARDR')
						{

							// spawn
							obj_ServerARDBOX* ARDBOX = (obj_ServerARDBOX*)srv_CreateGameObject("obj_ServerARDBOX", "ARDBOX", AirDropPos);
							ARDBOX->SetNetworkID(gServerLogic.GetFreeNetId());
							ARDBOX->NetworkLocal = true;
							ARDBOX->m_ItemID = 1996;
							ARDBOX->SetRotationVector(r3dPoint3D(0 + 180, 0, 0));
							ARDBOX->ARDBOXOwnerId = 909090;

							for (int i=0;i<20;i++)
							{
								if (m_DefaultItems == 1)
									Items[i] = GetItemDefault(i);
	
								wiInventoryItem wi;
								wi.itemID   = Items[i];
								wi.quantity = 1;
								ARDBOX->AddItemToARDBOX(wi, wi.quantity);
							}
						}
						wiInventoryItem Flare;
						Flare.itemID   = 'FLPS';
						Flare.quantity = 1;
						// create network object
						obj_DroppedItem* obj = (obj_DroppedItem*)srv_CreateGameObject("obj_DroppedItem", "obj_DroppedItem", AirDropPos);
						obj->SetPosition(AirDropPos);
						obj->SetNetworkID(gServerLogic.GetFreeNetId());
						obj->NetworkLocal = true;
						obj->m_Item          = Flare;
						obj->m_Item.quantity = 1;
						//r3dOutToLog("######## ha tocado suelo\n");
						//OnExplode();
						setActiveFlag(0);
					}
				}
			}
		}
		R3DPROFILE_END("RayCast");
/////////////////////////////////
	}
	
	if (m_IsOnTerrain == false && m_Item.itemID == 'ARBB')
	{
		AirBombPos.y-=1.03f;
		SetPosition(AirBombPos);

		PKT_S2C_BombItemYPosition_s n;
		n.YPos = AirBombPos.y;
		n.spawnID = toP2pNetId(GetNetworkID());
		gServerLogic.p2pBroadcastToAll(&n, sizeof(n), true);

/////////////////////////////////
		R3DPROFILE_START("RayCast");
		PhysicsCallbackObject* target = NULL;
		const MaterialType *mt = 0;

		PxRaycastHit hit;
		PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK,0,0,0), PxSceneQueryFilterFlags(PxSceneQueryFilterFlag::eSTATIC|PxSceneQueryFilterFlag::eDYNAMIC));

		bool hitResult = g_pPhysicsWorld->raycastSingle(PxVec3(AirBombPos.x, AirBombPos.y + 0.5f, AirBombPos.z), PxVec3(0, -1, 0), 1.0f, PxSceneQueryFlags(PxSceneQueryFlag::eIMPACT), hit, filter);
		if( hitResult )
		{
			if( hit.shape && (target = static_cast<PhysicsCallbackObject*>(hit.shape->getActor().userData)))
			{
				r3dMaterial* material = 0;
				GameObject *gameObj = target->isGameObject();
				if(gameObj)
				{
					if(gameObj->isObjType(OBJTYPE_Terrain))
					{
						m_IsOnTerrain = true;

						wiInventoryItem Flare;
						Flare.itemID   = 'FLBB';
						Flare.quantity = 1;
						// create network object
						obj_DroppedItem* obj = (obj_DroppedItem*)srv_CreateGameObject("obj_DroppedItem", "obj_DroppedItem", AirBombPos);
						obj->SetPosition(AirBombPos);
						obj->SetNetworkID(gServerLogic.GetFreeNetId());
						obj->NetworkLocal = true;
						obj->m_Item          = Flare;
						obj->m_Item.quantity = 1;
						//r3dOutToLog("######## ha tocado suelo\n");
						OnExplode();
						setActiveFlag(0);
					}
				}
			}
		}
		R3DPROFILE_END("RayCast");
/////////////////////////////////
	}

	return parent::Update();
}

int obj_DroppedItem::GetItemDefault(int i) // AIRDROP CONTAINER ITEMS
{
	switch(i)
	{
	case 0:	// Weapon category #1
		switch(u_random(76)) 
		{
			case 0:	
				return 101193;
			case 1:	
				return 100288;
			case 2:	
				return 101110;
			case 3:	
				return 101011;
			case 4:
				return 101028;
			case 5:
				return 101214;
			case 6:
				return 101027;
			case 7:
				return 101029;
			case 8:
				return 101002;
			case 9:
				return 101004;
			case 10:
				return 101005;
			case 11:
				return 101022;
			case 12:
				return 101032;
			case 13:
				return 101035;
			case 14:
				return 101040;
			case 15:
				return 101055;
			case 16:
				return 101063;
			case 17:
				return 101064;
			case 18:
				return 101093;
			case 19:
				return 101098;
			case 20:
				return 101103;
			case 21:
				return 101106;
			case 22:
				return 101107;
			case 23:
				return 101109;
			case 24:
				return 101111;
			case 25:
				return 101112;
			case 26:
				return 101115;
			case 27:
				return 101120;
			case 28:
				return 101158;
			case 29:
				return 101169;
			case 30:
				return 101172;
			case 31:
				return 101173;
			case 32:
				return 101180;
			case 33:
				return 101183;
			case 34:
				return 101197;
			case 35:
				return 101200;
			case 36:
				return 101201;
			case 37:
				return 101246;
			case 38:
				return 101320;
			case 39:
				return 101322;
			case 40:
				return 101342;
			case 41:
				return 101392;
			case 42:
				return 101330;
			case 43:
				return 101331;
			case 44:
				return 101332;
			case 45:
				return 101334;
			case 46:
				return 101068;
			case 47:
				return 101084;
			case 48:
				return 101085;
			case 49:
				return 101087;
			case 50:
				return 101247;
			case 51:
				return 101267;
			case 52:
				return 101278;
			case 53:
				return 101306;
			case 54:
				return 101307;
			case 55:
				return 101308;
			case 56:
				return 101309;
			case 57:
				return 101313;
			case 58:
				return 101314;
			case 59:
				return 101336;
			case 60:
				return 101339;
			case 61:
				return 101343;
			case 62:
				return 101344;
			case 63:
				return 101345;
			case 64:
				return 101346;
			case 65:
				return 101347;
			case 66:
				return 101381;
			case 67:
				return 101382;
			case 68:
				return 101383;
			case 69:
				return 101384;
			case 70:
				return 101385;
			case 71:
				return 101386;
			case 72:
				return 101388;
			case 73:
				return 101389;
			case 74:
				return 101390;
			case 75:
				return 101391;
		}
	case 1:	// Weapon category #2
		switch(u_random(76)) 
		{
			case 0:	
				return 101193;
			case 1:	
				return 100288;
			case 2:	
				return 101110;
			case 3:	
				return 101011;
			case 4:
				return 101028;
			case 5:
				return 101214;
			case 6:
				return 101027;
			case 7:
				return 101029;
			case 8:
				return 101002;
			case 9:
				return 101004;
			case 10:
				return 101005;
			case 11:
				return 101022;
			case 12:
				return 101032;
			case 13:
				return 101035;
			case 14:
				return 101040;
			case 15:
				return 101055;
			case 16:
				return 101063;
			case 17:
				return 101064;
			case 18:
				return 101093;
			case 19:
				return 101098;
			case 20:
				return 101103;
			case 21:
				return 101106;
			case 22:
				return 101107;
			case 23:
				return 101109;
			case 24:
				return 101111;
			case 25:
				return 101112;
			case 26:
				return 101115;
			case 27:
				return 101120;
			case 28:
				return 101158;
			case 29:
				return 101169;
			case 30:
				return 101172;
			case 31:
				return 101173;
			case 32:
				return 101180;
			case 33:
				return 101183;
			case 34:
				return 101197;
			case 35:
				return 101200;
			case 36:
				return 101201;
			case 37:
				return 101246;
			case 38:
				return 101320;
			case 39:
				return 101322;
			case 40:
				return 101342;
			case 41:
				return 101392;
			case 42:
				return 101330;
			case 43:
				return 101331;
			case 44:
				return 101332;
			case 45:
				return 101334;
			case 46:
				return 101068;
			case 47:
				return 101084;
			case 48:
				return 101085;
			case 49:
				return 101087;
			case 50:
				return 101247;
			case 51:
				return 101267;
			case 52:
				return 101278;
			case 53:
				return 101306;
			case 54:
				return 101307;
			case 55:
				return 101308;
			case 56:
				return 101309;
			case 57:
				return 101313;
			case 58:
				return 101314;
			case 59:
				return 101336;
			case 60:
				return 101339;
			case 61:
				return 101343;
			case 62:
				return 101344;
			case 63:
				return 101345;
			case 64:
				return 101346;
			case 65:
				return 101347;
			case 66:
				return 101381;
			case 67:
				return 101382;
			case 68:
				return 101383;
			case 69:
				return 101384;
			case 70:
				return 101385;
			case 71:
				return 101386;
			case 72:
				return 101388;
			case 73:
				return 101389;
			case 74:
				return 101390;
			case 75:
				return 101391;
		}
	case 2:	// Weapon category #3
		switch(u_random(76)) 
		{
			case 0:	
				return 101193;
			case 1:	
				return 100288;
			case 2:	
				return 101110;
			case 3:	
				return 101011;
			case 4:
				return 101028;
			case 5:
				return 101214;
			case 6:
				return 101027;
			case 7:
				return 101029;
			case 8:
				return 101002;
			case 9:
				return 101004;
			case 10:
				return 101005;
			case 11:
				return 101022;
			case 12:
				return 101032;
			case 13:
				return 101035;
			case 14:
				return 101040;
			case 15:
				return 101055;
			case 16:
				return 101063;
			case 17:
				return 101064;
			case 18:
				return 101093;
			case 19:
				return 101098;
			case 20:
				return 101103;
			case 21:
				return 101106;
			case 22:
				return 101107;
			case 23:
				return 101109;
			case 24:
				return 101111;
			case 25:
				return 101112;
			case 26:
				return 101115;
			case 27:
				return 101120;
			case 28:
				return 101158;
			case 29:
				return 101169;
			case 30:
				return 101172;
			case 31:
				return 101173;
			case 32:
				return 101180;
			case 33:
				return 101183;
			case 34:
				return 101197;
			case 35:
				return 101200;
			case 36:
				return 101201;
			case 37:
				return 101246;
			case 38:
				return 101320;
			case 39:
				return 101322;
			case 40:
				return 101342;
			case 41:
				return 101392;
			case 42:
				return 101330;
			case 43:
				return 101331;
			case 44:
				return 101332;
			case 45:
				return 101334;
			case 46:
				return 101068;
			case 47:
				return 101084;
			case 48:
				return 101085;
			case 49:
				return 101087;
			case 50:
				return 101247;
			case 51:
				return 101267;
			case 52:
				return 101278;
			case 53:
				return 101306;
			case 54:
				return 101307;
			case 55:
				return 101308;
			case 56:
				return 101309;
			case 57:
				return 101313;
			case 58:
				return 101314;
			case 59:
				return 101336;
			case 60:
				return 101339;
			case 61:
				return 101343;
			case 62:
				return 101344;
			case 63:
				return 101345;
			case 64:
				return 101346;
			case 65:
				return 101347;
			case 66:
				return 101381;
			case 67:
				return 101382;
			case 68:
				return 101383;
			case 69:
				return 101384;
			case 70:
				return 101385;
			case 71:
				return 101386;
			case 72:
				return 101388;
			case 73:
				return 101389;
			case 74:
				return 101390;
			case 75:
				return 101391;
		}
	case 3:	// Construction category #1
		switch(u_random(75)) 
		{
			case 0:	
				return 103000;
			case 1:	
				return 103001;
			case 2:	
				return 103002;
			case 3:	
				return 103003;
			case 4:
				return 103004;
			case 5:
				return 103005;
			case 6:
				return 103006;
			case 7:
				return 103007;
			case 8:
				return 103008;
			case 9:
				return 103009;
			case 10:
				return 103010;
			case 11:
				return 103011;
			case 12:
				return 103012;
			case 13:
				return 103013;
			case 14:
				return 103014;
			case 15:
				return 103015;
			case 16:
				return 103016;
			case 17:
				return 103017;
			case 18:
				return 103019;
			case 19:
				return 103018;
			case 20:
				return 104000;
			case 21:
				return 104001;
			case 22:
				return 104002;
			case 23:
				return 104003;
			case 24:
				return 104004;
			case 25:
				return 104005;
			case 26:
				return 104006;
			case 27:
				return 104007;
			case 28:
				return 104008;
			case 29:
				return 104009;
			case 30:
				return 104010;
			case 31:
				return 104011;
			case 32:
				return 104012;
			case 33:
				return 104013;
			case 34:
				return 104014;
			case 35:
				return 104015;
			case 36:
				return 104016;
			case 37:
				return 104017;
			case 38:
				return 104018;
			case 39:
				return 104019;
			case 40:
				return 104020;
			case 41:
				return 104021;
			case 42:
				return 104022;
			case 43:
				return 104023;
			case 44:
				return 104024;
			case 45:
				return 104025;
			case 46:
				return 104026;
			case 47:
				return 104027;
			case 48:
				return 104028;
			case 49:
				return 104029;
			case 50:
				return 104030;
			case 51:
				return 104031;
			case 52:
				return 104032;
			case 53:
				return 104033;
			case 54:
				return 104034;
			case 55:
				return 104035;
			case 56:
				return 104136;
			case 57:
				return 104137;
			case 58:
				return 104138;
			case 59:
				return 104139;
			case 60:
				return 104140;
			case 61:
				return 104141;
			case 62:
				return 104036;
			case 63:
				return 105000;
			case 64:
				return 105001;
			case 65:
				return 105002;
			case 66:
				return 105003;
			case 67:
				return 105004;
			case 68:
				return 105007;
			case 69:
				return 105008;
			case 70:
				return 105006;
			case 71:
				return 105017;
			case 72:
				return 105018;
			case 73:
				return 105019;
			case 74:
				return 105020;
		}
	case 4:	// Construction Category #2
		switch(u_random(75)) 
		{
			case 0:	
				return 103000;
			case 1:	
				return 103001;
			case 2:	
				return 103002;
			case 3:	
				return 103003;
			case 4:
				return 103004;
			case 5:
				return 103005;
			case 6:
				return 103006;
			case 7:
				return 103007;
			case 8:
				return 103008;
			case 9:
				return 103009;
			case 10:
				return 103010;
			case 11:
				return 103011;
			case 12:
				return 103012;
			case 13:
				return 103013;
			case 14:
				return 103014;
			case 15:
				return 103015;
			case 16:
				return 103016;
			case 17:
				return 103017;
			case 18:
				return 103019;
			case 19:
				return 103018;
			case 20:
				return 104000;
			case 21:
				return 104001;
			case 22:
				return 104002;
			case 23:
				return 104003;
			case 24:
				return 104004;
			case 25:
				return 104005;
			case 26:
				return 104006;
			case 27:
				return 104007;
			case 28:
				return 104008;
			case 29:
				return 104009;
			case 30:
				return 104010;
			case 31:
				return 104011;
			case 32:
				return 104012;
			case 33:
				return 104013;
			case 34:
				return 104014;
			case 35:
				return 104015;
			case 36:
				return 104016;
			case 37:
				return 104017;
			case 38:
				return 104018;
			case 39:
				return 104019;
			case 40:
				return 104020;
			case 41:
				return 104021;
			case 42:
				return 104022;
			case 43:
				return 104023;
			case 44:
				return 104024;
			case 45:
				return 104025;
			case 46:
				return 104026;
			case 47:
				return 104027;
			case 48:
				return 104028;
			case 49:
				return 104029;
			case 50:
				return 104030;
			case 51:
				return 104031;
			case 52:
				return 104032;
			case 53:
				return 104033;
			case 54:
				return 104034;
			case 55:
				return 104035;
			case 56:
				return 104136;
			case 57:
				return 104137;
			case 58:
				return 104138;
			case 59:
				return 104139;
			case 60:
				return 104140;
			case 61:
				return 104141;
			case 62:
				return 104036;
			case 63:
				return 105000;
			case 64:
				return 105001;
			case 65:
				return 105002;
			case 66:
				return 105003;
			case 67:
				return 105004;
			case 68:
				return 105007;
			case 69:
				return 105008;
			case 70:
				return 105006;
			case 71:
				return 105017;
			case 72:
				return 105018;
			case 73:
				return 105019;
			case 74:
				return 105020;
		}
	case 5:	// Construction Category #3
		switch(u_random(75)) 
		{
			case 0:	
				return 103000;
			case 1:	
				return 103001;
			case 2:	
				return 103002;
			case 3:	
				return 103003;
			case 4:
				return 103004;
			case 5:
				return 103005;
			case 6:
				return 103006;
			case 7:
				return 103007;
			case 8:
				return 103008;
			case 9:
				return 103009;
			case 10:
				return 103010;
			case 11:
				return 103011;
			case 12:
				return 103012;
			case 13:
				return 103013;
			case 14:
				return 103014;
			case 15:
				return 103015;
			case 16:
				return 103016;
			case 17:
				return 103017;
			case 18:
				return 103019;
			case 19:
				return 103018;
			case 20:
				return 104000;
			case 21:
				return 104001;
			case 22:
				return 104002;
			case 23:
				return 104003;
			case 24:
				return 104004;
			case 25:
				return 104005;
			case 26:
				return 104006;
			case 27:
				return 104007;
			case 28:
				return 104008;
			case 29:
				return 104009;
			case 30:
				return 104010;
			case 31:
				return 104011;
			case 32:
				return 104012;
			case 33:
				return 104013;
			case 34:
				return 104014;
			case 35:
				return 104015;
			case 36:
				return 104016;
			case 37:
				return 104017;
			case 38:
				return 104018;
			case 39:
				return 104019;
			case 40:
				return 104020;
			case 41:
				return 104021;
			case 42:
				return 104022;
			case 43:
				return 104023;
			case 44:
				return 104024;
			case 45:
				return 104025;
			case 46:
				return 104026;
			case 47:
				return 104027;
			case 48:
				return 104028;
			case 49:
				return 104029;
			case 50:
				return 104030;
			case 51:
				return 104031;
			case 52:
				return 104032;
			case 53:
				return 104033;
			case 54:
				return 104034;
			case 55:
				return 104035;
			case 56:
				return 104136;
			case 57:
				return 104137;
			case 58:
				return 104138;
			case 59:
				return 104139;
			case 60:
				return 104140;
			case 61:
				return 104141;
			case 62:
				return 104036;
			case 63:
				return 105000;
			case 64:
				return 105001;
			case 65:
				return 105002;
			case 66:
				return 105003;
			case 67:
				return 105004;
			case 68:
				return 105007;
			case 69:
				return 105008;
			case 70:
				return 105006;
			case 71:
				return 105017;
			case 72:
				return 105018;
			case 73:
				return 105019;
			case 74:
				return 105020;
		}
	case 6:	// Grenade category #1
		switch(u_random(20)) 
		{
			case 0:	
				return 101311;
			case 1:	
				return 101325;
			case 2:	
				return 101326;
			case 3:	
				return 101327;
			case 4:
				return 101328;
			case 5:
				return 101329;
			case 6:
				return 101312;
			case 7:
				return 101147;
			case 8:
				return 101148;
			case 9:
				return 101149;
			case 10:
				return 101150;
			case 11:
				return 101151;
			case 12:
				return 101152;
			case 13:
				return 101153;
			case 14:
				return 101154;
			case 15:
				return 101310;
			case 16:
				return 101404;
			case 17:
				return 101403;
			case 18:
				return 101405;
			case 19:
				return 101416;
		}
	case 7:	// Grenade category #2
		switch(u_random(20)) 
		{
			case 0:	
				return 101311;
			case 1:	
				return 101325;
			case 2:	
				return 101326;
			case 3:	
				return 101327;
			case 4:
				return 101328;
			case 5:
				return 101329;
			case 6:
				return 101312;
			case 7:
				return 101147;
			case 8:
				return 101148;
			case 9:
				return 101149;
			case 10:
				return 101150;
			case 11:
				return 101151;
			case 12:
				return 101152;
			case 13:
				return 101153;
			case 14:
				return 101154;
			case 15:
				return 101310;
			case 16:
				return 101404;
			case 17:
				return 101403;
			case 18:
				return 101405;
			case 19:
				return 101416;
		}
	case 8:	// Grenade Shield category #3
		switch(u_random(20)) 
		{
			case 0:	
				return 101311;
			case 1:	
				return 101325;
			case 2:	
				return 101326;
			case 3:	
				return 101327;
			case 4:
				return 101328;
			case 5:
				return 101329;
			case 6:
				return 101312;
			case 7:
				return 101147;
			case 8:
				return 101148;
			case 9:
				return 101149;
			case 10:
				return 101150;
			case 11:
				return 101151;
			case 12:
				return 101152;
			case 13:
				return 101153;
			case 14:
				return 101154;
			case 15:
				return 101310;
			case 16:
				return 101404;
			case 17:
				return 101403;
			case 18:
				return 101405;
			case 19:
				return 101416;
		}
	case 9:	// Health category #1
		switch(u_random(17)) 
		{
			case 0:	
				return 101301;
			case 1:	
				return 101302;
			case 2:	
				return 101261;
			case 3:	
				return 101304;
			case 4:
				return 101256;
			case 5:
				return 101262;
			case 6:
				return 100033;
			case 7:
				return 100031;
			case 8:
				return 100028;
			case 9:
				return 100036;
			case 10:
				return 100038;
			case 11:
				return 100034;
			case 12:
				return 100027;
			case 13:
				return 100035;
			case 14:
				return 100124;
			case 15:
				return 100125;
			case 16:
				return 100080;
		}
	case 10:	// Health category #2
		switch(u_random(17)) 
		{
			case 0:	
				return 101301;
			case 1:	
				return 101302;
			case 2:	
				return 101261;
			case 3:	
				return 101304;
			case 4:
				return 101256;
			case 5:
				return 101262;
			case 6:
				return 100033;
			case 7:
				return 100031;
			case 8:
				return 100028;
			case 9:
				return 100036;
			case 10:
				return 100038;
			case 11:
				return 100034;
			case 12:
				return 100027;
			case 13:
				return 100035;
			case 14:
				return 100124;
			case 15:
				return 100125;
			case 16:
				return 100080;
		}
	case 11:	// Health category #2
		switch(u_random(17)) 
		{
			case 0:	
				return 101301;
			case 1:	
				return 101302;
			case 2:	
				return 101261;
			case 3:	
				return 101304;
			case 4:
				return 101256;
			case 5:
				return 101262;
			case 6:
				return 100033;
			case 7:
				return 100031;
			case 8:
				return 100028;
			case 9:
				return 100036;
			case 10:
				return 100038;
			case 11:
				return 100034;
			case 12:
				return 100027;
			case 13:
				return 100035;
			case 14:
				return 100124;
			case 15:
				return 100125;
			case 16:
				return 100080;
		}
	case 12:	// Water category #1
		switch(u_random(4)) 
		{
			case 0:	
				return 100056;
			case 1:	
				return 100057;
			case 2:	
				return 100233;
			case 3:	
				return 100234;
		}
	case 13:	// Water category #2
		switch(u_random(4)) 
		{
			case 0:	
				return 100056;
			case 1:	
				return 100057;
			case 2:	
				return 100233;
			case 3:	
				return 100234;
		}
	case 14:	// Water category #3
		switch(u_random(4)) 
		{
			case 0:	
				return 100056;
			case 1:	
				return 100057;
			case 2:	
				return 100233;
			case 3:	
				return 100234;
		}
	case 15:	// Survival category #1
		switch(u_random(20)) 
		{
			case 0:	
				return 103023;
			case 1:	
				return 105010;
			case 2:	
				return 111363;
			case 3:	
				return 111364;
			case 4:
				return 111364;
			case 5:
				return 111366;
			case 6:
				return 111367;
			case 7:
				return 111368;
			case 8:
				return 101360;
			case 9:
				return 101361;
			case 10:
				return 101323;
			case 11:
				return 101408;
			case 12:
				return 101315;
			case 13:
				return 101319;
			case 14:
				return 101305;
			case 15:
				return 103020;
			case 16:
				return 101399;
			case 17:
				return 101400;
			case 18:
				return 101358;
			case 19:
				return 101359;
		}
	case 16:	// Survival category #2
		switch(u_random(20)) 
		{
			case 0:	
				return 103023;
			case 1:	
				return 105010;
			case 2:	
				return 111363;
			case 3:	
				return 111364;
			case 4:
				return 111364;
			case 5:
				return 111366;
			case 6:
				return 111367;
			case 7:
				return 111368;
			case 8:
				return 101360;
			case 9:
				return 101361;
			case 10:
				return 101323;
			case 11:
				return 101408;
			case 12:
				return 101315;
			case 13:
				return 101319;
			case 14:
				return 101305;
			case 15:
				return 103020;
			case 16:
				return 101399;
			case 17:
				return 101400;
			case 18:
				return 101358;
			case 19:
				return 101359;
		}
	case 17:	// Survival category #3
		switch(u_random(20)) 
		{
			case 0:	
				return 103023;
			case 1:	
				return 105010;
			case 2:	
				return 111363;
			case 3:	
				return 111364;
			case 4:
				return 111364;
			case 5:
				return 111366;
			case 6:
				return 111367;
			case 7:
				return 111368;
			case 8:
				return 101360;
			case 9:
				return 101361;
			case 10:
				return 101323;
			case 11:
				return 101408;
			case 12:
				return 101315;
			case 13:
				return 101319;
			case 14:
				return 101305;
			case 15:
				return 103020;
			case 16:
				return 101399;
			case 17:
				return 101400;
			case 18:
				return 101358;
			case 19:
				return 101359;
		}
	case 18:	// Gear Shield category #2
		switch(u_random(13)) 
		{
			case 0:	
				return 20540;
			case 1:	
				return 20642;
			case 2:	
				return 20643;
			case 3:	
				return 20644;
			case 4:
				return 20840;
			case 5:
				return 20645;
			case 6:
				return 20646;
			case 7:
				return 20647;
			case 8:
				return 20841;
			case 9:
				return 20620;
			case 10:
				return 20641;
			case 11:
				return 20828;
			case 12:
				return 20842;
		}
	case 19:	// Gear Shield category #2
		switch(u_random(13)) 
		{
			case 0:	
				return 20540;
			case 1:	
				return 20642;
			case 2:	
				return 20643;
			case 3:	
				return 20644;
			case 4:
				return 20840;
			case 5:
				return 20645;
			case 6:
				return 20646;
			case 7:
				return 20647;
			case 8:
				return 20841;
			case 9:
				return 20620;
			case 10:
				return 20641;
			case 11:
				return 20828;
			case 12:
				return 20842;
		}
	}
	return 0;
}

DefaultPacket* obj_DroppedItem::NetGetCreatePacket(int* out_size)
{
	if (m_Item.itemID == 'ARDR')
	{
		SetPosition(AirDropPos);
		PKT_S2C_AirDropMapUpdate_s AirDrop;
		AirDrop.location	= AirDropPos;
		AirDrop.m_time		= expireAirDrop; // 30 min to expire
		gServerLogic.p2pBroadcastToAll(&AirDrop, sizeof(AirDrop), true);
	}
	if (m_Item.itemID == 'ARBB')
	{
		SetPosition(AirBombPos);
		PKT_S2C_AirBombMapUpdate_s AirBomb;
		AirBomb.location	= AirBombPos;
		AirBomb.m_time		= expireAirBomb; // 1 min to expire
		gServerLogic.p2pBroadcastToAll(&AirBomb, sizeof(AirBomb), true);
	}

	static PKT_S2C_CreateDroppedItem_s n;
	n.spawnID = toP2pNetId(GetNetworkID());
	n.pos     = GetPosition();
	n.Item    = m_Item;
	n.FirstTime = m_FirstTime;
	n.FirstTimeBomb = m_FirstTimeBomb;

	*out_size = sizeof(n);
	return &n;
}

void obj_DroppedItem::LoadServerObjectData()
{
	// deserialize from xml
	IServerObject::CSrvObjXmlReader xml(srvObjParams_.Var1);
	m_Item.itemID      = srvObjParams_.ItemID;
	m_Item.InventoryID = xml.xmlObj.attribute("iid").as_int64();
	m_Item.quantity    = xml.xmlObj.attribute("q").as_int();
	m_Item.Var1        = xml.xmlObj.attribute("v1").as_int();
	m_Item.Var2        = xml.xmlObj.attribute("v2").as_int();
	m_Item.Var3        = xml.xmlObj.attribute("v3").as_int();
}

void obj_DroppedItem::SaveServerObjectData()
{
	srvObjParams_.ItemID     = m_Item.itemID;

	char strInventoryID[64];
	sprintf(strInventoryID, "%I64d", m_Item.InventoryID);

	IServerObject::CSrvObjXmlWriter xml;
	xml.xmlObj.append_attribute("iid") = strInventoryID;
	xml.xmlObj.append_attribute("q")   = m_Item.quantity;
	xml.xmlObj.append_attribute("v1")  = m_Item.Var1;
	xml.xmlObj.append_attribute("v2")  = m_Item.Var2;
	xml.xmlObj.append_attribute("v3")  = m_Item.Var3;
	xml.save(srvObjParams_.Var1);
}
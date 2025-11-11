#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"

#include "multiplayer/P2PMessages.h"

#include "obj_ServerPlayer.h"
#include "obj_ServerFarmBlock.h"
#include "ServerGameLogic.h"
#include "../EclipseStudio/Sources/ObjectsCode/weapons/WeaponArmory.h"
#include "../../GameEngine/ai/AutodeskNav/AutodeskNavMesh.h"
#include "Async_ServerObjects.h"

IMPLEMENT_CLASS(obj_ServerFarmBlock, "obj_ServerFarmBlock", "Object");
AUTOREGISTER_CLASS(obj_ServerFarmBlock);

const static int FARM_EXPIRE_TIME = 30 * 24 * 60 * 60; // farm will expire after 30 days
const static int FARM_RIPE_TIME = 1 * 60 * 60; // farm will produce one item each hour

obj_ServerFarmBlock::obj_ServerFarmBlock()
{
	ObjTypeFlags |= OBJTYPE_GameplayItem;
	ObjFlags |= OBJFLAG_SkipCastRay;

	m_ItemID = 0;
	m_ObstacleId = -1;
	m_TimeUntilRipe = FARM_RIPE_TIME;
	m_ActivateTrap = 0.0f;

	srvObjParams_.ExpireTime = r3dGetTime() + FARM_EXPIRE_TIME;
}

obj_ServerFarmBlock::~obj_ServerFarmBlock()
{
}

BOOL obj_ServerFarmBlock::OnCreate()
{
	//r3dOutToLog("obj_ServerFarmBlock[%d] created. ItemID:%d ripeTime:%.1f\n", srvObjParams_.ServerObjectID, m_ItemID, m_TimeUntilRipe);

	// set FileName based on itemid for ReadPhysicsConfig() in OnCreate() 
	r3dPoint3D bsize(1, 1, 1);
	if(m_ItemID == WeaponConfig::ITEMID_SolarWaterPurifier)
	{
		FileName = "Data\\ObjectsDepot\\INB_Gameplay\\inb_well_clean_01.sco";
		bsize    = r3dPoint3D(1.0900440f, 1.2519419f, 0.79267800f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_Garden_Box_Chicken)
	{
		FileName = "Data\\ObjectsDepot\\INB_Consumables\\inb_garden_chicken_coop_01.sco";
		bsize    = r3dPoint3D(0.527f, 1.171f, 1.247f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_GardenTrap_Rabbit)
	{
		FileName = "Data\\ObjectsDepot\\INB_Consumables\\inb_gardentrap_rabbit_01_closed.sco";
		bsize    = r3dPoint3D(0.527f, 1.171f, 1.247f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_Garden_Box_Lettuce)
	{
		FileName = "Data\\ObjectsDepot\\INB_Consumables\\inb_garden_box_dirt.sco";
		bsize    = r3dPoint3D(0.527f, 1.171f, 1.247f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_Garden_Box_Tomatos)
	{
		FileName = "Data\\ObjectsDepot\\INB_Consumables\\inb_garden_box_dirt.sco";
		bsize    = r3dPoint3D(0.527f, 1.171f, 1.247f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_Garden_Box_Beets)
	{
		FileName = "Data\\ObjectsDepot\\INB_Consumables\\inb_garden_box_dirt.sco";
		bsize    = r3dPoint3D(0.527f, 1.171f, 1.247f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_Garden_Box_Broccoli)
	{
		FileName = "Data\\ObjectsDepot\\INB_Consumables\\inb_garden_box_dirt.sco";
		bsize    = r3dPoint3D(0.527f, 1.171f, 1.247f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_Garden_Box_Carrots)
	{
		FileName = "Data\\ObjectsDepot\\INB_Consumables\\inb_garden_box_dirt.sco";
		bsize    = r3dPoint3D(0.527f, 1.171f, 1.247f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_Garden_Box_Potatos)
	{
		FileName = "Data\\ObjectsDepot\\INB_Consumables\\inb_garden_box_dirt.sco";
		bsize    = r3dPoint3D(0.527f, 1.171f, 1.247f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_Garden_Box)
	{
		FileName = "Data\\ObjectsDepot\\INB_Consumables\\inb_garden_box.sco";
		bsize    = r3dPoint3D(0.527f, 1.171f, 1.247f);
	}
	else
		r3dError("unknown farm item %d\n", m_ItemID);

	parent::OnCreate();

	// add navigational obstacle
	r3dBoundBox obb;
	obb.Size = bsize;
	obb.Org  = r3dPoint3D(GetPosition().x - obb.Size.x/2, GetPosition().y, GetPosition().z - obb.Size.z/2);
	m_ObstacleId = gAutodeskNavMesh.AddObstacle(this, obb, GetRotationVector().x);

	// calc 2d radius
	m_Radius = R3D_MAX(obb.Size.x, obb.Size.z) / 2;

	srvObjParams_.IsDirty = true;

	gServerLogic.NetRegisterObjectToPeers(this);

	return 1;
}

BOOL obj_ServerFarmBlock::OnDestroy()
{
	if(m_ObstacleId >= 0)
	{
		gAutodeskNavMesh.RemoveObstacle(m_ObstacleId);
	}

	PKT_S2C_DestroyNetObject_s n;
	n.spawnID = toP2pNetId(GetNetworkID());
	gServerLogic.p2pBroadcastToActive(this, &n, sizeof(n));

	return parent::OnDestroy();
}

BOOL obj_ServerFarmBlock::Update()
{
	const float curTime = r3dGetTime();

	if (m_ActivateTrap <=0.0 && m_ItemID == WeaponConfig::ITEMID_GardenTrap_Rabbit 
		&& m_ItemID == WeaponConfig::ITEMID_Garden_Box_Lettuce 
		&& m_ItemID == WeaponConfig::ITEMID_Garden_Box_Tomatos 
		&& m_ItemID == WeaponConfig::ITEMID_Garden_Box_Beets 
		&& m_ItemID == WeaponConfig::ITEMID_Garden_Box_Broccoli 
		&& m_ItemID == WeaponConfig::ITEMID_Garden_Box_Carrots 
		&& m_ItemID == WeaponConfig::ITEMID_Garden_Box_Potatos) //wait to close trap
		m_TimeUntilRipe = FARM_RIPE_TIME;

	if(m_TimeUntilRipe > 0)
	{
		if(m_ItemID == WeaponConfig::ITEMID_SolarWaterPurifier && m_ItemID == WeaponConfig::ITEMID_Garden_Box_Chicken) // solar water produces water only during day light
		{
			float inGameTime = gServerLogic.getInGameTime();
			if(inGameTime > 6.0f && inGameTime < 20.0f)
				m_TimeUntilRipe = R3D_MAX(m_TimeUntilRipe - r3dGetFrameTime(), 0.0f);
		}
		else
			m_TimeUntilRipe = R3D_MAX(m_TimeUntilRipe - r3dGetFrameTime(), 0.0f);
	}

	return parent::Update();
}

void obj_ServerFarmBlock::TryToHarvest(obj_ServerPlayer* plr)
{
		
	PKT_S2C_UseNetObjectAns_s n;
	if(m_TimeUntilRipe <= 0)
	{
		// harvest
		uint32_t harvestedItem = 0;
		if(m_ItemID == WeaponConfig::ITEMID_SolarWaterPurifier)
		{
			harvestedItem = 101485; // Cup of Water
		}
		if(m_ItemID == WeaponConfig::ITEMID_Garden_Box_Chicken)
		{
			harvestedItem = 100062; // meat
		}
		if(m_ItemID == WeaponConfig::ITEMID_GardenTrap_Rabbit)
		{
			harvestedItem = 100062; // Venison

			m_ActivateTrap=0.0f;
			PKT_S2C_SetupTraps_s n;
			n.spawnID = toP2pNetId(GetNetworkID());
			n.m_Activate = m_ActivateTrap>0.0?1:0;
			gServerLogic.p2pBroadcastToActive(this, &n, sizeof(n));	
		}
		if(m_ItemID == WeaponConfig::ITEMID_Garden_Box_Lettuce) //Lettuce
		{
			harvestedItem = 100070; // Lettuce item

			m_ActivateTrap=0.0f;
			PKT_S2C_SetupTraps_s n;
			n.spawnID = toP2pNetId(GetNetworkID());
			n.m_Activate = m_ActivateTrap>0.0?1:0;
			gServerLogic.p2pBroadcastToActive(this, &n, sizeof(n));	
		}
		if(m_ItemID == WeaponConfig::ITEMID_Garden_Box_Tomatos) //Tomatos
		{
			harvestedItem = 100075; // INB_Cons_Veg_Tomato_01 item

			m_ActivateTrap=0.0f;
			PKT_S2C_SetupTraps_s n;
			n.spawnID = toP2pNetId(GetNetworkID());
			n.m_Activate = m_ActivateTrap>0.0?1:0;
			gServerLogic.p2pBroadcastToActive(this, &n, sizeof(n));	
		}
		if(m_ItemID == WeaponConfig::ITEMID_Garden_Box_Beets) //Beet
		{
			harvestedItem = 100085; // INB_Cons_Veg_Beet_01

			m_ActivateTrap=0.0f;
			PKT_S2C_SetupTraps_s n;
			n.spawnID = toP2pNetId(GetNetworkID());
			n.m_Activate = m_ActivateTrap>0.0?1:0;
			gServerLogic.p2pBroadcastToActive(this, &n, sizeof(n));	
		}
		if(m_ItemID == WeaponConfig::ITEMID_Garden_Box_Broccoli) //Broccoli
		{
			harvestedItem = 100068; // INB_Cons_Veg_Broccoli_01

			m_ActivateTrap=0.0f;
			PKT_S2C_SetupTraps_s n;
			n.spawnID = toP2pNetId(GetNetworkID());
			n.m_Activate = m_ActivateTrap>0.0?1:0;
			gServerLogic.p2pBroadcastToActive(this, &n, sizeof(n));	
		}
		if(m_ItemID == WeaponConfig::ITEMID_Garden_Box_Carrots) //Carrots
		{
			harvestedItem = 100069; // INB_Cons_Veg_Carrot_01

			m_ActivateTrap=0.0f;
			PKT_S2C_SetupTraps_s n;
			n.spawnID = toP2pNetId(GetNetworkID());
			n.m_Activate = m_ActivateTrap>0.0?1:0;
			gServerLogic.p2pBroadcastToActive(this, &n, sizeof(n));	
		}
		if(m_ItemID == WeaponConfig::ITEMID_Garden_Box_Potatos) //Potatos
		{
			harvestedItem = 100073; // INB_Cons_Veg_Potato_01

			m_ActivateTrap=0.0f;
			PKT_S2C_SetupTraps_s n;
			n.spawnID = toP2pNetId(GetNetworkID());
			n.m_Activate = m_ActivateTrap>0.0?1:0;
			gServerLogic.p2pBroadcastToActive(this, &n, sizeof(n));	
		}
		if(m_ItemID == WeaponConfig::ITEMID_Garden_Box)
		{
			harvestedItem = (int)u_GetRandom(float(101363), float(101370));
		}

		wiInventoryItem wi;
		wi.itemID   = harvestedItem;
		wi.quantity = 3;
		if(plr->BackpackAddItem(wi))
		{
			m_TimeUntilRipe = FARM_RIPE_TIME;
			n.ans = PKT_S2C_UseNetObjectAns_s::Ans_HarvestedFarm;
		}
	}
	else
		n.ans = PKT_S2C_UseNetObjectAns_s::Ans_HarvestNotReady;

	gServerLogic.p2pSendToPeer(plr->peerId_, this, &n, sizeof(n));
}


DefaultPacket* obj_ServerFarmBlock::NetGetCreatePacket(int* out_size)
{
	static PKT_S2C_CreateNetObject_s n;
	n.spawnID = toP2pNetId(GetNetworkID());
	n.itemID  = m_ItemID;
	n.pos     = GetPosition();
	n.var1    = GetRotationVector().x;
	n.var2	  = m_TimeUntilRipe;
	n.var3    = m_ActivateTrap;
	//r3dOutToLog("##### 1 m_ActivateTrap %f\n",m_ActivateTrap);

	*out_size = sizeof(n);
	return &n;
}

void obj_ServerFarmBlock::LoadServerObjectData()
{
	m_ItemID = srvObjParams_.ItemID;

	// deserialize from xml
	IServerObject::CSrvObjXmlReader xml(srvObjParams_.Var1);
	m_TimeUntilRipe = xml.xmlObj.attribute("tuR").as_float();
	if(!xml.xmlObj.attribute("savedTime").empty() && m_TimeUntilRipe > 0)
	{
		uint32_t savedTime = xml.xmlObj.attribute("savedTime").as_uint();
		uint32_t curTime = _time32((__time32_t*)&curTime);
		m_TimeUntilRipe -= curTime-savedTime;
	}

	if(m_ItemID == WeaponConfig::ITEMID_GardenTrap_Rabbit 
		&& m_ItemID == WeaponConfig::ITEMID_Garden_Box_Lettuce
		&& m_ItemID == WeaponConfig::ITEMID_Garden_Box_Tomatos
		&& m_ItemID == WeaponConfig::ITEMID_Garden_Box_Beets
		&& m_ItemID == WeaponConfig::ITEMID_Garden_Box_Broccoli
		&& m_ItemID == WeaponConfig::ITEMID_Garden_Box_Carrots
		&& m_ItemID == WeaponConfig::ITEMID_Garden_Box_Potatos)
	{
		if (m_TimeUntilRipe>0)
			m_ActivateTrap=1.0f;
	}
}

void obj_ServerFarmBlock::SaveServerObjectData()
{
	srvObjParams_.ItemID = m_ItemID;

	IServerObject::CSrvObjXmlWriter xml;
	xml.xmlObj.append_attribute("tuR") = m_TimeUntilRipe;
	uint32_t secs1 = _time32((__time32_t*)&secs1);	// current UTC time
	xml.xmlObj.append_attribute("savedTime") = secs1;
	xml.save(srvObjParams_.Var1);
}


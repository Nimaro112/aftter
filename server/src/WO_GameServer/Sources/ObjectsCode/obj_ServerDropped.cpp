#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"

#include "multiplayer/P2PMessages.h"

#include "obj_ServerPlayer.h"
#include "obj_ServerDropped.h"
#include "ServerGameLogic.h"
#include "../EclipseStudio/Sources/ObjectsCode/weapons/WeaponArmory.h"
#include "../../GameEngine/ai/AutodeskNav/AutodeskNavMesh.h"
#include "Async_ServerObjects.h"

IMPLEMENT_CLASS(obj_ServerDropped, "obj_ServerDropped", "Object");
AUTOREGISTER_CLASS(obj_ServerDropped);

const static int DROPPED_NOT_USED_EXPIRE_TIME = 15 * 60; // Dropped will expire if not opened for 30min
const static int DROPPED_NOT_USED_EXPIRE_TIME_DM = 5 * 60; // Dropped will expire if not opened for 30min
const static int DROPPED_LOCKDOWN_TIME        = 5;

obj_ServerDropped::obj_ServerDropped()
{
	float expireTime = r3dGetTime() + DROPPED_NOT_USED_EXPIRE_TIME;
	float expireTime_DM = r3dGetTime() + DROPPED_NOT_USED_EXPIRE_TIME_DM;

	ObjTypeFlags |= OBJTYPE_GameplayItem;
	ObjFlags |= OBJFLAG_SkipCastRay;

	m_ItemID = 0;
	m_ObstacleId = -1;

	maxItems = 30; // for now hard coded
	nextInventoryID = 1;
	m_AccessCodeS[0] = 0;
	m_IsLocked   = 0;

	droppedOwnerId = 0;

	m_nextLockdownClear = r3dGetTime() + 1.0f;


	if (gServerLogic.ginfo_.mapId == GBGameInfo::MAPID_AM_Compound)
	{
		srvObjParams_.ExpireTime = expireTime_DM;
	}
	else 
		srvObjParams_.ExpireTime = expireTime;
}

obj_ServerDropped::~obj_ServerDropped()
{
}

BOOL obj_ServerDropped::OnCreate()
{
	r3dOutToLog("obj_ServerDropped[%d] created. ItemID:%d numItems:%d OwnerID:%d\n", srvObjParams_.ServerObjectID, m_ItemID, items.size(), srvObjParams_.CustomerID);

	// set FileName based on itemid for ReadPhysicsConfig() in OnCreate() 
	r3dPoint3D bsize(1, 1, 1);
	if(m_ItemID == WeaponConfig::ITEMID_Backpack1)
	{
		FileName = "Data\\ObjectsDepot\\SS_Loot\\SS_Loot_Corpse_Civ_01.sco";
		bsize    = r3dPoint3D(0, 0, 0);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_Backpack2)
	{
		FileName = "Data\\ObjectsDepot\\SS_Loot\\SS_Loot_Corpse_Civ_01.sco";
		bsize    = r3dPoint3D(0, 0, 0);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_Backpack3)
	{
		FileName = "Data\\ObjectsDepot\\SS_Loot\\SS_Loot_Corpse_Civ_01.sco";
		bsize    = r3dPoint3D(0, 0, 0);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_Backpack4)
	{
		FileName = "Data\\ObjectsDepot\\SS_Loot\\SS_Loot_Corpse_Mil_01.sco";
		bsize    = r3dPoint3D(0, 0, 0);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_Backpack5)
	{
		FileName = "Data\\ObjectsDepot\\SS_Loot\\SS_Loot_Corpse_Mil_01.sco";
		bsize    = r3dPoint3D(0, 0, 0);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_Backpack6)
	{
		FileName = "Data\\ObjectsDepot\\SS_Loot\\SS_Loot_Corpse_Mil_01.sco";
		bsize    = r3dPoint3D(0, 0, 0);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_Backpack7)
	{
		FileName = "Data\\ObjectsDepot\\SS_Loot\\SS_Loot_Corpse_Mil_01.sco";
		bsize    = r3dPoint3D(0, 0, 0);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_Backpack8)
	{
		FileName = "Data\\ObjectsDepot\\SS_Loot\\SS_Loot_Corpse_Specops_01.sco";
		bsize    = r3dPoint3D(0, 0, 0);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_Backpack9)
	{
		FileName = "Data\\ObjectsDepot\\SS_Loot\\SS_Loot_Corpse_Specops_01.sco";
		bsize    = r3dPoint3D(0, 0, 0);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_Backpack10)
	{
		FileName = "Data\\ObjectsDepot\\SS_Loot\\SS_Loot_Corpse_Mil_01.sco";
		bsize    = r3dPoint3D(0, 0, 0);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_Backpack11)
	{
		FileName = "Data\\ObjectsDepot\\SS_Loot\\SS_Loot_Corpse_Specops_01.sco";
		bsize    = r3dPoint3D(0, 0, 0);
	}
	//else if(m_ItemID == WeaponConfig::ITEMID_AirdropContainer)
	//{
	//	FileName = "Data\\ObjectsDepot\\SS_Gameplay\\SS_Terminal_GI.sco";
	//	//FileName = "Data\\ObjectsDepot\\SS_Loot\\SS_Loot_Corpse_Specops_01.sco";
	//	bsize    = r3dPoint3D(0, 0, 0);
	//}
	else
		r3dError("unknown container drop item %d\n", m_ItemID);

	parent::OnCreate();

	// add navigational obstacle
	r3dBoundBox obb;
	obb.Size = bsize;
	obb.Org  = r3dPoint3D(GetPosition().x - obb.Size.x/2, GetPosition().y, GetPosition().z - obb.Size.z/2);
	m_ObstacleId = gAutodeskNavMesh.AddObstacle(this, obb, GetRotationVector().x);

	// calc 2d radius
	m_Radius = R3D_MAX(obb.Size.x, obb.Size.z) / 2;

	gServerLogic.NetRegisterObjectToPeers(this);

	//droppedOwnerId = srvObjParams_.CustomerID;

	// convert old droppeds with unlimited expire time to current expiration time

	if (gServerLogic.ginfo_.mapId == GBGameInfo::MAPID_AM_Compound)
	{
		if((int)(srvObjParams_.ExpireTime - r3dGetTime()) > DROPPED_NOT_USED_EXPIRE_TIME_DM)
		{
			UpdateServerData();
		}
	}
	else if((int)(srvObjParams_.ExpireTime - r3dGetTime()) > DROPPED_NOT_USED_EXPIRE_TIME)
	{
		UpdateServerData();
	}

	return 1;
}

BOOL obj_ServerDropped::OnDestroy()
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

wiInventoryItem* obj_ServerDropped::FindItemWithInvID(__int64 invID)
{
	for(size_t i =0; i<items.size(); ++i)
	{
		if(items[i].InventoryID == invID)
			return &items[i];
	}
	return NULL;
}

bool obj_ServerDropped::AddItemToDropped(const wiInventoryItem& itm, int quantity)
{
	for(size_t i=0; i<items.size(); ++i)
	{
		if(items[i].CanStackWith(itm))
		{
			items[i].quantity += quantity;

			UpdateServerData();
			return true;
		}
	}
	
	if(items.size() < maxItems)
	{
		wiInventoryItem itm2 = itm;
		itm2.InventoryID = nextInventoryID++;
		itm2.quantity    = quantity;
		items.push_back(itm2);

		UpdateServerData();
		return true;
	}

	return false;
}

void obj_ServerDropped::RemoveItemFromDropped(__int64 invID, int amount)
{
	r3d_assert(amount >= 0);
	
	for(size_t i=0; i<items.size(); ++i)
	{
		if(items[i].InventoryID == invID)
		{
			r3d_assert(amount <= items[i].quantity);
			items[i].quantity -= amount;

			// remove from Dropped items array
			if(items[i].quantity <= 0)
			{
				items.erase(items.begin() + i);
			}

			UpdateServerData();	
			return;
		}
	}
	
	// invId must be validated first
	r3d_assert(false && "no invid in backpack");
	return;
}

BOOL obj_ServerDropped::Update()
{
	if (srvObjParams_.ExpireTime < r3dGetTime())
		DestroyDropped();
	
	if(isActive() && srvObjParams_.ExpireTime < r3dGetTime() && srvObjParams_.ServerObjectID > 0)
	{
		g_AsyncApiMgr->AddJob(new CJobDeleteServerObject(this));

		setActiveFlag(0);
		return TRUE;
	}
	const float curTime = r3dGetTime();
	
	// erase entries with expire lockdown. to avoid large memory usage if every player will try to unlock it :)
	if(curTime > m_nextLockdownClear)
	{
		m_nextLockdownClear = curTime + 60.0f;
		
		for(std::vector<lock_s>::iterator it = m_lockdowns.begin(); it != m_lockdowns.end(); )
		{
			if(curTime > it->lockEndTime) {
				it = m_lockdowns.erase(it);
			} else {
				++it;
			}
		}

		// keep uses for 5 min, lockEndTime used as lockbox opening time
		for(std::vector<lock_s>::iterator it = m_uses.begin(); it != m_uses.end(); )
		{
			if(curTime > it->lockEndTime + 5 * 60) {
				it = m_uses.erase(it);
			} else {
				++it;
			}
		}
	}

	return parent::Update();
}

void obj_ServerDropped::setAccessCode(const char* newCodeS)
{
	r3dscpy(m_AccessCodeS, newCodeS);
	m_IsLocked   = 1;

	UpdateServerData();	
}

void obj_ServerDropped::SendContentToPlayer(obj_ServerPlayer* plr)
{
	PKT_S2C_DroppedOpReq_s n2;
	n2.op = PKT_S2C_DroppedOpReq_s::LBOR_StartingToSendContent;
	n2.droppedID = toP2pNetId(GetNetworkID());
	gServerLogic.p2pSendToPeer(plr->peerId_, this, &n2, sizeof(n2));	

	PKT_S2C_DroppedContent_s n;
	for(uint32_t i=0; i<items.size(); ++i)
	{
		n.item = items[i];
		gServerLogic.p2pSendToPeer(plr->peerId_, this, &n, sizeof(n));
	}

	n2.op = PKT_S2C_DroppedOpReq_s::LBOR_DoneSendingContent;
	n2.droppedID = toP2pNetId(GetNetworkID());
	gServerLogic.p2pSendToPeer(plr->peerId_, this, &n2, sizeof(n2));	
}

bool obj_ServerDropped::IsLockdownActive(const obj_ServerPlayer* plr)
{
	const float curTime = r3dGetTime();
	
	for(size_t i=0; i<m_lockdowns.size(); i++)
	{
		lock_s& lock = m_lockdowns[i];
		if(lock.CustomerID == plr->profile_.CustomerID && curTime < lock.lockEndTime)
		{
			lock.tries++;
			// technically user can use issue only one attempt per 1.5 sec (item use time)
			// so we check if user issued them faster that 1sec
			if(lock.tries > DROPPED_LOCKDOWN_TIME)
			{
				gServerLogic.LogCheat(plr->peerId_, PKT_S2C_CheatWarning_s::CHEAT_Dropped, false, "Dropped",
					"tries %d", lock.tries);
			}
			return true;
		}
	}

	return false;
}

void obj_ServerDropped::SetLockdown(DWORD CustomerID)
{
	float lockEndTime = r3dGetTime() + DROPPED_LOCKDOWN_TIME;

	for(size_t i=0; i<m_lockdowns.size(); i++)
	{
		if(m_lockdowns[i].CustomerID == CustomerID)
		{
			m_lockdowns[i].lockEndTime = lockEndTime;
			m_lockdowns[i].tries       = 0;
			return;
		}
	}
	
	lock_s lock;
	lock.CustomerID  = CustomerID;
	lock.lockEndTime = lockEndTime;
	lock.tries       = 0;
	m_lockdowns.push_back(lock);
}

bool obj_ServerDropped::IsDroppedAbused(const obj_ServerPlayer* plr)
{
	const float curTime = r3dGetTime();
	
	for(size_t i=0; i<m_uses.size(); i++)
	{
		lock_s& lock = m_uses[i];
		if(lock.CustomerID == plr->profile_.CustomerID)
		{
			if(curTime > lock.lockEndTime + 60)
			{
				// used at least minute ago, reset timer (lockEndTime used as lockbox opening time)
				lock.lockEndTime = curTime - 0.001f;
				lock.tries       = 0;
			}

			// there was a 'possible' dupe method that allowed to fire alot of requests to update lockbox
			// hoping that they will be executed in wrong order on API side if was put to different job queues
			lock.tries++;
			float ups = (float)lock.tries / (curTime - lock.lockEndTime);
			if(lock.tries > 10 && ups > 20)
			{
				// on local machine using UI i was able to put about 5 requests per sec, so 20 usages per sec is exploit.
				gServerLogic.LogCheat(plr->peerId_, PKT_S2C_CheatWarning_s::CHEAT_Lockbox, true, "UsagePerSec",
					"tries %d, ups:%.1f", lock.tries, ups);
				return true;
			}

			return false;
		}
	}

	lock_s lock;
	lock.CustomerID  = plr->profile_.CustomerID;
	lock.lockEndTime = curTime;
	lock.tries       = 1;
	m_uses.push_back(lock);
	
	return false;
}


void obj_ServerDropped::DestroyDropped()
{
	setActiveFlag(0);
	g_AsyncApiMgr->AddJob(new CJobDeleteServerObject(this));
}

void obj_ServerDropped::UpdateServerData()
{
	// if lockbox was used, extend expire time for 2 weeks

	
	if (gServerLogic.ginfo_.mapId == GBGameInfo::MAPID_AM_Compound)
	{
		srvObjParams_.ExpireTime = srvObjParams_.CreateTime + DROPPED_NOT_USED_EXPIRE_TIME_DM;
	}
	else
		srvObjParams_.ExpireTime = srvObjParams_.CreateTime + DROPPED_NOT_USED_EXPIRE_TIME;

	g_AsyncApiMgr->AddJob(new CJobUpdateServerObject(this));
}

DefaultPacket* obj_ServerDropped::NetGetCreatePacket(int* out_size)
{
	static PKT_S2C_CreateNetObject_s n;
	n.spawnID = toP2pNetId(GetNetworkID());
	n.itemID  = m_ItemID;
	n.pos     = GetPosition();
	n.var1    = GetRotationVector().x;
	n.var4	= GetNetworkHelper()->srvObjParams_.CustomerID;

	*out_size = sizeof(n);
	return &n;
}

void obj_ServerDropped::LoadServerObjectData()
{
	m_ItemID = srvObjParams_.ItemID;

	// deserialize from xml
	IServerObject::CSrvObjXmlReader xml(srvObjParams_.Var1);
	r3dscpy(m_AccessCodeS, xml.xmlObj.attribute("accessCode").value());
	m_IsLocked   = xml.xmlObj.attribute("locked").as_int();
	uint32_t numItems = xml.xmlObj.attribute("numItems").as_uint();
	pugi::xml_node xmlItem = xml.xmlObj.child("item");
	for(uint32_t i=0; i<numItems; ++i)
	{
		if(xmlItem.empty()) // should never be empty
		{
			return; // bail out
		}
		wiInventoryItem it;
		it.InventoryID = nextInventoryID++;
		it.itemID = xmlItem.attribute("id").as_uint();
		
		// verify itemID is valid
		if(g_pWeaponArmory->getConfig(it.itemID)==NULL)
			return; // bail out

		it.quantity = xmlItem.attribute("q").as_uint();
		it.Var1 = xmlItem.attribute("v1").as_int();
		it.Var2 = xmlItem.attribute("v2").as_int();
		if(xmlItem.attribute("v3").value()[0])
			it.Var3 = xmlItem.attribute("v3").as_int();
		else
			it.Var3 = wiInventoryItem::MAX_DURABILITY;

		it.ResetClipIfFull(); // in case when full clip was saved before 2013-4-18
		items.push_back(it);

		xmlItem = xmlItem.next_sibling();
	}
}

void obj_ServerDropped::SaveServerObjectData()
{
	srvObjParams_.ItemID = m_ItemID;

	IServerObject::CSrvObjXmlWriter xml;
	xml.xmlObj.append_attribute("accessCode").set_value(m_AccessCodeS);
	xml.xmlObj.append_attribute("locked")     = m_IsLocked;
	xml.xmlObj.append_attribute("numItems") = items.size();
	for(size_t i=0; i<items.size(); ++i)
	{
		pugi::xml_node xmlItem = xml.xmlObj.append_child();
		xmlItem.set_name("item");
		xmlItem.append_attribute("id") = items[i].itemID;
		xmlItem.append_attribute("q") = items[i].quantity;
		xmlItem.append_attribute("v1") = items[i].Var1;
		xmlItem.append_attribute("v2") = items[i].Var2;
		xmlItem.append_attribute("v3") = items[i].Var3;
	}
	xml.save(srvObjParams_.Var1);
}


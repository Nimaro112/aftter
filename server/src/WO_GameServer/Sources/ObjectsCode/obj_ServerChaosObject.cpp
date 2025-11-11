//=========================================================================
//	Module: obj_ChaosObject.cpp
//	Copyright (C) Owl Production Group Inc. 2018.
//=========================================================================


#include "r3dPCH.h"
#include "r3d.h"
#include "GameCommon.h"

#include "obj_ServerChaosObject.h"
#include "obj_ServerPlayer.h"
#include "../../GameEngine/ai/AutodeskNav/AutodeskNavMesh.h"
#include "ObjectsCode/Zombies/sobj_Zombie.h"
#include "ServerGameLogic.h"

IMPLEMENT_CLASS(obj_ChaosObject, "obj_ChaosObject", "Object");
AUTOREGISTER_CLASS(obj_ChaosObject);

obj_ChaosObject::obj_ChaosObject()
{
	ChaosDurability();
	isChaosDestroyed = 0;
	RespawnChaosTimer = 0;
	m_ChaosEnable = 0; // 1 = available, 0 = disable
}

obj_ChaosObject::~obj_ChaosObject()
{
}

BOOL obj_ChaosObject::OnCreate()
{
	SetNetworkID(gServerLogic.GetFreeNetId());

	m_ChaosEnable = 1;

	SetPosition(pos);
	SetRotationVector(rot);

	gServerLogic.NetRegisterObjectToPeers(this);

	return parent::OnCreate();
}

BOOL obj_ChaosObject::OnDestroy()
{
	m_Health = 0;
	isChaosDestroyed = 1;

	PKT_S2C_ChaosTrack_s n;
	n.ChaosID			= toP2pNetId(GetNetworkID());
	n.m_ChaosEnable		= m_ChaosEnable;
	n.isChaosDestroyed	= isChaosDestroyed;
	n.SpawnParticles	= 1;

	gServerLogic.p2pBroadcastToActive(this, &n, sizeof(n));
	RespawnChaosTimer = r3dGetTime();

	return parent::OnDestroy();
}

BOOL obj_ChaosObject::Update()
{
	if (isChaosDestroyed == 1 && RespawnChaosTimer!=0)
	{
		if ((r3dGetTime() - RespawnChaosTimer) > (30*60)) // Respawn on 20 minutes
		{
			RespawnChaosTimer = 0;
			RespawnChaosObject();
		}
	}
	return parent::Update();
}

DefaultPacket* obj_ChaosObject::NetGetCreatePacket(int* out_size)
{
	static PKT_S2C_ChaosTrack_s n;
	n.ChaosID			= toP2pNetId(GetNetworkID());
	n.m_ChaosEnable		= m_ChaosEnable;
	n.isChaosDestroyed	= isChaosDestroyed;
	n.SpawnParticles	= 0;
	*out_size = sizeof(n);
	return &n;
}

void obj_ChaosObject::OnExplode()
{
	ObjectManager& GW = GameWorld();
	for( GameObject* obj = GameWorld().GetFirstObject(); obj; obj = GameWorld().GetNextObject(obj))
	{
		if(obj->isObjType(OBJTYPE_Human))
		{
			float minDist = 20;
			float maxDist = 60;
			float dist = (GetPosition() - obj->GetPosition()).LengthSq();
			if(dist < minDist ) // distance from explosion
			{
				obj_ServerPlayer* fromPlr = (obj_ServerPlayer*)obj;
				gServerLogic.ApplyDamage(fromPlr, obj, fromPlr->GetPosition()+r3dPoint3D(0,1,0), 40.0f, false, storecat_INVALID, 0);		
			}
			else if(dist < maxDist ) // distance from explosion
			{
				obj_ServerPlayer* fromPlr = (obj_ServerPlayer*)obj;
				gServerLogic.ApplyDamage(fromPlr, obj, fromPlr->GetPosition()+r3dPoint3D(0,1,0), 20.0f, false, storecat_INVALID, 0);					
			}
		}
	}
    for (GameObject *targetObj = GW.GetFirstObject(); targetObj; targetObj = GW.GetNextObject(targetObj))
    {

		obj_ServerPlayer* fromPlr = (obj_ServerPlayer*)targetObj;
        if(targetObj->isObjType(OBJTYPE_Zombie))
        {
			float minDist = 20;  // 20 meters from explosion
            obj_Zombie* zombie = (obj_Zombie*)targetObj;
			float dist = (GetPosition() - targetObj->GetPosition()).LengthSq();
            
            if(dist < minDist)
            {
                zombie->ApplyDamage(fromPlr, 100, 1, storecat_MELEE);
            }
        }
    }
	gServerLogic.InformZombiesAboutChaosExplosion(this);
}

#undef DEFINE_GAMEOBJ_PACKET_HANDLER
#define DEFINE_GAMEOBJ_PACKET_HANDLER(xxx) \
	case xxx: { \
		const xxx##_s&n = *(xxx##_s*)packetData; \
		if(packetSize != sizeof(n)) { \
			r3dOutToLog("!!!!errror!!!! %s packetSize %d != %d\n", #xxx, packetSize, sizeof(n)); \
			return TRUE; \
		} \
		OnNetPacket(n); \
		return TRUE; \
	}
BOOL obj_ChaosObject::OnNetReceive(DWORD EventID, const void* packetData, int packetSize)
{
	switch(EventID)
	{
		DEFINE_GAMEOBJ_PACKET_HANDLER(PKT_S2C_ChaosTrack);
	}
	return TRUE;
}
#undef DEFINE_GAMEOBJ_PACKET_HANDLER

void obj_ChaosObject::ReadSerializedData(pugi::xml_node& node)
{
	GameObject::ReadSerializedData(node);
	pugi::xml_node cNode = node.child("Chaos");
	SelectChaos = cNode.attribute("Number").as_int();

	pugi::xml_node cNodePos = node.child("position");
	pos.x = cNodePos.attribute("x").as_float();
	pos.y = cNodePos.attribute("y").as_float();
	pos.z = cNodePos.attribute("z").as_float();

	pugi::xml_node cNodeRot = node.child("gameObject");
	rot.x = cNodeRot.child("rotation").attribute("x").as_float();
	rot.y = 0.0f;
	rot.z = 0.0f;
}

BOOL obj_ChaosObject::DoDamage(float dmg)
{
	m_Health -= dmg;

	if (m_Health <= 0)
	{
		OnDestroy();
		OnExplode();

		isChaosDestroyed = 1;

		PKT_S2C_ChaosTrack_s n;
		n.ChaosID			= toP2pNetId(GetNetworkID());
		n.m_ChaosEnable		= m_ChaosEnable;
		n.isChaosDestroyed	= isChaosDestroyed;
		n.SpawnParticles	= 1;
		return TRUE;
	}
	return FALSE;
}

void obj_ChaosObject::RespawnChaosObject()
{
	ChaosDurability();

	isChaosDestroyed = 0;

	m_ChaosEnable = 1;

	PKT_S2C_ChaosTrack_s n;
	n.ChaosID			= toP2pNetId(GetNetworkID());
	n.m_ChaosEnable		= m_ChaosEnable;
	n.isChaosDestroyed	= isChaosDestroyed;
	n.SpawnParticles	= 0;

	gServerLogic.p2pBroadcastToActive(this, &n, sizeof(n));
}

void obj_ChaosObject::RelayPacket(const DefaultPacket* packetData, int packetSize, bool guaranteedAndOrdered)
{
	for(int i=0; i<gServerLogic.MAX_PEERS_COUNT; i++)
	{
		if(gServerLogic.peers_[i].status_ >= gServerLogic.PEER_VALIDATED1)
		{
			gServerLogic.RelayPacket(gServerLogic.peers_[i].CharID, this, packetData, packetSize, guaranteedAndOrdered);
		}
	}
}

void obj_ChaosObject::OnNetPacket(const PKT_S2C_ChaosTrack_s& n)
{
	m_ChaosEnable = n.m_ChaosEnable;

	PKT_S2C_ChaosTrack_s n2;
	n2.ChaosID			= n.ChaosID;
	n2.m_ChaosEnable	= m_ChaosEnable;
	n2.isChaosDestroyed	= isChaosDestroyed;
	n2.SpawnParticles	= 0;

	gServerLogic.p2pBroadcastToActive(this, &n2, sizeof(n2));

}

void obj_ChaosObject::LoadServerObjectData()
{
	// Do Nothing!
}

void obj_ChaosObject::SaveServerObjectData()
{
	// Do Nothing!
}

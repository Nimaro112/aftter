#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "XMLHelpers.h"
#include "ServerGameLogic.h"

#include "sobj_AirBombSpawn.h"

IMPLEMENT_CLASS(obj_AirBombSpawn, "obj_AirBombSpawn", "Object");
AUTOREGISTER_CLASS(obj_AirBombSpawn);

obj_AirBombSpawn::obj_AirBombSpawn()
	: spawnRadius(20)
{
	serializeFile = SF_ServerData;
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
}

obj_AirBombSpawn::~obj_AirBombSpawn()
{
}

BOOL obj_AirBombSpawn::OnCreate()
{

	ServerGameLogic::AirBombPositions AirBomb;
	AirBomb.m_radius	= spawnRadius;
	AirBomb.m_location	= GetPosition();
	AirBomb.m_DefaultItems = m_DefaultItems;
	AirBomb.m_LootBoxID1 = m_LootBoxID1;
	AirBomb.m_LootBoxID2 = m_LootBoxID2;
	AirBomb.m_LootBoxID3 = m_LootBoxID3;
	AirBomb.m_LootBoxID4 = m_LootBoxID4;
	AirBomb.m_LootBoxID5 = m_LootBoxID5;
	AirBomb.m_LootBoxID6 = m_LootBoxID6;
	AirBomb.m_LootBoxID7 = m_LootBoxID7;
	AirBomb.m_LootBoxID8 = m_LootBoxID8;
	AirBomb.m_LootBoxID9 = m_LootBoxID9;
	AirBomb.m_LootBoxID10 = m_LootBoxID10;
	AirBomb.m_LootBoxID11 = m_LootBoxID11;
	AirBomb.m_LootBoxID12 = m_LootBoxID12;
	AirBomb.m_LootBoxID13 = m_LootBoxID13;
	AirBomb.m_LootBoxID14 = m_LootBoxID14;
	AirBomb.m_LootBoxID15 = m_LootBoxID15;
	AirBomb.m_LootBoxID16 = m_LootBoxID16;
	AirBomb.m_LootBoxID17 = m_LootBoxID17;
	AirBomb.m_LootBoxID18 = m_LootBoxID18;
	AirBomb.m_LootBoxID19 = m_LootBoxID19;
	AirBomb.m_LootBoxID20 = m_LootBoxID20;
	gServerLogic.SetAirBomb( AirBomb );

	return parent::OnCreate();
}

BOOL obj_AirBombSpawn::OnDestroy()
{
	return parent::OnDestroy();
}

BOOL obj_AirBombSpawn::Update()
{
	return TRUE;
}

// copy from client version
void obj_AirBombSpawn::ReadSerializedData(pugi::xml_node& node)
{
	parent::ReadSerializedData(node);
	pugi::xml_node AirBombSpawnNode = node.child("LootID_parameters");
	GetXMLVal("spawn_radius", AirBombSpawnNode, &spawnRadius);
	GetXMLVal("m_DefaultItems", AirBombSpawnNode, &m_DefaultItems);
	GetXMLVal("m_LootBoxID1", AirBombSpawnNode, &m_LootBoxID1);
	GetXMLVal("m_LootBoxID2", AirBombSpawnNode, &m_LootBoxID2);
	GetXMLVal("m_LootBoxID3", AirBombSpawnNode, &m_LootBoxID3);
	GetXMLVal("m_LootBoxID4", AirBombSpawnNode, &m_LootBoxID4);
	GetXMLVal("m_LootBoxID5", AirBombSpawnNode, &m_LootBoxID5);
	GetXMLVal("m_LootBoxID6", AirBombSpawnNode, &m_LootBoxID6);
	GetXMLVal("m_LootBoxID7", AirBombSpawnNode, &m_LootBoxID7);
	GetXMLVal("m_LootBoxID8", AirBombSpawnNode, &m_LootBoxID8);
	GetXMLVal("m_LootBoxID9", AirBombSpawnNode, &m_LootBoxID9);
	GetXMLVal("m_LootBoxID10", AirBombSpawnNode, &m_LootBoxID10);
	GetXMLVal("m_LootBoxID11", AirBombSpawnNode, &m_LootBoxID11);
	GetXMLVal("m_LootBoxID12", AirBombSpawnNode, &m_LootBoxID12);
	GetXMLVal("m_LootBoxID13", AirBombSpawnNode, &m_LootBoxID13);
	GetXMLVal("m_LootBoxID14", AirBombSpawnNode, &m_LootBoxID14);
	GetXMLVal("m_LootBoxID15", AirBombSpawnNode, &m_LootBoxID15);
	GetXMLVal("m_LootBoxID16", AirBombSpawnNode, &m_LootBoxID16);
	GetXMLVal("m_LootBoxID17", AirBombSpawnNode, &m_LootBoxID17);
	GetXMLVal("m_LootBoxID18", AirBombSpawnNode, &m_LootBoxID18);
	GetXMLVal("m_LootBoxID19", AirBombSpawnNode, &m_LootBoxID19);
	GetXMLVal("m_LootBoxID20", AirBombSpawnNode, &m_LootBoxID20);
}

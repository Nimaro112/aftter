#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "XMLHelpers.h"
#include "ServerGameLogic.h"

#include "sobj_AirDropSpawn.h"

IMPLEMENT_CLASS(obj_AirDropSpawn, "obj_AirDropSpawn", "Object");
AUTOREGISTER_CLASS(obj_AirDropSpawn);

obj_AirDropSpawn::obj_AirDropSpawn()
	: spawnRadius(12)
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

obj_AirDropSpawn::~obj_AirDropSpawn()
{
}

BOOL obj_AirDropSpawn::OnCreate()
{

	ServerGameLogic::AirDropPositions AirDrop;
	AirDrop.m_radius	= spawnRadius;
	AirDrop.m_location	= GetPosition();
	AirDrop.m_DefaultItems = m_DefaultItems;
	AirDrop.m_LootBoxID1 = m_LootBoxID1;
	AirDrop.m_LootBoxID2 = m_LootBoxID2;
	AirDrop.m_LootBoxID3 = m_LootBoxID3;
	AirDrop.m_LootBoxID4 = m_LootBoxID4;
	AirDrop.m_LootBoxID5 = m_LootBoxID5;
	AirDrop.m_LootBoxID6 = m_LootBoxID6;
	AirDrop.m_LootBoxID7 = m_LootBoxID7;
	AirDrop.m_LootBoxID8 = m_LootBoxID8;
	AirDrop.m_LootBoxID9 = m_LootBoxID9;
	AirDrop.m_LootBoxID10 = m_LootBoxID10;
	AirDrop.m_LootBoxID11 = m_LootBoxID11;
	AirDrop.m_LootBoxID12 = m_LootBoxID12;
	AirDrop.m_LootBoxID13 = m_LootBoxID13;
	AirDrop.m_LootBoxID14 = m_LootBoxID14;
	AirDrop.m_LootBoxID15 = m_LootBoxID15;
	AirDrop.m_LootBoxID16 = m_LootBoxID16;
	AirDrop.m_LootBoxID17 = m_LootBoxID17;
	AirDrop.m_LootBoxID18 = m_LootBoxID18;
	AirDrop.m_LootBoxID19 = m_LootBoxID19;
	AirDrop.m_LootBoxID20 = m_LootBoxID20;
	gServerLogic.SetAirDrop( AirDrop );

	return parent::OnCreate();
}

BOOL obj_AirDropSpawn::OnDestroy()
{
	return parent::OnDestroy();
}

BOOL obj_AirDropSpawn::Update()
{
	return TRUE;
}

// copy from client version
void obj_AirDropSpawn::ReadSerializedData(pugi::xml_node& node)
{
	parent::ReadSerializedData(node);
	pugi::xml_node AirDropSpawnNode = node.child("LootID_parameters");
	GetXMLVal("spawn_radius", AirDropSpawnNode, &spawnRadius);
	GetXMLVal("m_DefaultItems", AirDropSpawnNode, &m_DefaultItems);
	GetXMLVal("m_LootBoxID1", AirDropSpawnNode, &m_LootBoxID1);
	GetXMLVal("m_LootBoxID2", AirDropSpawnNode, &m_LootBoxID2);
	GetXMLVal("m_LootBoxID3", AirDropSpawnNode, &m_LootBoxID3);
	GetXMLVal("m_LootBoxID4", AirDropSpawnNode, &m_LootBoxID4);
	GetXMLVal("m_LootBoxID5", AirDropSpawnNode, &m_LootBoxID5);
	GetXMLVal("m_LootBoxID6", AirDropSpawnNode, &m_LootBoxID6);
	GetXMLVal("m_LootBoxID7", AirDropSpawnNode, &m_LootBoxID7);
	GetXMLVal("m_LootBoxID8", AirDropSpawnNode, &m_LootBoxID8);
	GetXMLVal("m_LootBoxID9", AirDropSpawnNode, &m_LootBoxID9);
	GetXMLVal("m_LootBoxID10", AirDropSpawnNode, &m_LootBoxID10);
	GetXMLVal("m_LootBoxID11", AirDropSpawnNode, &m_LootBoxID11);
	GetXMLVal("m_LootBoxID12", AirDropSpawnNode, &m_LootBoxID12);
	GetXMLVal("m_LootBoxID13", AirDropSpawnNode, &m_LootBoxID13);
	GetXMLVal("m_LootBoxID14", AirDropSpawnNode, &m_LootBoxID14);
	GetXMLVal("m_LootBoxID15", AirDropSpawnNode, &m_LootBoxID15);
	GetXMLVal("m_LootBoxID16", AirDropSpawnNode, &m_LootBoxID16);
	GetXMLVal("m_LootBoxID17", AirDropSpawnNode, &m_LootBoxID17);
	GetXMLVal("m_LootBoxID18", AirDropSpawnNode, &m_LootBoxID18);
	GetXMLVal("m_LootBoxID19", AirDropSpawnNode, &m_LootBoxID19);
	GetXMLVal("m_LootBoxID20", AirDropSpawnNode, &m_LootBoxID20);
}

#pragma once

#include "GameCommon.h"

class obj_AirBombSpawn : public GameObject
{
	DECLARE_CLASS(obj_AirBombSpawn, GameObject)

public:
	float			spawnRadius;
	uint32_t		m_LootBoxID1;
	uint32_t		m_LootBoxID2;
	uint32_t		m_LootBoxID3;
	uint32_t		m_LootBoxID4;
	uint32_t		m_LootBoxID5;
	uint32_t		m_LootBoxID6;
	uint32_t		m_LootBoxID7;
	uint32_t		m_LootBoxID8;
	uint32_t		m_LootBoxID9;
	uint32_t		m_LootBoxID10;
	uint32_t		m_LootBoxID11;
	uint32_t		m_LootBoxID12;
	uint32_t		m_LootBoxID13;
	uint32_t		m_LootBoxID14;
	uint32_t		m_LootBoxID15;
	uint32_t		m_LootBoxID16;
	uint32_t		m_LootBoxID17;
	uint32_t		m_LootBoxID18;
	uint32_t		m_LootBoxID19;
	uint32_t		m_LootBoxID20;
	int				m_DefaultItems;

public:
	obj_AirBombSpawn();
	~obj_AirBombSpawn();

	virtual BOOL	OnCreate();
	virtual BOOL	OnDestroy();
	virtual BOOL	Update();
	virtual	void	ReadSerializedData(pugi::xml_node& node);

};

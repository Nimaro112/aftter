//=========================================================================
//	Module: obj_AirDropSpawn.h
//	Copyright (C) Online Warmongers Group Inc. 2012.
//=========================================================================

#pragma once

#include "GameCommon.h"
#include "gameobjects/GameObj.h"

//////////////////////////////////////////////////////////////////////////

class obj_AirDropSpawn: public GameObject
{
	DECLARE_CLASS(obj_AirDropSpawn, GameObject)

	/**	AirDrop spawn radius. */
	float spawnRadius;

	void CloneParameters(obj_AirDropSpawn *o);

public:
	obj_AirDropSpawn();
	~obj_AirDropSpawn();

	uint32_t				m_LootBoxID1;
	uint32_t				m_LootBoxID2;
	uint32_t				m_LootBoxID3;
	uint32_t				m_LootBoxID4;
	uint32_t				m_LootBoxID5;
	uint32_t				m_LootBoxID6;
	uint32_t				m_LootBoxID7;
	uint32_t				m_LootBoxID8;
	uint32_t				m_LootBoxID9;
	uint32_t				m_LootBoxID10;
	uint32_t				m_LootBoxID11;
	uint32_t				m_LootBoxID12;
	uint32_t				m_LootBoxID13;
	uint32_t				m_LootBoxID14;
	uint32_t				m_LootBoxID15;
	uint32_t				m_LootBoxID16;
	uint32_t				m_LootBoxID17;
	uint32_t				m_LootBoxID18;
	uint32_t				m_LootBoxID19;
	uint32_t				m_LootBoxID20;
	int			m_DefaultItems;

	virtual void AppendRenderables(RenderArray (& render_arrays  )[ rsCount ], const r3dCamera& Cam);
#ifndef FINAL_BUILD
	virtual float DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected);
#endif
	virtual BOOL OnCreate();
	virtual BOOL Update();
	virtual BOOL OnDestroy();
	virtual	void ReadSerializedData(pugi::xml_node& node);
	virtual void WriteSerializedData(pugi::xml_node& node);
	virtual	GameObject * Clone();
};


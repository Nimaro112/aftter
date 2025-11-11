//=========================================================================
//	Module: obj_AirBombSpawn.h
//	Copyright (C) Online Warmongers Group Inc. 2012.
//=========================================================================

#pragma once

#include "GameCommon.h"
#include "gameobjects/GameObj.h"

//////////////////////////////////////////////////////////////////////////

class obj_AirBombSpawn: public GameObject
{
	DECLARE_CLASS(obj_AirBombSpawn, GameObject)

	/**	AirBomb spawn radius. */
	float spawnRadius;

	void CloneParameters(obj_AirBombSpawn *o);

public:
	obj_AirBombSpawn();
	~obj_AirBombSpawn();

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


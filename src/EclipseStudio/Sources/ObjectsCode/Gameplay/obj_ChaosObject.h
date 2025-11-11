//=========================================================================
//	Module: obj_ChaosObject.h
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================

#pragma once

#include "SharedUsableItem.h"
#include "multiplayer/P2PMessages.h"

class obj_ChaosObject : public SharedUsableItem
{
	DECLARE_CLASS(obj_ChaosObject, SharedUsableItem)
public:

	bool		m_ChaosEnable;
	int			SelectChaosObj;
	BYTE		isChaosDestroyed;
	bool		RemoveChaosPhyx;
	bool		EnableChaosPhyx;

public:
	obj_ChaosObject();
	virtual ~obj_ChaosObject();

	virtual	BOOL		Load(const char *name);

	virtual	BOOL		OnCreate();
	virtual	BOOL		OnDestroy();

	virtual	BOOL		Update();
	void	Set(bool Open);
	void	SelectionObj(int Selection);
	void	ExeParticle();

	virtual void WriteSerializedData(pugi::xml_node& node);
	virtual void ReadSerializedData(pugi::xml_node& node);

	void CloneParameters(obj_ChaosObject *o);
	virtual	GameObject * Clone();

#ifndef FINAL_BUILD
	float DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected);
#endif
};

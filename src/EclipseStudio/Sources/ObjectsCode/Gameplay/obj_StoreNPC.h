#pragma once

#include "GameCommon.h"
#include "GameCode/UserProfile.h"
#include "SharedUsableItem.h"

class obj_StoreNPC : public SharedUsableItem
{
	DECLARE_CLASS(obj_StoreNPC, SharedUsableItem)
public:

public:
	obj_StoreNPC();
	virtual ~obj_StoreNPC();

	virtual	BOOL		Load(const char *name);

	virtual	BOOL		OnCreate();
	virtual	BOOL		OnDestroy();

	virtual	BOOL		Update();

	virtual void		WriteSerializedData(pugi::xml_node& node);
	virtual	void		ReadSerializedData(pugi::xml_node& node);
};

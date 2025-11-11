#pragma once

#include "GameCommon.h"
#include "GameCode/UserProfile.h"
#include "SharedUsableItem.h"

class obj_VaultNPC : public SharedUsableItem
{
	DECLARE_CLASS(obj_VaultNPC, SharedUsableItem)
public:

public:
	obj_VaultNPC();
	virtual ~obj_VaultNPC();

	virtual	BOOL		Load(const char *name);

	virtual	BOOL		OnCreate();
	virtual	BOOL		OnDestroy();

	virtual	BOOL		Update();

	virtual void		WriteSerializedData(pugi::xml_node& node);
	virtual	void		ReadSerializedData(pugi::xml_node& node);
};

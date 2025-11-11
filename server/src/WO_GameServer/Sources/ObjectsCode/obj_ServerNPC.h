#pragma once

#include "GameCommon.h"

class obj_ServerStoreNPC : public GameObject
{
	DECLARE_CLASS(obj_ServerStoreNPC, GameObject)

public:
	obj_ServerStoreNPC();
	virtual ~obj_ServerStoreNPC();

	virtual BOOL	OnCreate();
};


class obj_ServerVaultNPC : public GameObject
{
	DECLARE_CLASS(obj_ServerVaultNPC, GameObject)

public:
	obj_ServerVaultNPC();
	virtual ~obj_ServerVaultNPC();

	virtual BOOL	OnCreate();
};

class obj_ServerCraftNPC : public GameObject
{
	DECLARE_CLASS(obj_ServerCraftNPC, GameObject)

public:
	obj_ServerCraftNPC();
	virtual ~obj_ServerCraftNPC();

	virtual BOOL	OnCreate();
};

class obj_ServerArmyNPC : public GameObject
{
	DECLARE_CLASS(obj_ServerArmyNPC, GameObject)

public:
	obj_ServerArmyNPC();
	virtual ~obj_ServerArmyNPC();

	virtual BOOL	OnCreate();
};

class obj_ServerFoodNPC : public GameObject
{
	DECLARE_CLASS(obj_ServerFoodNPC, GameObject)

public:
	obj_ServerFoodNPC();
	virtual ~obj_ServerFoodNPC();

	virtual BOOL	OnCreate();
};

class obj_ServerCosmeticNPC : public GameObject
{
	DECLARE_CLASS(obj_ServerCosmeticNPC, GameObject)

public:
	obj_ServerCosmeticNPC();
	virtual ~obj_ServerCosmeticNPC();

	virtual BOOL	OnCreate();
};

class obj_ServerGearNPC : public GameObject
{
	DECLARE_CLASS(obj_ServerGearNPC, GameObject)

public:
	obj_ServerGearNPC();
	virtual ~obj_ServerGearNPC();

	virtual BOOL	OnCreate();
};

class obj_ServerSurvivalNPC : public GameObject
{
	DECLARE_CLASS(obj_ServerSurvivalNPC, GameObject)

public:
	obj_ServerSurvivalNPC();
	virtual ~obj_ServerSurvivalNPC();

	virtual BOOL	OnCreate();
};

class ServerNPCMgr
{
public:
	enum { MAX_NPC = 64 }; // 64 should be more than enough, if not, will redo into vector
	GameObject* NPCs_[MAX_NPC];
	int		numNPC_;

	void RegisterNPC(GameObject* npc) 
	{
		r3d_assert(numNPC_ < MAX_NPC);
		NPCs_[numNPC_++] = npc;
	}

	bool isCloseToNPC(const r3dPoint3D& pos, const char* className);

public:
	ServerNPCMgr() { numNPC_ = 0; }
	~ServerNPCMgr() {}
};

extern	ServerNPCMgr gServerNPCMngr;

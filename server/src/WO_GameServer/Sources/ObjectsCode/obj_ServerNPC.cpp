#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "XMLHelpers.h"

#include "multiplayer/P2PMessages.h"
#include "ServerGameLogic.h"

#include "obj_ServerNPC.h"

IMPLEMENT_CLASS(obj_ServerStoreNPC, "obj_StoreNPC", "Object");
AUTOREGISTER_CLASS(obj_ServerStoreNPC);

IMPLEMENT_CLASS(obj_ServerCraftNPC, "obj_CraftNPC", "Object");
AUTOREGISTER_CLASS(obj_ServerCraftNPC);

IMPLEMENT_CLASS(obj_ServerVaultNPC, "obj_VaultNPC", "Object");
AUTOREGISTER_CLASS(obj_ServerVaultNPC);

IMPLEMENT_CLASS(obj_ServerArmyNPC, "obj_ArmyNPC", "Object");
AUTOREGISTER_CLASS(obj_ServerArmyNPC);

IMPLEMENT_CLASS(obj_ServerFoodNPC, "obj_FoodNPC", "Object");
AUTOREGISTER_CLASS(obj_ServerFoodNPC);

IMPLEMENT_CLASS(obj_ServerCosmeticNPC, "obj_CosmeticNPC", "Object");
AUTOREGISTER_CLASS(obj_ServerCosmeticNPC);

IMPLEMENT_CLASS(obj_ServerGearNPC, "obj_GearNPC", "Object");
AUTOREGISTER_CLASS(obj_ServerGearNPC);

IMPLEMENT_CLASS(obj_ServerSurvivalNPC, "obj_SurvivalNPC", "Object");
AUTOREGISTER_CLASS(obj_ServerSurvivalNPC);

ServerNPCMgr gServerNPCMngr;

bool ServerNPCMgr::isCloseToNPC(const r3dPoint3D& pos, const char* className)
{
	float minDist = 9999999.0f;
	for(int i=0; i<numNPC_; ++i)
	{
		float d = (pos-NPCs_[i]->GetPosition()).Length();
		if(d < minDist && NPCs_[i]->Class->Name == className)
			minDist = d;
	}

	return minDist <= 6.0f; //NOW 6 METERS!
}

obj_ServerStoreNPC::obj_ServerStoreNPC() 
{
}

obj_ServerStoreNPC::~obj_ServerStoreNPC()
{
}

BOOL obj_ServerStoreNPC::OnCreate()
{
	parent::OnCreate();

	gServerNPCMngr.RegisterNPC(this);
	return 1;
}

obj_ServerCraftNPC::obj_ServerCraftNPC() 
{
}

obj_ServerCraftNPC::~obj_ServerCraftNPC()
{
}

BOOL obj_ServerCraftNPC::OnCreate()
{
	parent::OnCreate();

	gServerNPCMngr.RegisterNPC(this);
	return 1;
}

obj_ServerVaultNPC::obj_ServerVaultNPC() 
{
}

obj_ServerVaultNPC::~obj_ServerVaultNPC()
{
}

BOOL obj_ServerVaultNPC::OnCreate()
{
	parent::OnCreate();

	gServerNPCMngr.RegisterNPC(this);
	return 1;
}

obj_ServerArmyNPC::obj_ServerArmyNPC() 
{
}

obj_ServerArmyNPC::~obj_ServerArmyNPC()
{
}

BOOL obj_ServerArmyNPC::OnCreate()
{
	parent::OnCreate();

	gServerNPCMngr.RegisterNPC(this);
	return 1;
}

obj_ServerFoodNPC::obj_ServerFoodNPC() 
{
}

obj_ServerFoodNPC::~obj_ServerFoodNPC()
{
}

BOOL obj_ServerFoodNPC::OnCreate()
{
	parent::OnCreate();

	gServerNPCMngr.RegisterNPC(this);
	return 1;
}

obj_ServerCosmeticNPC::obj_ServerCosmeticNPC() 
{
}

obj_ServerCosmeticNPC::~obj_ServerCosmeticNPC()
{
}

BOOL obj_ServerCosmeticNPC::OnCreate()
{
	parent::OnCreate();

	gServerNPCMngr.RegisterNPC(this);
	return 1;
}

obj_ServerGearNPC::obj_ServerGearNPC() 
{
}

obj_ServerGearNPC::~obj_ServerGearNPC()
{
}

BOOL obj_ServerGearNPC::OnCreate()
{
	parent::OnCreate();

	gServerNPCMngr.RegisterNPC(this);
	return 1;
}

obj_ServerSurvivalNPC::obj_ServerSurvivalNPC() 
{
}

obj_ServerSurvivalNPC::~obj_ServerSurvivalNPC()
{
}

BOOL obj_ServerSurvivalNPC::OnCreate()
{
	parent::OnCreate();

	gServerNPCMngr.RegisterNPC(this);
	return 1;
}

#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"

#include "multiplayer/P2PMessages.h"

#include "obj_ServerBarricade.h"
#include "ServerGameLogic.h"
#include "ObjectsCode/obj_ServerPlayer.h"
#include "../EclipseStudio/Sources/ObjectsCode/weapons/WeaponArmory.h"
#include "../../GameEngine/ai/AutodeskNav/AutodeskNavMesh.h"
#include "Async_ServerObjects.h"
#include "ObjectsCode/obj_ServerCookingRack.h"
#include "ObjectsCode/sobj_DroppedItem.h"

IMPLEMENT_CLASS(obj_ServerBarricade, "obj_ServerBarricade", "Object");
AUTOREGISTER_CLASS(obj_ServerBarricade);
IMPLEMENT_CLASS(obj_StrongholdServerBarricade, "obj_StrongholdServerBarricade", "Object");
AUTOREGISTER_CLASS(obj_StrongholdServerBarricade);
IMPLEMENT_CLASS(obj_ConstructorServerBarricade, "obj_ConstructorServerBarricade", "Object");
AUTOREGISTER_CLASS(obj_ConstructorServerBarricade);

std::vector<obj_ServerBarricade*> obj_ServerBarricade::allBarricades;

const static int CONSTRUCTION_EXPIRE_TIME = 30 * 24 * 60 * 60; // barricade will expire in 14 days
const static int BARRICADE_EXPIRE_TIME = 3 * 24 * 60 * 60; // barricade will expire in 1 days
const static int STRONGHOLD_EXPIRE_TIME = 7 * 24 * 60 * 60; // stronghold items will expire in 30 days
const static int DEV_EVENT_EXPIRE_TIME = 30 * 60; // dev event items will expire in 30 minutes

obj_StrongholdServerBarricade::obj_StrongholdServerBarricade() :
obj_ServerBarricade()
{
	float expireTime = r3dGetTime() + STRONGHOLD_EXPIRE_TIME;

#ifdef DISABLE_GI_ACCESS_FOR_DEV_EVENT_SERVER
	if (gServerLogic.ginfo_.gameServerId == 148353
		// for testing in dev environment
		//|| gServerLogic.ginfo_.gameServerId==11
		)
		expireTime = r3dGetTime() + DEV_EVENT_EXPIRE_TIME;
#endif

	srvObjParams_.ExpireTime = expireTime; //r3dGetTime() + STRONGHOLD_EXPIRE_TIME;
}

obj_StrongholdServerBarricade::~obj_StrongholdServerBarricade()
{
}

obj_ConstructorServerBarricade::obj_ConstructorServerBarricade() :
obj_ServerBarricade()
{
	float expireTime = r3dGetTime() + CONSTRUCTION_EXPIRE_TIME;

	srvObjParams_.ExpireTime = expireTime;
}

obj_ConstructorServerBarricade::~obj_ConstructorServerBarricade()
{
}

obj_ServerBarricade::obj_ServerBarricade()
{
	allBarricades.push_back(this);

	ObjTypeFlags |= OBJTYPE_GameplayItem | OBJTYPE_Barricade;
	ObjFlags |= OBJFLAG_SkipCastRay;
	
	m_ItemID = 0;
	m_Health = 1;
	m_ObstacleId = -1;
	m_ActivateTrap = 0.0f;
	
	float expireTime = r3dGetTime() + BARRICADE_EXPIRE_TIME;

#ifdef DISABLE_GI_ACCESS_FOR_DEV_EVENT_SERVER
	if (gServerLogic.ginfo_.gameServerId == 148353
		// for testing in dev environment
		//|| gServerLogic.ginfo_.gameServerId==11
		)
		expireTime = r3dGetTime() + DEV_EVENT_EXPIRE_TIME;
#endif

	// CONSTRUCTION MATERIALS - ROTB
	if (m_ItemID == WeaponConfig::ITEMID_ConstructorBlockSmall ||
		m_ItemID == WeaponConfig::ITEMID_ConstructorBlockBig ||
		m_ItemID == WeaponConfig::ITEMID_ConstructorBlockCircle ||
		m_ItemID == WeaponConfig::ITEMID_ConstructorColum1 ||
		m_ItemID == WeaponConfig::ITEMID_ConstructorColum2 ||
		m_ItemID == WeaponConfig::ITEMID_ConstructorColum3 ||
		m_ItemID == WeaponConfig::ITEMID_ConstructorFloor1 ||
		m_ItemID == WeaponConfig::ITEMID_ConstructorFloor2 ||
		m_ItemID == WeaponConfig::ITEMID_ConstructorCeiling1 ||
		m_ItemID == WeaponConfig::ITEMID_ConstructorCeiling2 ||
		m_ItemID == WeaponConfig::ITEMID_ConstructorCeiling3 ||
		m_ItemID == WeaponConfig::ITEMID_ConstructorWallMetalic ||
		m_ItemID == WeaponConfig::ITEMID_ConstructorSlope ||
		m_ItemID == WeaponConfig::ITEMID_ConstructorWall1 ||
		m_ItemID == WeaponConfig::ITEMID_ConstructorWall2 ||
		m_ItemID == WeaponConfig::ITEMID_ConstructorWall3 ||
		m_ItemID == WeaponConfig::ITEMID_ConstructorWall4 ||
		m_ItemID == WeaponConfig::ITEMID_ConstructorBaseBunker ||
		m_ItemID == WeaponConfig::ITEMID_ConstructorWall5 ||
		// CONSTRUCTION MATERIALS - ROTB
		// CONSTRUCTION MATERIALS - INB
		m_ItemID == WeaponConfig::ITEMID_ConstructionINB000 ||
		m_ItemID == WeaponConfig::ITEMID_ConstructionINB001 ||
		m_ItemID == WeaponConfig::ITEMID_ConstructionINB002 ||
		m_ItemID == WeaponConfig::ITEMID_ConstructionINB003 ||
		m_ItemID == WeaponConfig::ITEMID_ConstructionINB004 ||
		m_ItemID == WeaponConfig::ITEMID_ConstructionINB005 ||
		m_ItemID == WeaponConfig::ITEMID_ConstructionINB006 ||
		m_ItemID == WeaponConfig::ITEMID_ConstructionINB007 ||
		m_ItemID == WeaponConfig::ITEMID_ConstructionINB008 ||
		m_ItemID == WeaponConfig::ITEMID_ConstructionINB009 ||
		m_ItemID == WeaponConfig::ITEMID_ConstructionINB010 ||
		m_ItemID == WeaponConfig::ITEMID_ConstructionINB011 ||
		m_ItemID == WeaponConfig::ITEMID_ConstructionINB012 ||
		m_ItemID == WeaponConfig::ITEMID_ConstructionINB013 ||
		m_ItemID == WeaponConfig::ITEMID_ConstructionINB014 ||
		m_ItemID == WeaponConfig::ITEMID_ConstructionINB015 ||
		m_ItemID == WeaponConfig::ITEMID_ConstructionINB016 ||
		m_ItemID == WeaponConfig::ITEMID_ConstructionINB017 ||
		m_ItemID == WeaponConfig::ITEMID_ConstructionINB018 ||
		m_ItemID == WeaponConfig::ITEMID_ConstructionINB019 ||
		m_ItemID == WeaponConfig::ITEMID_ConstructionINB020 ||
		m_ItemID == WeaponConfig::ITEMID_ConstructionINB021 ||
		m_ItemID == WeaponConfig::ITEMID_ConstructionINB022 ||
		m_ItemID == WeaponConfig::ITEMID_ConstructionINB023 ||
		m_ItemID == WeaponConfig::ITEMID_ConstructionINB024
		// CONSTRUCTION MATERIALS - INB
		)
	{
		expireTime = r3dGetTime() + CONSTRUCTION_EXPIRE_TIME;
	}

	srvObjParams_.ExpireTime = expireTime;
}

obj_ServerBarricade::~obj_ServerBarricade()
{
}
BOOL obj_ServerBarricade::OnCreate()
{
	r3dOutToLog("obj_ServerBarricade[%d] created. ItemID:%d Health:%.0f, %.0f mins left\n", srvObjParams_.ServerObjectID, m_ItemID, m_Health, (srvObjParams_.ExpireTime - r3dGetTime()) / 60.0f);
	
	// set FileName based on itemid for ReadPhysicsConfig() in OnCreate() 
	r3dPoint3D bsize(1, 1, 1);
	if(m_ItemID == WeaponConfig::ITEMID_PlaceableLight)
	{
		FileName = "Data\\ObjectsDepot\\INB_Town_Buildings\\prop_floodLight_02.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_SmallPowerGenerator)
	{
		FileName = "Data\\ObjectsDepot\\ROTB_Chaos_Objects\\Chaos_ELC_ElectricBox_01.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_BigPowerGenerator)
	{
		FileName = "Data\\ObjectsDepot\\ROTB_Chaos_Objects\\Chaos_Gas_Tank_01.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	// CONSTRUCTION MATERIALS - ROTB
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructorBlockSmall)
	{
		FileName = "Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_ab_01.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructorBlockBig)
	{
		FileName = "Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_ab_02.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructorBlockCircle)
	{
		FileName = "Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_ab_03.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructorColum1)
	{
		FileName = "Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_col_01.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructorColum2)
	{
		FileName = "Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_col_02.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructorColum3)
	{
		FileName = "Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_col_03.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructorBaseBunker)
	{
		FileName = "Data\\ObjectsDepot\\G3_BuildingBlocks\\base_lm_infantrybunker_01.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructorFloor1)
	{
		FileName = "Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_flr_01.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructorFloor2)
	{
		FileName = "Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_flr_02.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructorCeiling1)
	{
		FileName = "Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_fnd_01.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructorCeiling2)
	{
		FileName = "Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_fnd_02.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructorCeiling3)
	{
		FileName = "Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_fnd_03.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructorWallMetalic)
	{
		FileName = "Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_metalicpole_01.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructorSlope)
	{
		FileName = "Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_str_01.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructorWall1)
	{
		FileName = "Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_wal_01.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructorWall2)
	{
		FileName = "Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_wal_02.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructorWall3)
	{
		FileName = "Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_wal_03.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructorWall4)
	{
		FileName = "Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_01_01x04m.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructorWall5)
	{
		FileName = "Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_01_02x04m.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	// CONSTRUCTION MATERIALS - ROTB
	// CONSTRUCTION MATERIALS - INB
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB000)
	{
		FileName = "Data\\ObjectsDepot\\INB_SurvivalStructures\\inb_basebuilding_container_config_01.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB001)
	{
		FileName = "Data\\ObjectsDepot\\INB_SurvivalStructures\\inb_basebuilding_container_config_02.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB002)
	{
		FileName = "Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_basebuilding_Container_Config_03.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB003)
	{
		FileName = "Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_basebuilding_Container_Config_04.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB004)
	{
		FileName = "Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_basebuilding_Container_Stairs_01.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB005)
	{
		FileName = "Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_basebuilding_Wall_01.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB006)
	{
		FileName = "Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_basebuilding_Wall_02.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB007)
	{
		FileName = "Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_basebuilding_Wall_03.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB008)
	{
		FileName = "Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_BLD_Greenhouse_01.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB009)
	{
		FileName = "Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_bld_Shanty_House_1story_01.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB010)
	{
		FileName = "Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_Bld_WatchTower_Small.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB011)
	{
		FileName = "Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_Catwalks_noramp_01.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB012)
	{
		FileName = "Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_Catwalks_ramp_01.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB013)
	{
		FileName = "Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_Foundation_20x20.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB014)
	{
		FileName = "Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_Foundation_20x40.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB015)
	{
		FileName = "Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_Foundation_40x40.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB016)
	{
		FileName = "Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_SafeZone_Walls_01.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB017)
	{
		FileName = "Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_SafeZone_Walls_02.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB018)
	{
		FileName = "Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_SafeZone_Walls_03.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB019)
	{
		FileName = "Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_Shack_Survival_L_01.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB020)
	{
		FileName = "Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_Shack_Survival_S_01.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB021)
	{
		FileName = "Data\\ObjectsDepot\\INB_Gameplay\\INB_Statue_GOLD_01.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB022)
	{
		FileName = "Data\\ObjectsDepot\\INB_Gameplay\\INB_Statue_Marble_01.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB023)
	{
		FileName = "Data\\ObjectsDepot\\INB_Gameplay\\INB_Statue_Metal_01.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB024)
	{
		FileName = "Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_barricade_Tent_Shack_01.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	// CONSTRUCTION MATERIALS - INB

	// BARRICADES DECLARATIONS - INB
	else if(m_ItemID == WeaponConfig::ITEMID_BarBlockwallINB01)
	{
		FileName = "Data\\ObjectsDepot\\INB_Consumables\\INB_Barricade_Blockwall_01.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_BarBlockwallINB02)
	{
		FileName = "Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_Barricade_Blockwall_DX_01.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_BarConcreteINB01)
	{
		FileName = "Data\\ObjectsDepot\\INB_Consumables\\INB_Barricade_Concrete_01.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_BarConcreteINB02)
	{
		FileName = "Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_Barricade_Concrete_DX_01.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_BarConcreteINB03)
	{
		FileName = "Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_Barricade_Concrete_DX_02.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_BarConcreteINB04)
	{
		FileName = "Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_Barricade_Concrete_DX_03.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_BarConcreteINB05)
	{
		FileName = "Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_Barricade_Concrete_DX_04.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_BarPlywoodINB)
	{
		FileName = "Data\\ObjectsDepot\\INB_Consumables\\INB_Barricade_Plywood_01.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_BarSandbagINB)
	{
		FileName = "Data\\ObjectsDepot\\INB_Consumables\\INB_Barricade_Sandbag_01.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_BarScrapMetalINB)
	{
		FileName = "Data\\ObjectsDepot\\INB_Consumables\\INB_Barricade_ScrapMetal_01.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	// BARRICADES DECLARATIONS - INB

	// BARRICADES DECLARATIONS - SS
	else if(m_ItemID == WeaponConfig::ITEMID_SS_Arc_Shield)
	{
		FileName = "Data\\ObjectsDepot\\SS_Survival\\ss_barricade_shield_arc_deployed.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_SS_Bar_Concrete)
	{
		FileName = "Data\\ObjectsDepot\\SS_Survival\\SS_Barricade_Concrete_01_Deployed.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_SS_Bar_MetalWood)
	{
		FileName = "Data\\ObjectsDepot\\SS_Survival\\SS_Barricade_MetalWood_01_Deployed.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_SS_Bar_Shield_Meta)
	{
		FileName = "Data\\ObjectsDepot\\SS_Survival\\SS_Barricade_Shield_Metal_01_Deployed.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_SS_Bar_Shield_Wood)
	{
		FileName = "Data\\ObjectsDepot\\SS_Survival\\SS_Barricade_Shield_Wood_01_Deployed.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_SS_Bar_WoodWall)
	{
		FileName = "Data\\ObjectsDepot\\SS_Survival\\SS_Barricade_Woodwall_01_Deployed.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_SS_Bar_FenceWall)
	{
		FileName = "Data\\ObjectsDepot\\SS_Survival\\SS_Barricade_FenceWall_01_Deployed.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_SS_Spike_Trap)
	{
		FileName = "Data\\ObjectsDepot\\INB_Consumables\\SS_Survival_Spike_Trap_Deployed_Armed.sco";
		bsize    = r3dPoint3D(0.858f, 0.146f, 0.858f);
	}
	// BARRICADES DECLARATIONS - SS
	
	// BARRICADES DECLARATIONS - BURSTFIRE
	else if(m_ItemID == WeaponConfig::ITEMID_BarPortablecoverBF)
	{
		FileName = "Data\\ObjectsDepot\\TS_Tools\\TL_Portablecover_01.sco";
		bsize    = r3dPoint3D(1.513974f, 1.057301f, 1.111396f);
	}
	// BARRICADES DECLARATIONS - BURSTFIRE

	else if(m_ItemID == WeaponConfig::ITEMID_Traps_Bear)
	{
		FileName = "Data\\ObjectsDepot\\INB_Consumables\\INB_Traps_Bear_01_Armed_tps.sco";
		bsize    = r3dPoint3D(0.720f, 0.095f, 0.765f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_Traps_Spikes)
	{
		FileName = "Data\\ObjectsDepot\\INB_Consumables\\INB_Traps_Spikes_01_Armed_tps.sco";
		bsize    = r3dPoint3D(0.858f, 0.146f, 0.858f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_Campfire)
	{
		FileName = "Data\\ObjectsDepot\\INB_Consumables\\INB_prop_campfire_01.sco";
		bsize    = r3dPoint3D(0.929f, 0.580f, 0.929f);
	}
	///////////////TRAPS - INB
	else if(m_ItemID == WeaponConfig::ITEMID_BarbWireINB_01)
	{
		FileName = "Data\\ObjectsDepot\\INB_Consumables\\inb_barricade_barbwire_01.sco";
		bsize    = r3dPoint3D(0.720f, 0.095f, 0.765f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_BarbWireINB_02)
	{
		FileName = "Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_barricade_BarbWire_DX_01.sco";
		bsize    = r3dPoint3D(0.720f, 0.095f, 0.765f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_WoodSpikeINB_01)
	{
		FileName = "Data\\ObjectsDepot\\INB_Consumables\\INB_Barricade_WoodSpike_01.sco";
		bsize    = r3dPoint3D(0.720f, 0.095f, 0.765f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_WoodSpikeINB_02)
	{
		FileName = "Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_Barricade_WoodSpike_DX_01.sco";
		bsize    = r3dPoint3D(0.720f, 0.095f, 0.765f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_SpikeStripINB_01)
	{
		FileName = "Data\\ObjectsDepot\\INB_Consumables\\INB_Barricade_SpikeStrip_01.sco";
		bsize    = r3dPoint3D(0.720f, 0.095f, 0.765f);
	}
	///////////////TRAPS - INB

	///////////////TRAPS - BURSTFIRE
	else if(m_ItemID == WeaponConfig::ITEMID_BarbWireBF)
	{
		FileName = "Data\\ObjectsDepot\\TS_Tools\\tl_barbwire_extended.sco";
		bsize    = r3dPoint3D(0.720f, 0.095f, 0.765f);
	}
	///////////////TRAPS - BURSTFIRE
	
	///////////////TRAPS - SS
	else if(m_ItemID == WeaponConfig::ITEMID_SS_Traps_Spike_Mat)
	{
		FileName = "Data\\ObjectsDepot\\SS_Survival\\SS_Barricade_Spike_Mat_Deployed.sco";
		bsize    = r3dPoint3D(0.720f, 0.095f, 0.765f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_SS_Traps_BarbWire)
	{
		FileName = "Data\\ObjectsDepot\\SS_Survival\\SS_Barricade_BarbWire_Deployed.sco";
		bsize    = r3dPoint3D(0.720f, 0.095f, 0.765f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_SS_Spike_Trap)
	{
		FileName = "Data\\ObjectsDepot\\SS_Survival\\SS_Survival_Spike_Trap_Deployed_Armed.sco";
		bsize    = r3dPoint3D(0.720f, 0.095f, 0.765f);
	}
	else if(m_ItemID == WeaponConfig::ITEMID_SS_Bar_MetalW_Spike)
	{
		FileName = "Data\\ObjectsDepot\\SS_Survival\\SS_Barricade_MetalWood_01_Spike_Deployed.sco";
		bsize    = r3dPoint3D(0.720f, 0.095f, 0.765f);
	}
	///////////////TRAPS - SS

	///////////////NON TRAPS - INB
	else if(m_ItemID == WeaponConfig::ITEMID_BarrDoorINB_01)
	{
		FileName = "Data\\ObjectsDepot\\INB_SurvivalStructures\\inb_barr_doormtl_door_Close.sco";
		bsize    = r3dPoint3D(0.720f, 0.095f, 0.765f);
	}
	///////////////NON TRAPS - INB
	else
		r3dError("unknown barricade item %d\n", m_ItemID);

	parent::OnCreate();
	
	// add navigational obstacle
	r3dBoundBox obb;
	obb.Size = bsize;
	obb.Org  = r3dPoint3D(GetPosition().x - obb.Size.x/2, GetPosition().y, GetPosition().z - obb.Size.z/2);
	m_ObstacleId = gAutodeskNavMesh.AddObstacle(this, obb, GetRotationVector().x);
	
	// calc 2d radius
	m_Radius = R3D_MAX(obb.Size.x, obb.Size.z) / 2;

	gServerLogic.NetRegisterObjectToPeers(this);
	return 1;
}

BOOL obj_ServerBarricade::OnDestroy()
{
	if(m_ObstacleId >= 0)
	{
		gAutodeskNavMesh.RemoveObstacle(m_ObstacleId);
	}

	PKT_S2C_DestroyNetObject_s n;
	n.spawnID = toP2pNetId(GetNetworkID());
	gServerLogic.p2pBroadcastToActive(this, &n, sizeof(n));

	for(std::vector<obj_ServerBarricade*>::iterator it = allBarricades.begin(); it != allBarricades.end(); ++it)
	{
		if(*it == this)
		{
			allBarricades.erase(it);
			break;
		}
	}
	
	return parent::OnDestroy();
}

BOOL obj_ServerBarricade::Update()
{
	if (srvObjParams_.ExpireTime < r3dGetTime())
		DestroyBarricade();

	// check for pending delete
	if(isActive() && m_Health <= 0.0f && srvObjParams_.ServerObjectID > 0)
	{
		g_AsyncApiMgr->AddJob(new CJobDeleteServerObject(this));

		setActiveFlag(0);
		return TRUE;
	}
	if (m_ItemID == WeaponConfig::ITEMID_Traps_Bear ||
		m_ItemID == WeaponConfig::ITEMID_Traps_Spikes)
		CheckTouch();

	if (m_ItemID == WeaponConfig::ITEMID_SS_Spike_Trap)
		CheckTouchV2();

	if (m_ItemID == WeaponConfig::ITEMID_Campfire)
	{
		CampFireEffect();
		FireTouch();
	}

	if (m_ItemID == WeaponConfig::ITEMID_SpikeStripINB_01 
		|| m_ItemID == WeaponConfig::ITEMID_BarbWireBF 
		|| m_ItemID == WeaponConfig::ITEMID_SS_Traps_Spike_Mat
		|| m_ItemID == WeaponConfig::ITEMID_SS_Traps_BarbWire)
		AlwaysTouch();

	// Barricades Trap
	if (m_ItemID == WeaponConfig::ITEMID_BarbWireINB_01 ||
		m_ItemID == WeaponConfig::ITEMID_BarbWireINB_02 ||
		m_ItemID == WeaponConfig::ITEMID_WoodSpikeINB_01 ||
		m_ItemID == WeaponConfig::ITEMID_WoodSpikeINB_02 ||
		m_ItemID == WeaponConfig::ITEMID_SS_Bar_MetalW_Spike)
		BarricadeTouch();

	return parent::Update();
}

DefaultPacket* obj_ServerBarricade::NetGetCreatePacket(int* out_size)
{
	static PKT_S2C_CreateNetObject_s n;
	n.spawnID = toP2pNetId(GetNetworkID());
	n.itemID  = m_ItemID;
	n.pos     = GetPosition();
	n.var1    = GetRotationVector().x;
	n.var3    = m_ActivateTrap;

	*out_size = sizeof(n);
	return &n;
}

void obj_ServerBarricade::DoDamage(float dmg) // BARRICADE WITHOUT DAMAGE
{
		// CONSTRUCTION MATERIALS - ROTB
	//if (m_ItemID == WeaponConfig::ITEMID_ConstructorBlockSmall ||
	//	m_ItemID == WeaponConfig::ITEMID_ConstructorBlockBig ||
	//	m_ItemID == WeaponConfig::ITEMID_ConstructorBlockCircle ||
	//	m_ItemID == WeaponConfig::ITEMID_ConstructionINB024)
	//{
	//	m_Health -=0.01f;
	//	return;
	//}

	if(m_Health > 0 && m_ItemID != WeaponConfig::ITEMID_ConstructorWODBOX 
	//NEW AIRDROP CONTAINER START
	|| m_Health > 0 && m_ItemID != WeaponConfig::ITEMID_SurvivalContainer
	|| m_Health > 0 && m_ItemID != WeaponConfig::ITEMID_SurvivalContainerV2
	|| m_Health > 0 && m_ItemID != WeaponConfig::ITEMID_GunnerContainer
	|| m_Health > 0 && m_ItemID != WeaponConfig::ITEMID_KnifeContainer
	|| m_Health > 0 && m_ItemID != WeaponConfig::ITEMID_GearContainer
	|| m_Health > 0 && m_ItemID != WeaponConfig::ITEMID_PremiumContainer)
	//NEW AIRDROP CONTAINER START
	{
		srvObjParams_.IsDirty = true;
		m_Health -= dmg;
		// do not delete object here, it may still be waiting for assigned ServerObjectID
	}
}

void obj_ServerBarricade::LoadServerObjectData()
{
	m_ItemID = srvObjParams_.ItemID;
	
	// deserialize from xml
	IServerObject::CSrvObjXmlReader xml(srvObjParams_.Var1);
	m_Health = xml.xmlObj.attribute("Health").as_float();
}

void obj_ServerBarricade::CampFireEffect()
{
	for( GameObject* obj = GameWorld().GetFirstObject(); obj; obj = GameWorld().GetNextObject(obj) )
	{
		if(obj->isObjType(OBJTYPE_Human))
		{
			float dist = (GetPosition() - obj->GetPosition()).LengthSq();
			if(dist < 8)
			{
				obj_ServerPlayer* Player = (obj_ServerPlayer*)obj;
				//PKT_C2C_PlayerUseItem_s n;
				//wiInventoryItem& wi = Player->loadout_->Items[n.SlotFrom];
				//uint32_t usedItemId = wi.itemID;

				if (Player->loadout_->Alive > 0)
				{
					if (!Player->IsInVehicle())
					{
						if (Player->loadout_->Health<100)
							Player->loadout_->Health+=0.03f;
						if (Player->loadout_->MedBleeding!=0.0)
							Player->loadout_->MedBleeding = 0.0f;
						if (Player->loadout_->MedBloodInfection!=0.0)
							Player->loadout_->MedBloodInfection = 0.0f;
					}
				}
				/*if (Player->loadout_->Alive > 0 && Player->loadout_->GameFlags & wiCharDataFull::GAMEFLAG_NearCampfire)
				{
					//////////////// INB FOOD ////////////////
					if(usedItemId == 100105) // INB_Cons_Can_Beans_01
					{
						wiInventoryItem GenericSystem;
						GenericSystem.itemID = 100106; // INB_Cons_Can_Beans_01_Cooked
						GenericSystem.quantity = 1;
						if(Player->BackpackAddItem(GenericSystem) == false)
						{
							obj_DroppedItem* obj = (obj_DroppedItem*)srv_CreateGameObject("obj_DroppedItem", "obj_DroppedItem", Player->GetRandomPosForItemDrop());
							Player->SetupPlayerNetworkItem(obj);
							obj->m_Item = GenericSystem;
						}
					}
				}*/
			}
			// MISSING CREATE COOKING RADIUS SPOT
			// MISSING CREATE COOKING RADIUS SPOT
			// MISSING CREATE COOKING RADIUS SPOT
			// MISSING CREATE COOKING RADIUS SPOT
			// MISSING CREATE COOKING RADIUS SPOT
			/*obj_ServerPlayer* Player = (obj_ServerPlayer*)obj;
			PKT_C2C_PlayerUseItem_s n;
			wiInventoryItem& wi = Player->loadout_->Items[n.SlotFrom];
			uint32_t usedItemId = wi.itemID;
			
			for (int i = 0; i<gCookBoxesMngr.numcookBoxes_; i++)
			{
				obj_ServerCookingRack* cbox = gCookBoxesMngr.cookBoxes_[i];
				float distance = (GetPosition() - cbox->GetPosition()).Length();
				if (distance < cbox->useRadius)
				{
					Player->loadout_->GameFlags |= wiCharDataFull::GAMEFLAG_NearCookingRack;
					break;
				}
			}
			for (int i = 0; i<gCookBoxesMngr.numcookBoxes_; i++)
			{
				obj_ServerCookingRack* cbox = gCookBoxesMngr.cookBoxes_[i];
				float distance = (GetPosition() - cbox->GetPosition()).Length();
				if (distance < cbox->useRadius + 0)
				{
					Player->loadout_->GameFlags |= wiCharDataFull::GAMEFLAG_NearCookingRack;
					break;
				}
			}
			if (Player->loadout_->GameFlags & wiCharDataFull::GAMEFLAG_NearCookingRack)
			{
				if(usedItemId == 100105) // INB_Cons_Can_Beans_01
				{
					wiInventoryItem GenericSystem;
					GenericSystem.itemID = 100106; // INB_Cons_Can_Beans_01_Cooked
					GenericSystem.quantity = 1;
					if(Player->BackpackAddItem(GenericSystem) == false)
					{
						obj_DroppedItem* obj = (obj_DroppedItem*)srv_CreateGameObject("obj_DroppedItem", "obj_DroppedItem", Player->GetRandomPosForItemDrop());
						Player->SetupPlayerNetworkItem(obj);
						obj->m_Item = GenericSystem;
					}
				}
				
			}*/
		}
	}
}

void obj_ServerBarricade::CheckTouch()
{
	if (m_ActivateTrap>0.0)
	{
		for( GameObject* obj = GameWorld().GetFirstObject(); obj; obj = GameWorld().GetNextObject(obj) )
		{
			if(obj->isObjType(OBJTYPE_Human))
			{
				float dist = (GetPosition() - obj->GetPosition()).LengthSq();
				if(dist < 0.6)
				{
					obj_ServerPlayer* fromPlr = (obj_ServerPlayer*)obj;
					gServerLogic.ApplyDamage(fromPlr, obj, fromPlr->GetPosition()+r3dPoint3D(0,1,0), 12.0f, false, storecat_UsableItem, m_ItemID);
					m_ActivateTrap=0.0f;
					PKT_S2C_SetupTraps_s n;
					n.spawnID = toP2pNetId(GetNetworkID());
					n.m_Activate = m_ActivateTrap>0.0?1:0;
					gServerLogic.p2pBroadcastToActive(this, &n, sizeof(n));					
				}
			}
		}
	}
}

void obj_ServerBarricade::CheckTouchV2()
{
	if (m_ActivateTrap>0.0)
	{
		for( GameObject* obj = GameWorld().GetFirstObject(); obj; obj = GameWorld().GetNextObject(obj) )
		{
			if(obj->isObjType(OBJTYPE_Human))
			{
				float dist = (GetPosition() - obj->GetPosition()).LengthSq();
				if(dist < 0.7)
				{
					obj_ServerPlayer* fromPlr = (obj_ServerPlayer*)obj;
					gServerLogic.ApplyDamage(fromPlr, obj, fromPlr->GetPosition()+r3dPoint3D(0,1,0), 18.0f, false, storecat_UsableItem, m_ItemID);
					m_ActivateTrap=0.0f;
					PKT_S2C_SetupTraps_s n;
					n.spawnID = toP2pNetId(GetNetworkID());
					n.m_Activate = m_ActivateTrap>0.0?1:0;
					gServerLogic.p2pBroadcastToActive(this, &n, sizeof(n));					
				}
			}
		}
	}
}

void obj_ServerBarricade::AlwaysTouch()
{
	for( GameObject* obj = GameWorld().GetFirstObject(); obj; obj = GameWorld().GetNextObject(obj))
		{
			if(obj->isObjType(OBJTYPE_Human))
			{
				float dist = (GetPosition() - obj->GetPosition()).LengthSq();
				if(dist < 1.2)
				{
					obj_ServerPlayer* fromPlr = (obj_ServerPlayer*)obj;
					gServerLogic.ApplyDamage(fromPlr, obj, fromPlr->GetPosition()+r3dPoint3D(0,1,0), 0.20f, false, storecat_UsableItem, m_ItemID);
					PKT_S2C_SetupTraps_s n;
					n.spawnID = toP2pNetId(GetNetworkID());
					gServerLogic.p2pBroadcastToActive(this, &n, sizeof(n));					
				}
			}
		}
}

void obj_ServerBarricade::BarricadeTouch()
{
	for( GameObject* obj = GameWorld().GetFirstObject(); obj; obj = GameWorld().GetNextObject(obj))
	{
		if(obj->isObjType(OBJTYPE_Human))
		{
			float dist = (GetPosition() - obj->GetPosition()).LengthSq();
			if(dist < 2.5) // distance
			{
				obj_ServerPlayer* fromPlr = (obj_ServerPlayer*)obj;
				gServerLogic.ApplyDamage(fromPlr, obj, fromPlr->GetPosition()+r3dPoint3D(0,1,0), 0.30f, false, storecat_UsableItem, m_ItemID);
				PKT_S2C_SetupTraps_s n;
				n.spawnID = toP2pNetId(GetNetworkID());
				gServerLogic.p2pBroadcastToActive(this, &n, sizeof(n));					
			}
		}
	}
}

void obj_ServerBarricade::FireTouch()
{
	for( GameObject* obj = GameWorld().GetFirstObject(); obj; obj = GameWorld().GetNextObject(obj))
		{
			if(obj->isObjType(OBJTYPE_Human))
			{
				float dist = (GetPosition() - obj->GetPosition()).LengthSq();
				if(dist < 0.6)
				{
					obj_ServerPlayer* fromPlr = (obj_ServerPlayer*)obj;
					gServerLogic.ApplyDamage(fromPlr, obj, fromPlr->GetPosition()+r3dPoint3D(4,4,4), 0.15f, false, storecat_UsableItem, m_ItemID);
					PKT_S2C_SetupTraps_s n;
					n.spawnID = toP2pNetId(GetNetworkID());
					gServerLogic.p2pBroadcastToActive(this, &n, sizeof(n));					
				}
			}
		}
}

void obj_ServerBarricade::SaveServerObjectData()
{
	srvObjParams_.ItemID = m_ItemID;

	IServerObject::CSrvObjXmlWriter xml;
	xml.xmlObj.append_attribute("Health") = m_Health;
	xml.save(srvObjParams_.Var1);
}

#ifdef VEHICLES_ENABLED
int obj_ServerBarricade::GetDamageForVehicle()
{
	switch( m_ItemID )
	{
	default:
		return 0;

	case WeaponConfig::ITEMID_BarBlockwallINB01:
	case WeaponConfig::ITEMID_BarBlockwallINB02:
	case WeaponConfig::ITEMID_BarConcreteINB01:
	case WeaponConfig::ITEMID_BarConcreteINB02:
	case WeaponConfig::ITEMID_BarConcreteINB03:
	case WeaponConfig::ITEMID_BarConcreteINB04:
	case WeaponConfig::ITEMID_BarConcreteINB05:

	case WeaponConfig::ITEMID_SS_Arc_Shield:
	case WeaponConfig::ITEMID_SS_Bar_Concrete:
		return 1000;

	case WeaponConfig::ITEMID_SS_Bar_Shield_Meta:
	case WeaponConfig::ITEMID_SS_Bar_Shield_Wood:
	case WeaponConfig::ITEMID_SS_Bar_WoodWall:
	case WeaponConfig::ITEMID_SS_Bar_FenceWall:
	case WeaponConfig::ITEMID_SS_Bar_MetalWood:

	case WeaponConfig::ITEMID_BarPortablecoverBF:
		return 800;

	case WeaponConfig::ITEMID_BarPlywoodINB:
	case WeaponConfig::ITEMID_BarSandbagINB:
	case WeaponConfig::ITEMID_BarScrapMetalINB:
		return 500;
	}
}
#endif

void obj_ServerBarricade::DestroyBarricade()
{
	setActiveFlag(0);
	g_AsyncApiMgr->AddJob(new CJobDeleteServerObject(this));
}

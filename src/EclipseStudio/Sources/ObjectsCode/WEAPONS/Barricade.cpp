#include "r3dPCH.h"
#include "r3d.h"

#include "Barricade.h"
#include "WeaponConfig.h"
#include "WeaponArmory.h"
#include "..\ai\AI_Player.H"
#include "..\..\multiplayer\ClientGameLogic.h"
#include "../../EclipseStudio/Sources/ObjectsCode/world/MaterialTypes.h"
#ifdef VEHICLES_ENABLED
#include "../../GameEngine/gameobjects/obj_Vehicle.h"
#endif

extern bool gDestroyingWorld;

IMPLEMENT_CLASS(obj_Barricade, "obj_Barricade", "Object");
AUTOREGISTER_CLASS(obj_Barricade);

obj_Barricade::obj_Barricade()
	: isRenderable(true) 
	, defferredPhysicsDisable(false)
{
	m_PrivateModel = NULL;
	m_ItemID = -1;
	m_RotX = 0;
	ObjTypeFlags = OBJTYPE_GameplayItem | OBJTYPE_Barricade;
	m_GeneratorSound = 0;
	m_BBoxSet = 0;
}

obj_Barricade::~obj_Barricade()
{

}

BOOL obj_Barricade::OnCreate()
{
	bool trustServerPosition = false;
	r3d_assert(m_ItemID > 0);
	// CONSTRUCTION MATERIALS - ROTB
	if(m_ItemID == WeaponConfig::ITEMID_ConstructorBlockBig)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_ab_02.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructorBlockCircle)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_ab_03.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructorColum1)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_col_01.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructorColum2)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_col_02.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructorColum3)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_col_03.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructorFloor1)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_flr_01.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructorFloor2)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_flr_02.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other;
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructorCeiling1)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_fnd_01.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructorCeiling2)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_fnd_02.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructorCeiling3)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_fnd_03.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructorWallMetalic)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_metalicpole_01.sco", true, false, true, true );
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructorSlope)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_str_01.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructorWall1)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_wal_01.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructorWall2)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_wal_02.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructorWall3)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_wal_03.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructorWall4)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_01_01x04m.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructorWall5)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_01_02x04m.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructorBlockSmall)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_BuildingBlocks\\g3_buildingblock_ab_01.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructorBaseBunker)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_BuildingBlocks\\base_lm_infantrybunker_01.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	// CONSTRUCTION MATERIALS - ROTB
	// CONSTRUCTION MATERIALS - INB
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB000)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_SurvivalStructures\\inb_basebuilding_container_config_01.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB001)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_SurvivalStructures\\inb_basebuilding_container_config_02.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB002)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_basebuilding_Container_Config_03.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB003)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_basebuilding_Container_Config_04.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB004)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_basebuilding_Container_Stairs_01.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB005)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_basebuilding_Wall_01.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB006)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_basebuilding_Wall_02.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB007)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_basebuilding_Wall_03.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB008)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_BLD_Greenhouse_01.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB009)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_bld_Shanty_House_1story_01.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB010)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_Bld_WatchTower_Small.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB011)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_Catwalks_noramp_01.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB012)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_Catwalks_ramp_01.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB013)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_Foundation_20x20.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB014)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_Foundation_20x40.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB015)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_Foundation_40x40.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB016)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_SafeZone_Walls_01.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB017)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_SafeZone_Walls_02.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB018)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_SafeZone_Walls_03.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB019)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_Shack_Survival_L_01.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB020)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_Shack_Survival_S_01.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB021)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_Gameplay\\INB_Statue_GOLD_01.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB022)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_Gameplay\\INB_Statue_Marble_01.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB023)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_Gameplay\\INB_Statue_Metal_01.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	else if(m_ItemID == WeaponConfig::ITEMID_ConstructionINB024)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_barricade_Tent_Shack_01.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
		trustServerPosition = true; // as they can be stacked on top of each other
	}
	// CONSTRUCTION MATERIALS - INB

	// BARRICADES DECLARATIONS - INB
	else if(m_ItemID == WeaponConfig::ITEMID_BarBlockwallINB01)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_Consumables\\INB_Barricade_Blockwall_01.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
	}
	else if(m_ItemID == WeaponConfig::ITEMID_BarBlockwallINB02)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_Barricade_Blockwall_DX_01.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
	}
	else if(m_ItemID == WeaponConfig::ITEMID_BarConcreteINB01)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_Consumables\\INB_Barricade_Concrete_01.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
	}
	else if(m_ItemID == WeaponConfig::ITEMID_BarConcreteINB02)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_Barricade_Concrete_DX_01.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
	}
	else if(m_ItemID == WeaponConfig::ITEMID_BarConcreteINB03)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_Barricade_Concrete_DX_02.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
	}
	else if(m_ItemID == WeaponConfig::ITEMID_BarConcreteINB04)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_Barricade_Concrete_DX_03.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
	}
	else if(m_ItemID == WeaponConfig::ITEMID_BarConcreteINB05)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_Barricade_Concrete_DX_04.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
	}
	else if(m_ItemID == WeaponConfig::ITEMID_BarPlywoodINB)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_Consumables\\INB_Barricade_Plywood_01.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
	}
	else if(m_ItemID == WeaponConfig::ITEMID_BarSandbagINB)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_Consumables\\INB_Barricade_Sandbag_01.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
	}
	else if(m_ItemID == WeaponConfig::ITEMID_BarScrapMetalINB)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_Consumables\\INB_Barricade_ScrapMetal_01.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
	}
	// BARRICADES DECLARATIONS - INB
	// BARRICADES DECLARATIONS - SS
	else if(m_ItemID == WeaponConfig::ITEMID_SS_Arc_Shield)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\SS_Survival\\ss_barricade_shield_arc_deployed.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
	}
	else if(m_ItemID == WeaponConfig::ITEMID_SS_Bar_Concrete)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\SS_Survival\\SS_Barricade_Concrete_01_Deployed.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
	}
	else if(m_ItemID == WeaponConfig::ITEMID_SS_Bar_MetalWood)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\SS_Survival\\SS_Barricade_MetalWood_01_Deployed.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
	}
	else if(m_ItemID == WeaponConfig::ITEMID_SS_Bar_Shield_Meta)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\SS_Survival\\SS_Barricade_Shield_Metal_01_Deployed.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
	}
	else if(m_ItemID == WeaponConfig::ITEMID_SS_Bar_Shield_Wood)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\SS_Survival\\SS_Barricade_Shield_Wood_01_Deployed.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
	}
	else if(m_ItemID == WeaponConfig::ITEMID_SS_Bar_WoodWall)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\SS_Survival\\SS_Barricade_Woodwall_01_Deployed.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
	}
	else if(m_ItemID == WeaponConfig::ITEMID_SS_Bar_FenceWall)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\SS_Survival\\SS_Barricade_FenceWall_01_Deployed.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
	}
	// BARRICADES DECLARATIONS - SS

	// BARRICADES DECLARATIONS - BURSTFIRE
	else if(m_ItemID == WeaponConfig::ITEMID_BarPortablecoverBF)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\TS_Tools\\TL_Portablecover_01.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
	}
	// BARRICADES DECLARATIONS - BURSTFIRE

	else if(m_ItemID == WeaponConfig::ITEMID_PlaceableLight)
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_Town_Buildings\\inb_const_prop_floodlight_02.sco", true, false, true, true );
	else if(m_ItemID == WeaponConfig::ITEMID_SmallPowerGenerator)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\ROTB_Chaos_Objects\\Chaos_ELC_ElectricBox_01.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
	}
	else if(m_ItemID == WeaponConfig::ITEMID_BigPowerGenerator)
	{
		m_PrivateModel = r3dGOBAddMesh("Data\\ObjectsDepot\\ROTB_Chaos_Objects\\Chaos_Gas_Tank_01.sco", true, false, true, true );
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "buildingobj_fx", GetPosition());
	}

	if(m_PrivateModel==NULL)
		return FALSE;

	ReadPhysicsConfig();

	//PhysicsConfig.group = PHYSCOLL_NETWORKPLAYER;
	//PhysicsConfig.isDynamic = 0;

	// raycast and see where the ground is and place dropped box there
	if(!trustServerPosition)
	{
		PxRaycastHit hit;
		PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK, 0, 0, 0), PxSceneQueryFilterFlag::eSTATIC);
		if(g_pPhysicsWorld->raycastSingle(PxVec3(GetPosition().x, GetPosition().y+1, GetPosition().z), PxVec3(0, -1, 0), 50.0f, PxSceneQueryFlag::eIMPACT, hit, filter))
		{
			SetPosition(r3dPoint3D(hit.impact.x, hit.impact.y, hit.impact.z));
		}
	}
	SetRotationVector(r3dPoint3D(m_RotX, 0, 0));

	UpdateTransform();

	SoundSys.PlayAndForget(SoundSys.GetEventIDByPath("Sounds/AM_Group/AIPlayer/CraftNPC/User_Action_Craft_Item_01"), GetPosition());

	/*if(m_ItemID == WeaponConfig::ITEMID_PlaceableLight) // Disabled
	{
		const r3dPoint3D lightOffset(0.41f, 2.0f, -0.16f);

		m_Light.Assign(GetPosition()+lightOffset);
		m_Light.SetType(R3D_OMNI_LIGHT);
		m_Light.SetRadius(10.0f, 25.0f);
		m_Light.SetColor(255, 255, 182);
		m_Light.bCastShadows = false;//r_lighting_quality->GetInt() == 3 && r_shadows_quality->GetInt() >= 3;
		m_Light.Intensity = 2.0f;
		m_Light.SpotAngleFalloffPow = 1.6f;
		m_Light.bSSShadowBlur = 1;
		m_Light.bUseGlobalSSSBParams = 0;
		m_Light.SSSBParams.Bias = 0.1f;
		m_Light.SSSBParams.PhysRange = 43.12f;
		m_Light.SSSBParams.Radius = 10.0f;
		m_Light.SSSBParams.Sense = 635.18f;

		m_Light.TurnOff();
		WorldLightSystem.Add(&m_Light); 
		m_Light.TurnOn();
	}
	else */
	if(m_ItemID == WeaponConfig::ITEMID_SmallPowerGenerator || m_ItemID == WeaponConfig::ITEMID_BigPowerGenerator)
	{
		m_GeneratorSound = SoundSys.Play(SoundSys.GetEventIDByPath("Sounds/AM_Group/Item_Action/ENV_Generator"), GetPosition());
	}

	return parent::OnCreate();
}

BOOL obj_Barricade::OnDestroy()
{
	Destroy(true);

	m_PrivateModel = NULL;
	if(m_Light.pLightSystem)
		WorldLightSystem.Remove(&m_Light);
	if(m_GeneratorSound)
	{
		SoundSys.Stop(m_GeneratorSound);
		SoundSys.Release(m_GeneratorSound);
		m_GeneratorSound = NULL;
	}

	return parent::OnDestroy();
}

void obj_Barricade::OnContactModify(PhysicsCallbackObject *obj, PxContactSet& contacts)
{
	if (!obj)
		return;

	GameObject* gameObj = obj->isGameObject();
	if (!gameObj)
		return;

#ifdef VEHICLES_ENABLED
	if (gameObj->isObjType(OBJTYPE_Vehicle) && ((obj_Vehicle*)gameObj)->CanSmashBarricade())
	{
		obj_Vehicle* vehicle = (obj_Vehicle*)gameObj;
		vehicle->DoSlowDown();

		for(uint32_t i = 0; i < contacts.size(); ++i)
		{
			contacts.ignore(i);

			Destroy(false);
		}
	}
#endif
}

BOOL obj_Barricade::Update()
{
	// Can't destroy barricades while Physics simulation is running,
	// so we do it in a deffered manner.  (e.g. car runs into barricade)
	if( defferredPhysicsDisable )
	{
		m_bEnablePhysics = true;
		UpdatePhysicsEnabled(false);
		defferredPhysicsDisable = false;
	}
	
	bool FoundPowerGenerator=false;
	for( GameObject* obj = GameWorld().GetFirstObject(); obj; obj = GameWorld().GetNextObject(obj) )
	{
		if (obj->isObjType(OBJTYPE_Barricade))
		{
			obj_Barricade* Barricade = (obj_Barricade*)obj;
			if(Barricade->m_ItemID == WeaponConfig::ITEMID_SmallPowerGenerator || Barricade->m_ItemID == WeaponConfig::ITEMID_BigPowerGenerator)
			{
				float dist = (GetPosition() - obj->GetPosition()).Length();
				if (dist<100)
				{
					FoundPowerGenerator=true;
					break;
				}
			}
		}
	}
	if (FoundPowerGenerator)
		LightTurnOn();
	else
		LightTurnOff();

	if( !m_BBoxSet )
	{
		if( r3dMesh* m = GetObjectMesh() )
		{
			if( m->IsLoaded() )
			{
				m_BBoxSet = 1;
				SetBBoxLocal( GetObjectMesh()->localBBox ) ;
			}
		}
	}

	return parent::Update();
}

void obj_Barricade::LightTurnOn()
{
	if(m_ItemID == WeaponConfig::ITEMID_PlaceableLight)
	{
		const r3dPoint3D lightOffset(0.41f, 2.0f, -0.16f);

		m_Light.Assign(GetPosition()+lightOffset);
		m_Light.SetType(R3D_OMNI_LIGHT);
		m_Light.SetRadius(10.0f, 25.0f);
		m_Light.SetColor(255, 255, 182);
		m_Light.bCastShadows = false;//r_lighting_quality->GetInt() == 3 && r_shadows_quality->GetInt() >= 3;
		m_Light.Intensity = 2.0f;
		m_Light.SpotAngleFalloffPow = 1.6f;
		m_Light.bSSShadowBlur = 1;
		m_Light.bUseGlobalSSSBParams = 0;
		m_Light.SSSBParams.Bias = 0.1f;
		m_Light.SSSBParams.PhysRange = 43.12f;
		m_Light.SSSBParams.Radius = 10.0f;
		m_Light.SSSBParams.Sense = 635.18f;

		m_Light.TurnOff();
		if(!m_Light.pLightSystem)
			WorldLightSystem.Add(&m_Light); 
		m_Light.TurnOn();
	}
}
void obj_Barricade::LightTurnOff()
{
	if(m_ItemID == WeaponConfig::ITEMID_PlaceableLight)
	{
		m_Light.TurnOff();
		WorldLightSystem.Remove(&m_Light);
	}
}

void obj_Barricade::Destroy(bool playParticles)
{
	if( PhysicsObject )
	{
		PxActor* actor = PhysicsObject->getPhysicsActor();
		if( actor )
		{
			m_bEnablePhysics = false;
			defferredPhysicsDisable = true;
		}
	}
	if( playParticles && !gDestroyingWorld )
	{
		switch( m_ItemID )
		{
		default:
			break;
		case WeaponConfig::ITEMID_BarBlockwallINB01:
			SpawnImpactParticle(r3dHash::MakeHash(""), r3dHash::MakeHash("INB_Concrete_Bar"), GetPosition(), r3dPoint3D(0,1,0));
			break;
		case WeaponConfig::ITEMID_BarBlockwallINB02:
			SpawnImpactParticle(r3dHash::MakeHash(""), r3dHash::MakeHash("INB_Concrete_Bar"), GetPosition(), r3dPoint3D(0,1,0));
			break;
		case WeaponConfig::ITEMID_BarConcreteINB01:
			SpawnImpactParticle(r3dHash::MakeHash(""), r3dHash::MakeHash("INB_Concrete_Bar"), GetPosition(), r3dPoint3D(0,1,0));
			break;
		case WeaponConfig::ITEMID_BarConcreteINB02:
			SpawnImpactParticle(r3dHash::MakeHash(""), r3dHash::MakeHash("INB_Concrete_Bar"), GetPosition(), r3dPoint3D(0,1,0));
			break;
		case WeaponConfig::ITEMID_BarConcreteINB03:
			SpawnImpactParticle(r3dHash::MakeHash(""), r3dHash::MakeHash("INB_Concrete_Bar"), GetPosition(), r3dPoint3D(0,1,0));
			break;
		case WeaponConfig::ITEMID_BarConcreteINB04:
			SpawnImpactParticle(r3dHash::MakeHash(""), r3dHash::MakeHash("INB_Concrete_Bar"), GetPosition(), r3dPoint3D(0,1,0));
			break;
		case WeaponConfig::ITEMID_BarConcreteINB05:
			SpawnImpactParticle(r3dHash::MakeHash(""), r3dHash::MakeHash("INB_Concrete_Bar"), GetPosition(), r3dPoint3D(0,1,0));
			break;
		case WeaponConfig::ITEMID_BarPlywoodINB:
			SpawnImpactParticle(r3dHash::MakeHash(""), r3dHash::MakeHash("INB_Wood_Bar"), GetPosition(), r3dPoint3D(0,1,0));
			break;
		case WeaponConfig::ITEMID_BarSandbagINB:
			SpawnImpactParticle(r3dHash::MakeHash(""), r3dHash::MakeHash("INB_SandBg_Bar"), GetPosition(), r3dPoint3D(0,1,0));
			break;
		case WeaponConfig::ITEMID_BarScrapMetalINB:
			SpawnImpactParticle(r3dHash::MakeHash(""), r3dHash::MakeHash("INB_ScrapM_Bar"), GetPosition(), r3dPoint3D(0,1,0));
			break;
		// BURSTFIRE BARRICADE
		case WeaponConfig::ITEMID_BarPortablecoverBF:
			SpawnImpactParticle(r3dHash::MakeHash(""), r3dHash::MakeHash("BF_Cover_Bar"), GetPosition(), r3dPoint3D(0,1,0));
			break;
		/// SS BARRICADE
		case WeaponConfig::ITEMID_SS_Arc_Shield:
			SpawnImpactParticle(r3dHash::MakeHash(""), r3dHash::MakeHash("SS_Bar_Concrete"), GetPosition(), r3dPoint3D(0,1,0));
			break;
		case WeaponConfig::ITEMID_SS_Bar_Concrete:
			SpawnImpactParticle(r3dHash::MakeHash(""), r3dHash::MakeHash("SS_Bar_Concrete"), GetPosition(), r3dPoint3D(0,1,0));
			break;
		case WeaponConfig::ITEMID_SS_Bar_MetalWood:
			SpawnImpactParticle(r3dHash::MakeHash(""), r3dHash::MakeHash("SS_Bar_MetalWood"), GetPosition(), r3dPoint3D(0,1,0));
			break;
		case WeaponConfig::ITEMID_SS_Bar_Shield_Meta:
			SpawnImpactParticle(r3dHash::MakeHash(""), r3dHash::MakeHash("SS_Bar_Shield_Meta"), GetPosition(), r3dPoint3D(0,1,0));
			break;
		case WeaponConfig::ITEMID_SS_Bar_Shield_Wood:
			SpawnImpactParticle(r3dHash::MakeHash(""), r3dHash::MakeHash("SS_Bar_Shield_Wood"), GetPosition(), r3dPoint3D(0,1,0));
			break;
		case WeaponConfig::ITEMID_SS_Bar_WoodWall:
			SpawnImpactParticle(r3dHash::MakeHash(""), r3dHash::MakeHash("SS_Bar_WoodWall"), GetPosition(), r3dPoint3D(0,1,0));
			break;
		case WeaponConfig::ITEMID_SS_Bar_FenceWall:
			SpawnImpactParticle(r3dHash::MakeHash(""), r3dHash::MakeHash("SS_Bar_FenceWall"), GetPosition(), r3dPoint3D(0,1,0));
			break;
		/// POWER GENERATOR
		case WeaponConfig::ITEMID_SmallPowerGenerator:
			SpawnImpactParticle(r3dHash::MakeHash(""), r3dHash::MakeHash("RegularMine"), GetPosition(), r3dPoint3D(0,1,0));
			break;
		case WeaponConfig::ITEMID_BigPowerGenerator:
			SpawnImpactParticle(r3dHash::MakeHash(""), r3dHash::MakeHash("RegularMine"), GetPosition(), r3dPoint3D(0,1,0));
			break;
		}
	}

	isRenderable = false;
}

void obj_Barricade::DrawDebugInfo()
{
#ifndef FINAL_BUILD
	r3dRenderer->SetMaterial(NULL);

	struct PushRestoreStates
	{
		PushRestoreStates()
		{
			r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_NZ | R3D_BLEND_PUSH );
		}

		~PushRestoreStates()
		{
			r3dRenderer->SetRenderingMode( R3D_BLEND_POP );
		}
	} pushRestoreStates; (void)pushRestoreStates;

	r3dPoint3D position;

	switch( r_show_barricade_debug->GetInt() )
	{
	default:
		break;

	case 1:
		{
			r3dPoint3D scrCoord;
			if(r3dProjectToScreen(GetPosition(), &scrCoord))
			{
				Font_Editor->PrintF(scrCoord.x, scrCoord.y, r3dColor24( 0, 255, 200 ), "NetID: %d", GetNetworkID() );
			}
		}
		break;
	}
#endif
}

#ifdef VEHICLES_ENABLED
bool obj_Barricade::CanStopVehicle()
{
	switch( m_ItemID )
	{
	default:
		return false;
	// INB BARRICADE
	case WeaponConfig::ITEMID_BarBlockwallINB01:
	case WeaponConfig::ITEMID_BarBlockwallINB02:
	case WeaponConfig::ITEMID_BarConcreteINB01:
	case WeaponConfig::ITEMID_BarConcreteINB02:
	case WeaponConfig::ITEMID_BarConcreteINB03:
	case WeaponConfig::ITEMID_BarConcreteINB04:
	case WeaponConfig::ITEMID_BarConcreteINB05:
	case WeaponConfig::ITEMID_BarPlywoodINB:
	case WeaponConfig::ITEMID_BarSandbagINB:
	case WeaponConfig::ITEMID_BarScrapMetalINB:
	
	/// SS BARRICADE
	case WeaponConfig::ITEMID_SS_Arc_Shield:
	case WeaponConfig::ITEMID_SS_Bar_Concrete:
	case WeaponConfig::ITEMID_SS_Bar_MetalWood:
	case WeaponConfig::ITEMID_SS_Bar_Shield_Meta:
	case WeaponConfig::ITEMID_SS_Bar_Shield_Wood:
	case WeaponConfig::ITEMID_SS_Bar_WoodWall:
	case WeaponConfig::ITEMID_SS_Bar_FenceWall:

	// BURSTFIRE BARRICADE
	case WeaponConfig::ITEMID_BarPortablecoverBF:
		return false;
	}
}
#endif

struct BarricadeDeferredRenderable : MeshDeferredRenderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		BarricadeDeferredRenderable* This = static_cast< BarricadeDeferredRenderable* >( RThis );
		r3dApplyPreparedMeshVSConsts(This->Parent->preparedVSConsts);

		if(This->DrawState != rsCreateSM)
			This->Parent->m_PrivateModel->DrawMeshDeferred(r3dColor::white, 0);
		else
			This->Parent->m_PrivateModel->DrawMeshShadows();
	}

	obj_Barricade* Parent;
	eRenderStageID DrawState;
};

#ifndef FINAL_BUILD
struct BarricadeDebugRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		BarricadeDebugRenderable* This = static_cast<BarricadeDebugRenderable*>( RThis );

		This->Parent->DrawDebugInfo();
		
		r3dDrawBoundBox(This->Parent->GetBBoxWorld(), gCam, r3dColor24::green, 0.05f);
	}

	obj_Barricade* Parent;
};
#endif

void obj_Barricade::AppendShadowRenderables( RenderArray & rarr, int sliceIndex, const r3dCamera& Cam )
{
	if (!isRenderable)
		return;

	uint32_t prevCount = rarr.Count();
	m_PrivateModel->AppendShadowRenderables( rarr );
	for( uint32_t i = prevCount, e = rarr.Count(); i < e; i ++ )
	{
		BarricadeDeferredRenderable& rend = static_cast<BarricadeDeferredRenderable&>( rarr[ i ] );
		rend.Init();
		rend.Parent = this;
		rend.DrawState = rsCreateSM;
	}
}

void obj_Barricade::AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam )
{
	if (!isRenderable)
		return;

	uint32_t prevCount = render_arrays[ rsFillGBuffer ].Count();
	m_PrivateModel->AppendRenderablesDeferred( render_arrays[ rsFillGBuffer ], r3dColor::white, GetSelfIllumMultiplier() );
	for( uint32_t i = prevCount, e = render_arrays[ rsFillGBuffer ].Count(); i < e; i ++ )
	{
		BarricadeDeferredRenderable& rend = static_cast<BarricadeDeferredRenderable&>( render_arrays[ rsFillGBuffer ][ i ] );
		rend.Init();
		rend.Parent = this;
		rend.DrawState = rsFillGBuffer;
	}
#ifndef FINAL_BUILD
	if(r_show_barricade_debug->GetBool())
	{
		BarricadeDebugRenderable rend;
		rend.Init();
		rend.Parent		= this;
		rend.SortValue	= 2*RENDERABLE_USER_SORT_VALUE;
		render_arrays[ rsDrawFlashUI ].PushBack( rend );
	}
#endif
}

/*virtual*/
r3dMesh* obj_Barricade::GetObjectMesh() 
{
	return m_PrivateModel;
}

/*virtual*/
r3dMesh* obj_Barricade::GetObjectLodMesh()
{
	return m_PrivateModel;
}
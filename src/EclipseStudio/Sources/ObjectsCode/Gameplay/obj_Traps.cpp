//=========================================================================
//	Module: obj_Traps.cpp
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================
#include "r3dPCH.h"
#include "r3d.h"

#include "obj_Traps.h"
#include "../../EclipseStudio/Sources/multiplayer/ClientGameLogic.h"
#include "../../EclipseStudio/Sources/ObjectsCode/world/MaterialTypes.h"
#include "../../EclipseStudio/Sources/objectscode/weapons/weaponconfig.h"
#include "../../EclipseStudio/Sources/ObjectsCode/EFFECTS/obj_ParticleSystem.h"

extern bool g_bEditMode;

IMPLEMENT_CLASS(obj_Traps, "obj_Traps", "Object");
AUTOREGISTER_CLASS(obj_Traps);


obj_Traps::obj_Traps()
	: m_ItemID( -1 )
{
	m_CampFireSound = 0; //Bombillo:: Campfire Sound
	DisablePhysX = false;
	m_RotX = 0;
	ActualMesh = NULL;
	m_Activated = false;
}

obj_Traps::~obj_Traps()
{
}

BOOL obj_Traps::Load(const char *fname)
{
	return TRUE;
}

BOOL obj_Traps::OnCreate()
{
	m_ActionUI_Title = gLangMngr.getString("$FR_ActivateTrap");
	m_ActionUI_Msg = gLangMngr.getString("HoldEToActivateTrap");


	switch (m_ItemID)
	{
		case WeaponConfig::ITEMID_Traps_Bear:
				ActivateMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_Consumables\\inb_traps_bear_01_armed_tps.sco", true, false, true, true );
				ActualMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_Consumables\\inb_traps_bear_01_disarmed_tps.sco", true, false, true, true );
				//DisablePhysX  = true;
				break;
		case WeaponConfig::ITEMID_Traps_Spikes:
				ActivateMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_Consumables\\inb_traps_spikes_01_armed_tps.sco", true, false, true, true );
				ActualMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_Consumables\\inb_traps_spikes_01_disarmed_tps.sco", true, false, true, true );
				//DisablePhysX  = true;
				break;
		case WeaponConfig::ITEMID_Campfire:
				ActivateMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_Consumables\\INB_prop_campfire_01.sco", true, false, true, true );
				ActualMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_Consumables\\INB_prop_campfire_01.sco", true, false, true, true );
				particlefire = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "Campfire", GetPosition());
				LightFire = (obj_LightHelper*)srv_CreateGameObject("obj_LightHelper", "Omni", GetPosition() + r3dPoint3D(0,0.61,0));
				LightFire->LT.Intensity = 4.8f;
				LightFire->bOn = true;
				LightFire->innerRadius = 0.0f;
				LightFire->outerRadius = 4.0f;
				LightFire->bKilled = false;
				break;
		///////////////TRAPS - INB
		case WeaponConfig::ITEMID_BarbWireINB_01:
				ActivateMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_Consumables\\inb_barricade_barbwire_01.sco", true, false, true, true );
				ActualMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_Consumables\\inb_barricade_barbwire_01.sco", true, false, true, true );
				//DisablePhysX  = true;
				break;
		case WeaponConfig::ITEMID_BarbWireINB_02:
				ActivateMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_barricade_BarbWire_DX_01.sco", true, false, true, true );
				ActualMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_barricade_BarbWire_DX_01.sco", true, false, true, true );
				//DisablePhysX  = true;
				break;
		case WeaponConfig::ITEMID_WoodSpikeINB_01:
				ActivateMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_Consumables\\INB_Barricade_WoodSpike_01.sco", true, false, true, true );
				ActualMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_Consumables\\INB_Barricade_WoodSpike_01.sco", true, false, true, true );
				//DisablePhysX  = true;
				break;
		case WeaponConfig::ITEMID_WoodSpikeINB_02:
				ActivateMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_Barricade_WoodSpike_DX_01.sco", true, false, true, true );
				ActualMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_SurvivalStructures\\INB_Barricade_WoodSpike_DX_01.sco", true, false, true, true );
				//DisablePhysX  = true;
				break;
		case WeaponConfig::ITEMID_SpikeStripINB_01:
				ActivateMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_Consumables\\INB_Barricade_SpikeStrip_01.sco", true, false, true, true );
				ActualMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_Consumables\\INB_Barricade_SpikeStrip_01.sco", true, false, true, true );
				//DisablePhysX  = true;
				break;
		///////////////TRAPS - INB

		///////////////	TRAPS - BURSTFIRE
		case WeaponConfig::ITEMID_BarbWireBF:
				ActivateMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\TS_Tools\\tl_barbwire_extended.sco", true, false, true, true );
				ActualMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\TS_Tools\\tl_barbwire_extended.sco", true, false, true, true );
				//DisablePhysX  = true;
				break;
		///////////////	TRAPS - BURSTFIRE

		///////////////	TRAPS - SS
		case WeaponConfig::ITEMID_SS_Traps_Spike_Mat:
				ActivateMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\SS_Survival\\SS_Barricade_Spike_Mat_Deployed.sco", true, false, true, true );
				ActualMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\SS_Survival\\SS_Barricade_Spike_Mat_Deployed.sco", true, false, true, true );
				//DisablePhysX  = true;
				break;
		case WeaponConfig::ITEMID_SS_Traps_BarbWire:
				ActivateMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\SS_Survival\\SS_Barricade_BarbWire_Deployed.sco", true, false, true, true );
				ActualMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\SS_Survival\\SS_Barricade_BarbWire_Deployed.sco", true, false, true, true );
				//DisablePhysX  = true;
				break;

		case WeaponConfig::ITEMID_SS_Bar_MetalW_Spike:
				ActivateMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\SS_Survival\\SS_Barricade_MetalWood_01_Spike_Deployed.sco", true, false, true, true );
				ActualMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\SS_Survival\\SS_Barricade_MetalWood_01_Spike_Deployed.sco", true, false, true, true );
				//DisablePhysX  = true;
				break;
		case WeaponConfig::ITEMID_SS_Spike_Trap:
				ActivateMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\SS_Survival\\SS_Survival_Spike_Trap_Deployed_Armed.sco", true, false, true, true );
				ActualMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\SS_Survival\\SS_Survival_Spike_Trap_Deployed_DisArmed.sco", true, false, true, true );
				//DisablePhysX  = true;
				break;
		///////////////	TRAPS - SS

		///////////////NON TRAPS - INB
		case WeaponConfig::ITEMID_BarrDoorINB_01:
				ActivateMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_SurvivalStructures\\inb_barr_doormtl_door_Close.sco", true, false, true, true );
				ActualMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\INB_SurvivalStructures\\inb_barr_doormtl_door_Open.sco", true, false, true, true );
				//DisablePhysX  = true;
				break;
		///////////////NON TRAPS - INB
	}
	parent::MeshLOD[0] = ActualMesh;
	m_spawnPos = GetPosition();

	SAFE_DELETE(PhysicsObject);
	ReadPhysicsConfig();
	PhysicsConfig.group = PHYSCOLL_STATIC_GEOMETRY; // skip collision with players
	PhysicsConfig.requireNoBounceMaterial = true;
	PhysicsConfig.isFastMoving = true;
	if (m_ItemID != WeaponConfig::ITEMID_Campfire)
	///////////////TRAPS - INB
	if (m_ItemID != WeaponConfig::ITEMID_BarbWireINB_01)
	if (m_ItemID != WeaponConfig::ITEMID_BarbWireINB_02)
	if (m_ItemID != WeaponConfig::ITEMID_WoodSpikeINB_01)
	if (m_ItemID != WeaponConfig::ITEMID_WoodSpikeINB_02)
	if (m_ItemID != WeaponConfig::ITEMID_SpikeStripINB_01)
	///////////////TRAPS - INB
	///////////////TRAPS - SS
	if (m_ItemID != WeaponConfig::ITEMID_SS_Bar_MetalW_Spike)
	///////////////TRAPS - SS
	///////////////NON TRAPS - INB
	if (m_ItemID != WeaponConfig::ITEMID_BarrDoorINB_01)
	///////////////NON TRAPS - INB
	PhysicsConfig.isKinematic = true;

	/*if(DisablePhysX)
	{
		SAFE_DELETE(PhysicsObject);
		ReadPhysicsConfig();
		PhysicsConfig.group = PHYSCOLL_TINY_GEOMETRY; // skip collision with players
		PhysicsConfig.requireNoBounceMaterial = false;
		PhysicsConfig.isFastMoving = false;
	}*/
	SetRotationVector(r3dPoint3D(m_RotX, 0, 0));

	if(m_ItemID == WeaponConfig::ITEMID_Campfire)//Bombillo:: Campfire Sound
	{
		m_CampFireSound = SoundSys.Play(SoundSys.GetEventIDByPath("Sounds/AM_Group/Item_Action/Campfire_LP_01"), GetPosition()); 
	}

	return parent::OnCreate();
}

BOOL obj_Traps::OnDestroy()
{
	if(particlefire)
		particlefire->bKillDelayed = 1;
	if (LightFire != NULL)
	{
		LightFire = NULL;
	}

	if(m_CampFireSound) //Bombillo:: Campfire Sound
	{
		SoundSys.Stop(m_CampFireSound);
		SoundSys.Release(m_CampFireSound);
		m_CampFireSound = NULL;
	}

	return parent::OnDestroy();
}

BOOL obj_Traps::Update()
{
	return parent::Update();
}

void obj_Traps::Deactivate()
{
	parent::MeshLOD[0] = ActualMesh;
	SoundSys.PlayAndForget(SoundSys.GetEventIDByPath("Sounds/AM_Group/Item_Action/User_Action_Trap_Trigger_01"),GetPosition());
	//ExeParticle();
	m_Activated = false;
}

void obj_Traps::Activate(bool isme)
{
	parent::MeshLOD[0] = ActivateMesh;
	m_Activated = true;
	if (isme)
	{
		PKT_S2C_SetupTraps_s n;
		n.spawnID = toP2pNetId(GetNetworkID());
		n.m_Activate = 1;
		p2pSendToHost(NULL, &n, sizeof(n));
	}
}

void obj_Traps::ExeParticle()
{
	SpawnImpactParticle(r3dHash::MakeHash(""), r3dHash::MakeHash("Barricade_Smash_Riot"), GetPosition(), r3dPoint3D(0,1,0));
}
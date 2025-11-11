#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "obj_DroppedItem.h"

#include "ObjectsCode/weapons/WeaponArmory.h"
#include "../EFFECTS/obj_ParticleSystem.h"
#include "../../multiplayer/ClientGameLogic.h"
#include "../../EclipseStudio/Sources/ObjectsCode/world/MaterialTypes.h"
#include "../ai/AI_Player.H"

extern bool g_bEditMode;

IMPLEMENT_CLASS(obj_DroppedItem, "obj_DroppedItem", "Object");
AUTOREGISTER_CLASS(obj_DroppedItem);

obj_DroppedItem::obj_DroppedItem()
{
	m_AllowAsyncLoading = 1;
	NeedInitPhysics = 0;
	m_AirHigthPos = 200.0f;
	m_AirBHigthPos = 200.0f;
	m_IsOnTerrain = false;
	m_IsAirDrop = false;
	m_IsAirBomb = false;
	NetworLocal = false;
	AirDropPos = r3dPoint3D(0,0,0);
	AirBombPos = r3dPoint3D(0,0,0);
	AirCraftDistance = -600.0f;
	AirBombDistance = -600.0f;
	m_FirstTime = 0;
	AirCraft=NULL;
	AirBomb=NULL;
	Light1=NULL;
	Light2=NULL;
}

obj_DroppedItem::~obj_DroppedItem()
{
}

void obj_DroppedItem::SetHighlight( bool highlight )
{
	m_FillGBufferTarget = highlight ? rsFillGBufferAfterEffects : rsFillGBuffer;
}

bool obj_DroppedItem::GetHighlight() const
{
	return m_FillGBufferTarget == rsFillGBufferAfterEffects;
}

BOOL obj_DroppedItem::Load(const char *fname)
{
	return TRUE;
}

BOOL obj_DroppedItem::OnCreate()
{
	R3DPROFILE_FUNCTION( "obj_DroppedItem::OnCreate" );

#ifndef FINAL_BUILD
	if( g_bEditMode )
	{
		m_Item.itemID = 'GOLD';
		SetHighlight( true );
	}
#endif

	r3d_assert(m_Item.itemID);
	
	const char* cpMeshName = "";
	if(m_Item.itemID == 'GOLD')
	{
		switch(u_random(2)) 
		{
		default:
			case 0:
				cpMeshName = "Data\\ObjectsDepot\\Weapons\\Item_Money_Stack_01.sco";
				break;
			case 1: 
                 cpMeshName = "Data\\ObjectsDepot\\INB_Weapons\\item_money_stack_01.sco";
				 break;
		};
	}
	else if(m_Item.itemID == 'ARDR')
	{
		cpMeshName = "Data\\ObjectsDepot\\SS_Gameplay\\ss_supplydrop_static.sco";	
		if (m_FirstTime == 1)
		{
			SetObjFlags(OBJFLAG_SkipDraw | OBJFLAG_DisableShadows);
			//AirCraft create
			AirCraft = (obj_Building*)srv_CreateGameObject("obj_Building", "Data\\ObjectsDepot\\G3_Vehicles\\VTOLSHIP.sco", GetPosition());
			AirCraft->DrawDistanceSq = FLT_MAX;
			r3dscpy(AirCraft->m_sAnimName,"VTOLSHIP2.anm");
			AirCraft->m_bGlobalAnimFolder = 0;
			AirCraft->m_bAnimated = 1;
			sndAircraft = SoundSys.Play(SoundSys.GetEventIDByPath("Sounds/Vehicles/C130/Turbine"),AirCraft->GetPosition());
			SoundSys.Stop(sndAircraft);
		}
		
	}
	else if(m_Item.itemID == 'FLPS')
	{
		SpawnImpactParticle(r3dHash::MakeHash(""), r3dHash::MakeHash("Airdrop_Smash_FX"), GetPosition(), r3dPoint3D(0,1,0));
		cpMeshName = "Data\\ObjectsDepot\\Weapons\\item_flare_emergency01.sco";
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "aerialdropped", GetPosition());
	}
	else if(m_Item.itemID == 'ARBB')
	{
		cpMeshName = "Data\\ObjectsDepot\\ROTB_Chaos_Objects\\chaos_res_gas_barrel_01.sco";
	}
	else if(m_Item.itemID == 'FLBB')
	{
		SpawnImpactParticle(r3dHash::MakeHash(""), r3dHash::MakeHash("Airdrop_Bomb"), GetPosition(), r3dPoint3D(0,1,0));
		cpMeshName = "Data\\ObjectsDepot\\ROTB_AmmoShells\\shell_9mm.sco";
		Flare = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "explosion_bomb_01", GetPosition());
	}
	else if(m_Item.itemID == 'LOOT')
	{
		switch(u_random(57))
		{
		default:
			case 0: 
                 cpMeshName = "Data\\ObjectsDepot\\INB_Loot\\AM_StachBox_01.sco";
				 break;
			case 1: 
                 cpMeshName = "Data\\ObjectsDepot\\INB_Loot\\AM_Stachbox_01_Bronze.sco";
				 break;
			case 2: 
                 cpMeshName = "Data\\ObjectsDepot\\INB_Loot\\AM_StachBox_01_Diamond.sco";
				 break;
			case 3: 
                 cpMeshName = "Data\\ObjectsDepot\\INB_Loot\\AM_Stachbox_01_Gold.sco";
				 break;
			case 4: 
                 cpMeshName = "Data\\ObjectsDepot\\INB_Loot\\AM_StachBox_01_Handgun.sco";
				 break;
			case 5: 
                 cpMeshName = "Data\\ObjectsDepot\\INB_Loot\\AM_StachBox_01_Orange.sco";
				 break;
			case 6: 
                 cpMeshName = "Data\\ObjectsDepot\\INB_Loot\\AM_StachBox_01_Red.sco";
				 break;
			case 7: 
                 cpMeshName = "Data\\ObjectsDepot\\INB_Loot\\AM_StachBox_01_Shotgun.sco";
				 break;
			case 8: 
                 cpMeshName = "Data\\ObjectsDepot\\INB_Loot\\AM_stachbox_01_Silver.sco";
				 break;
			case 9: 
                 cpMeshName = "Data\\ObjectsDepot\\INB_Loot\\AM_StachBox_01_Survival.sco";
				 break;
			case 10: 
                 cpMeshName = "Data\\ObjectsDepot\\INB_Loot\\INB_Crate_Liberator_01.sco";
				 break;
			case 11: 
                 cpMeshName = "Data\\ObjectsDepot\\INB_Loot\\INB_Crate_Weapon_01_Carbon.sco";
				 break;
			case 12: 
                 cpMeshName = "Data\\ObjectsDepot\\INB_Loot\\INB_Crate_Weapon_01_DG.sco";
				 break;
			case 13: 
                 cpMeshName = "Data\\ObjectsDepot\\INB_Loot\\INB_Crate_Weapon_01_Red.sco";
				 break;
			case 14: 
                 cpMeshName = "Data\\ObjectsDepot\\INB_Loot\\INB_Crate_Weapon_02_Carbon.sco";
				 break;
			case 15: 
                 cpMeshName = "Data\\ObjectsDepot\\INB_Loot\\INB_mysteryCrates_01.sco";
				 break;
			case 16: 
                 cpMeshName = "Data\\ObjectsDepot\\INB_Consumables\\INB_Cons_Crate_Wooden_01.sco";
				 break;
			case 17: 
                 cpMeshName = "Data\\ObjectsDepot\\INB_Consumables\\INB_container_crafted_01.sco";
				 break;
			case 18: 
                 cpMeshName = "Data\\ObjectsDepot\\INB_Consumables\\INB_GardenBox_01.sco";
				 break;
			case 19: 
                 cpMeshName = "Data\\ObjectsDepot\\INB_Consumables\\INB_GiftBox_01.sco";
				 break;
				 
			// SS BOXES
			case 20: 
                 cpMeshName = "Data\\ObjectsDepot\\SS_Loot\\SS_Crate_Brotherhood1.sco";
				 break;
			case 21: 
                 cpMeshName = "Data\\ObjectsDepot\\SS_Loot\\SS_Crate_Brotherhood2.sco";
				 break;
			case 22: 
                 cpMeshName = "Data\\ObjectsDepot\\SS_Loot\\SS_Crate_Dominion.sco";
				 break;
			case 23: 
                 cpMeshName = "Data\\ObjectsDepot\\SS_Loot\\SS_Crate_Explorer1.sco";
				 break;
			case 24: 
                 cpMeshName = "Data\\ObjectsDepot\\SS_Loot\\SS_Crate_Taunts_02.sco";
				 break;
			case 25: 
                 cpMeshName = "Data\\ObjectsDepot\\SS_Loot\\SS_Crate_Taunts_03.sco";
				 break;
			case 26: 
                 cpMeshName = "Data\\ObjectsDepot\\SS_Loot\\SS_Crate_Helmets_01.sco";
				 break;
			case 27: 
                 cpMeshName = "Data\\ObjectsDepot\\SS_Loot\\SS_Crate_Hitman.sco";
				 break;
			case 28: 
                 cpMeshName = "Data\\ObjectsDepot\\SS_Loot\\SS_Crate_Mask_01.sco";
				 break;
			case 29: 
                 cpMeshName = "Data\\ObjectsDepot\\SS_Loot\\SS_Crate_Masks_02.sco";
				 break;
			case 30: 
                 cpMeshName = "Data\\ObjectsDepot\\SS_Loot\\SS_Crate_Taunts_01.sco";
				 break;
			case 31: 
                 cpMeshName = "Data\\ObjectsDepot\\SS_Loot\\SS_Crate_Winter2016.sco";
				 break;
			case 32: 
                 cpMeshName = "Data\\ObjectsDepot\\SS_Loot\\SS_Crate_Military1.sco";
				 break;
			case 33: 
                 cpMeshName = "Data\\ObjectsDepot\\SS_Loot\\SS_Crate_SpecOps.sco";
				 break;
			case 34: 
                 cpMeshName = "Data\\ObjectsDepot\\SS_Loot\\SS_Crate_Survivor1.sco";
				 break;
			case 35: 
                 cpMeshName = "Data\\ObjectsDepot\\SS_Loot\\SS_Crate_Survivor2.sco";
				 break;
			case 36: 
                 cpMeshName = "Data\\ObjectsDepot\\SS_Loot\\SS_Crate_Survivor3.sco";
				 break;
			case 37: 
                 cpMeshName = "Data\\ObjectsDepot\\SS_Loot\\Giftbox_Char_Att_03.sco";
				 break;
			case 38: 
                 cpMeshName = "Data\\ObjectsDepot\\SS_Loot\\Giftbox_Char_Att_02.sco";
				 break;
			case 39: 
                 cpMeshName = "Data\\ObjectsDepot\\SS_Loot\\Giftbox_Char_Att_01.sco";
				 break;
			case 40: 
                 cpMeshName = "Data\\ObjectsDepot\\SS_Loot\\Crate_UniqueSpawn.sco";
				 break;
			case 41: 
                 cpMeshName = "Data\\ObjectsDepot\\SS_Loot\\SS_Crate_GunSkin_04.sco";
				 break;
			case 42: 
                 cpMeshName = "Data\\ObjectsDepot\\SS_Loot\\SS_Crate_GunSkin_01.sco";
				 break;
			case 43: 
                 cpMeshName = "Data\\ObjectsDepot\\SS_Loot\\SS_Crate_GunSkin_05.sco";
				 break;
			case 44: 
                 cpMeshName = "Data\\ObjectsDepot\\SS_Loot\\SS_Crate_GunSkin_02.sco";
				 break;
			case 45: 
                 cpMeshName = "Data\\ObjectsDepot\\SS_Loot\\SS_Crate_GunSkin_06.sco";
				 break;
			case 46: 
                 cpMeshName = "Data\\ObjectsDepot\\SS_Loot\\SS_Crate_GunSkin_03.sco";
				 break;
			case 47: 
                 cpMeshName = "Data\\ObjectsDepot\\SS_Loot\\SS_Loot_Dufflebag_Crusaders.sco";
				 break;
			case 48: 
                 cpMeshName = "Data\\ObjectsDepot\\SS_Loot\\SS_Loot_Dufflebag_Dom.sco";
				 break;
			case 49: 
                 cpMeshName = "Data\\ObjectsDepot\\SS_Loot\\SS_Loot_Dufflebag_NewUser.sco";
				 break;
			case 50: 
                 cpMeshName = "Data\\ObjectsDepot\\SS_Loot\\SS_Loot_Dufflebag_01.sco";
				 break;
			case 51: 
                 cpMeshName = "Data\\ObjectsDepot\\SS_Loot\\SS_Loot_Dufflebag_Dom_Rare.sco";
				 break;
			case 52: 
                 cpMeshName = "Data\\ObjectsDepot\\SS_Loot\\SS_Loot_Dufflebag_Reward.sco";
				 break;
			case 53: 
                 cpMeshName = "Data\\ObjectsDepot\\SS_Loot\\SS_Loot_Dufflebag_Brohood.sco";
				 break;
			case 54: 
                 cpMeshName = "Data\\ObjectsDepot\\SS_Loot\\SS_Loot_Dufflebag_Comm.sco";
				 break;
			case 55: 
                 cpMeshName = "Data\\ObjectsDepot\\SS_Loot\\SS_Loot_Dufflebag_Merchants.sco";
				 break;
			case 56: 
                 cpMeshName = "Data\\ObjectsDepot\\SS_Loot\\SS_Loot_Dufflebag_Roadblock.sco";
				 break;
				 
			// EVENT ONLY
			/*case 0: 
                 cpMeshName = "Data\\ObjectsDepot\\INB_Loot\\AM_StachBox_01_Halloween.sco";
				 break;
			case 0: 
                 cpMeshName = "Data\\ObjectsDepot\\INB_Loot\\INB_mysteryCrates_01_Halloween.sco";
				 break;
			case 0: 
                 cpMeshName = "Data\\ObjectsDepot\\INB_Loot\\INB_mysteryCrates_01_Christmas.sco";
				 break;*/
		};
	}
	else if(m_Item.itemID == 'PRBX')
	{
		cpMeshName = "Data\\ObjectsDepot\\Weapons\\mil_box_wood_m_02.sco";
		m_bEnablePhysics=true;
	}
	else if(m_Item.itemID == 'ABX1')
	{
		cpMeshName = "Data\\ObjectsDepot\\SS_Gameplay\\ss_lootbox_01.sco";
		m_bEnablePhysics=true;
	}
	else if(m_Item.itemID == 'ABX2')
	{
		cpMeshName = "Data\\ObjectsDepot\\SS_Gameplay\\ss_lootbox_01.sco";
		m_bEnablePhysics=true;
	}
	else if(m_Item.itemID == 'ABX3')
	{
		cpMeshName = "Data\\ObjectsDepot\\SS_Gameplay\\ss_lootbox_01.sco";
		m_bEnablePhysics=true;
	}
	else if(m_Item.itemID == 'ABX4')
	{
		cpMeshName = "Data\\ObjectsDepot\\SS_Gameplay\\ss_lootbox_01.sco";
		m_bEnablePhysics=true;
	}
	else if(m_Item.itemID == 'ABX5')
	{
		cpMeshName = "Data\\ObjectsDepot\\SS_Gameplay\\ss_lootbox_01.sco";
		m_bEnablePhysics=true;
	}
	else if(m_Item.itemID == 'ABX6')
	{
		cpMeshName = "Data\\ObjectsDepot\\SS_Gameplay\\ss_lootbox_01.sco";
		m_bEnablePhysics=true;
	}
	else
	{
		const ModelItemConfig* cfg = (const ModelItemConfig*)g_pWeaponArmory->getConfig(m_Item.itemID);
		switch(cfg->category)
		{
			case storecat_Account:
			case storecat_Boost:
			case storecat_LootBox:
			case storecat_HeroPackage:
				r3dError("spawned item is not model");
				break;
		}
		cpMeshName = cfg->m_ModelPath;
	}
	
	if(!parent::Load(cpMeshName)) 
		return FALSE;

	if(m_Item.itemID == 'GOLD')
	{
		m_ActionUI_Title = gLangMngr.getString("$FR_Money");
		m_ActionUI_Msg = gLangMngr.getString("HoldEToPickUpMoney");
	}
	else if(m_Item.itemID == 'ARDR')
	{
		m_ActionUI_Title = gLangMngr.getString("$FR_Airdrop");
		m_ActionUI_Msg = gLangMngr.getString("HoldEToPickUpMoney");
	}
	else if(m_Item.itemID == 'ARBB')
	{
		m_ActionUI_Title = gLangMngr.getString("$FR_Airdrop");
		m_ActionUI_Msg = gLangMngr.getString("HoldEToPickUpMoney");
	}
	else if(m_Item.itemID == 'PRBX')
	{
		m_ActionUI_Title = gLangMngr.getString("$FR_Construction");
		m_ActionUI_Msg = gLangMngr.getString("HoldEToPickUpItem");
	}
	else if(m_Item.itemID == 'ABX1')
	{
		m_ActionUI_Title = gLangMngr.getString("$FR_SurvivalContainer");
		m_ActionUI_Msg = gLangMngr.getString("HoldEToPickUpItem");
	}
	else if(m_Item.itemID == 'ABX2')
	{
		m_ActionUI_Title = gLangMngr.getString("$FR_SurvivalContainerV2");
		m_ActionUI_Msg = gLangMngr.getString("HoldEToPickUpItem");
	}
	else if(m_Item.itemID == 'ABX3')
	{
		m_ActionUI_Title = gLangMngr.getString("$FR_GunnerContainer");
		m_ActionUI_Msg = gLangMngr.getString("HoldEToPickUpItem");
	}
	else if(m_Item.itemID == 'ABX4')
	{
		m_ActionUI_Title = gLangMngr.getString("$FR_KnifeContainer");
		m_ActionUI_Msg = gLangMngr.getString("HoldEToPickUpItem");
	}
	else if(m_Item.itemID == 'ABX5')
	{
		m_ActionUI_Title = gLangMngr.getString("$FR_GearContainer");
		m_ActionUI_Msg = gLangMngr.getString("HoldEToPickUpItem");
	}
	else if(m_Item.itemID == 'ABX6')
	{
		m_ActionUI_Title = gLangMngr.getString("$FR_PremiumContainer");
		m_ActionUI_Msg = gLangMngr.getString("HoldEToPickUpItem");
	}
	else if(m_Item.itemID == 'FLPS')
	{
		m_ActionUI_Title = gLangMngr.getString("$FR_Item");
		m_ActionUI_Msg = gLangMngr.getString("HoldEToPickUpItem");
	}
	else if(m_Item.itemID == 'FLBB')
	{
		m_ActionUI_Title = gLangMngr.getString("$FR_Item");
		m_ActionUI_Msg = gLangMngr.getString("HoldEToPickUpItem");
	}
	else if(m_Item.itemID == 'LOOT')
	{
		m_ActionUI_Title = gLangMngr.getString("$FR_LOOT");
		m_ActionUI_Msg = gLangMngr.getString("HoldEToPickUpItem");
	}
	else
	{
		const BaseItemConfig* cfg = g_pWeaponArmory->getConfig(m_Item.itemID);
		m_ActionUI_Title = cfg->m_StoreName;
		m_ActionUI_Msg = gLangMngr.getString("HoldEToPickUpItem");
	}

	
	if(m_Item.itemID != 'PRBX' && m_Item.itemID != 'ARDR' && m_Item.itemID != 'ARBB' && m_Item.itemID != 'ABX1' && m_Item.itemID != 'ABX2' && m_Item.itemID != 'ABX3' && m_Item.itemID != 'ABX4' && m_Item.itemID != 'ABX5' && m_Item.itemID != 'ABX6')
	{
		ReadPhysicsConfig();
		PhysicsConfig.isDynamic = false; // false = default setting // to prevent items from falling through the ground
		PhysicsConfig.isKinematic = true; // to prevent them from being completely static, as moving static object is causing physx performance issues, and we cannot place item at this point, as it is not loaded fully yet
		PhysicsConfig.group = PHYSCOLL_TINY_GEOMETRY; // skip collision with players
		PhysicsConfig.requireNoBounceMaterial = false; // default = false
		PhysicsConfig.isFastMoving = false; // fucked up PhysX CCD is crashing all the time
	
		SetPosition(GetPosition()+r3dPoint3D(0,0.25f,0));
	}
	if(m_Item.itemID == 'ARDR')
	{
		//AirCraft Position Start and Sound
		if (AirCraft!=NULL) 
			AirCraft->SetPosition(m_spawnPos+r3dPoint3D(0,0,AirCraftDistance));

		m_spawnPos = AirDropPos;
		SetPosition(m_spawnPos);
		m_IsAirDrop = true;
		m_bEnablePhysics = true;
	}
	else if(m_Item.itemID == 'ARBB')
	{
		//AirCraft Position Start and Sound
		if (AirBomb!=NULL) 
			AirBomb->SetPosition(m_spawnPos+r3dPoint3D(0,0,AirBombDistance));

		m_spawnPos = AirBombPos;
		SetPosition(m_spawnPos);
		m_IsAirBomb = true;
		m_bEnablePhysics = true;
	}
	else 
	{
		m_spawnPos = GetPosition();
	}

	// will have to create it later - when we are loaded.
	if( m_bEnablePhysics && m_IsAirDrop == false  && m_IsAirBomb == false )
	{
		NeedInitPhysics = 1;
		m_bEnablePhysics = 0;
	}

	parent::OnCreate();

	return 1;
}

void obj_DroppedItem::StartLights()
{
	if (Light1==NULL)
	{
		Light1 = (obj_LightHelper*)srv_CreateGameObject("obj_LightHelper", "Omni", GetPosition() + r3dPoint3D(0,3,0));
		Light1->Color = r3dColor::white;
		Light1->LT.Intensity = 2.0f;
		Light1->bOn = true;
		Light1->innerRadius = 0.0f;
		Light1->outerRadius = 8.37f;
		Light1->bKilled = false;
		Light1->DrawDistanceSq = FLT_MAX;
		Light1->SetPosition(GetPosition()+r3dPoint3D(0,0,3.11f));
	}

	if (Light2==NULL)
	{
		Light2 = (obj_LightHelper*)srv_CreateGameObject("obj_LightHelper", "Omni", GetPosition() + r3dPoint3D(0,3,0));
		Light2->Color = r3dColor::white;
		Light2->LT.Intensity = 2.0f;
		Light2->bOn = true;
		Light2->innerRadius = 0.0f;
		Light2->outerRadius = 8.37f;
		Light2->bKilled = false;
		Light2->DrawDistanceSq = FLT_MAX;
		Light2->SetPosition(GetPosition()+r3dPoint3D(0,0,-3.11f));
	}
}

void obj_DroppedItem::UpdateObjectPositionAfterCreation()
{
	if(!PhysicsObject)
		return;

	PxActor* actor = PhysicsObject->getPhysicsActor();
	if(!actor)
		return;

	PxBounds3 pxBbox = actor->getWorldBounds();
	PxVec3 pxCenter = pxBbox.getCenter();

	// place object on the ground, to prevent excessive bouncing
	{
		PxRaycastHit hit;
		PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK, 0, 0, 0), PxSceneQueryFilterFlag::eSTATIC);
		if(g_pPhysicsWorld->raycastSingle(PxVec3(pxCenter.x, pxCenter.y, pxCenter.z), PxVec3(0, -1, 0), 50.0f, PxSceneQueryFlag::eIMPACT, hit, filter))
		{
			float diff = GetPosition().y - pxBbox.minimum.y;
			SetPosition(r3dPoint3D(hit.impact.x, hit.impact.y+diff, hit.impact.z));
		}
	}
}

BOOL obj_DroppedItem::OnDestroy()
{

	if (SoundSys.IsHandleValid(sndAircraft))
	{
		SoundSys.Release(sndAircraft);
		sndAircraft = NULL;
	}
	if (AirCraft!=NULL)
		AirCraft = NULL;
	
	if (SoundSys.IsHandleValid(sndAirBomb))
	{
		SoundSys.Release(sndAirBomb);
		sndAirBomb = NULL;
	}
	if (AirBomb!=NULL)
		AirBomb = NULL;

	if (Light1!=NULL)
		Light1=NULL;
	if (Light2!=NULL)
		Light2=NULL;

	return parent::OnDestroy();
}

BOOL obj_DroppedItem::Update()
{
	if( NeedInitPhysics && MeshLOD[ 0 ] && MeshLOD[ 0 ]->IsDrawable() && m_Item.itemID != 'ARDR' && m_Item.itemID != 'ARBB')
	{
		m_bEnablePhysics = 1;
		CreatePhysicsData();
		NeedInitPhysics = 0;
		UpdateObjectPositionAfterCreation();
	}

	if (AirCraft!=NULL && NetworLocal)
	{
		float dist = (AirCraft->GetPosition() - m_spawnPos).Length();

		if(dist<1.8f)
		{
			ResetObjFlags(OBJFLAG_SkipDraw | OBJFLAG_DisableShadows);
			//StartLights();
		}

		AirCraft->SetPosition(m_spawnPos+r3dPoint3D(0,0,AirCraftDistance));
		SoundSys.SetSoundPos(sndAircraft,AirCraft->GetPosition());
		AirCraftDistance+=2.0f;

		if (!SoundSys.isPlaying(sndAircraft))
			SoundSys.Start(sndAircraft);

		if (AirCraftDistance>600.0f)
		{
			SoundSys.Stop(sndAircraft);
			AirCraft->setActiveFlag(0);
			AirCraft = NULL;
		}
	}
	if (AirBomb!=NULL && NetworLocal)
	{
		float dist = (AirBomb->GetPosition() - m_spawnPos).Length();

		if(dist<1.8f)
		{
			ResetObjFlags(OBJFLAG_SkipDraw | OBJFLAG_DisableShadows);
			//StartLights();
		}

		AirBomb->SetPosition(m_spawnPos+r3dPoint3D(0,0,AirBombDistance));
		SoundSys.SetSoundPos(sndAirBomb, AirBomb->GetPosition());
		AirBombDistance+=2.0f;

		if (!SoundSys.isPlaying(sndAirBomb))
			SoundSys.Start(sndAirBomb);

		if (AirBombDistance>600.0f)
		{
			SoundSys.Stop(sndAirBomb);
			AirBomb->setActiveFlag(0);
			AirBomb = NULL;
		}
	}

	parent::Update();
	
	r3dPoint3D pos = GetBBoxWorld().Center();

#ifndef FINAL_BUILD
	if( !g_bEditMode )
#endif
	{
		const ClientGameLogic& CGL = gClientLogic();
		if(CGL.localPlayer_ && (CGL.localPlayer_->GetPosition() - pos).Length() < 3.0f)
			SetHighlight(true);
		else
			SetHighlight(false);
	}

	return TRUE;
}

void obj_DroppedItem::ServerPost(float posY)
{
	if (!NetworLocal)
		return;

		m_spawnPos.y=posY;
		SetPosition(m_spawnPos);

		if (Light1!=NULL)
		{
			Light1->SetPosition(r3dPoint3D(Light1->GetPosition().x,posY+1,Light1->GetPosition().z));
		}
		if (Light2!=NULL)
		{
			Light2->SetPosition(r3dPoint3D(Light2->GetPosition().x,posY+1,Light2->GetPosition().z));
		}
}

void obj_DroppedItem::AppendRenderables( RenderArray ( & render_arrays )[ rsCount ], const r3dCamera& Cam )
{
	MeshGameObject::AppendRenderables( render_arrays, Cam );

	if( GetHighlight() )
	{
		MeshObjDeferredHighlightRenderable rend;
		rend.Init( MeshGameObject::GetObjectLodMesh(), this, NULL ); // DEFAULT CATEGORY RENDER COLOR
		//rend.Init( MeshGameObject::GetObjectLodMesh(), this, r3dColor::green.GetPacked() ); // DEFAULT CATEGORY RENDER COLOR
		const WeaponConfig* wc = g_pWeaponArmory->getWeaponConfig(m_Item.itemID);
		if(wc)
		{
			rend.Init( MeshGameObject::GetObjectLodMesh(), this, r3dColor::white.GetPacked() );
		}
		const BackpackConfig* bc = g_pWeaponArmory->getBackpackConfig(m_Item.itemID);
		if(bc)
		{
			rend.Init( MeshGameObject::GetObjectLodMesh(), this, r3dColor::lightgreen.GetPacked() );
		}
		const FoodConfig* fc = g_pWeaponArmory->getFoodConfig(m_Item.itemID);
		if(fc)
		{
			rend.Init( MeshGameObject::GetObjectLodMesh(), this, r3dColor::lightred.GetPacked() );
		}
		const WeaponAttachmentConfig* wa = g_pWeaponArmory->getAttachmentConfig(m_Item.itemID);
		if(wa)
		{
			rend.Init( MeshGameObject::GetObjectLodMesh(), this, r3dColor::blue.GetPacked() );
		}
		const GearConfig* gc = g_pWeaponArmory->getGearConfig(m_Item.itemID);
		if(gc)
		{
			rend.Init( MeshGameObject::GetObjectLodMesh(), this, r3dColor::lightblue.GetPacked() );
		}
		const CraftComponentConfig* cc = g_pWeaponArmory->getCraftComponentConfig(m_Item.itemID);
		if(cc)
		{
			rend.Init( MeshGameObject::GetObjectLodMesh(), this, r3dColor::lightyellow.GetPacked() );
		}
		const CraftRecipeConfig* cr = g_pWeaponArmory->getCraftRecipeConfig(m_Item.itemID);
		if(cr)
		{
			rend.Init( MeshGameObject::GetObjectLodMesh(), this, r3dColor::green.GetPacked() );
		}
		
		rend.SortValue = 0;
		rend.DoExtrude = 0;
		
		render_arrays[ rsDrawHighlightPass0 ].PushBack( rend );

		rend.DoExtrude = 1;
		render_arrays[ rsDrawHighlightPass1 ].PushBack( rend );
	}
}



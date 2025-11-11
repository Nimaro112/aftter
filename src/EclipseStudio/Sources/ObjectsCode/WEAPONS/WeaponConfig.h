#pragma once

#include "BaseItemConfig.h"

enum WeaponAnimTypeEnum
{
	WPN_ANIM_ASSAULT,
	WPN_ANIM_PISTOL,
	WPN_ANIM_GRENADE,
	WPN_ANIM_RPG,
	WPN_ANIM_SMG,
	WPN_ANIM_MINE,
	WPN_ANIM_MELEE,

	WPN_ANIM_COUNT
};

enum WeaponFiremodeEnum
{
	WPN_FRM_SINGLE=1<<0,
	WPN_FRM_TRIPLE=1<<1,
	WPN_FRM_AUTO=1<<2,
};

#include "../../../ServerNetPackets/NetPacketsWeaponInfo.h"

extern const char* WeaponAttachmentBoneNames[WPN_ATTM_MAX];

class WeaponAttachmentConfig : public ModelItemConfig
{
private:
	mutable r3dMesh* m_Model_AIM; // second model for when you are aiming. enabled only for scopes right now
public:
	// config
	WeaponAttachmentTypeEnum m_type;
	int m_specID;

	char* m_MuzzleParticle;

	char* m_ScopeAnimPath;

	// mods
	float	m_Damage;
	float	m_Range;
	int		m_Firerate;
	float	m_Recoil;
	float	m_Spread;
	int		m_Clipsize;
	const struct ScopeConfig* m_scopeConfig;
	const struct ScopeConfig* m_scopeConfigTPS; // spec.config for when you are switching to TPS mode
	float	m_scopeZoom; // 0..1; 0 - no zoom. 1 - maximum zoom

	// new weapon sounds IDs
	int		m_sndFireID_single;
	int		m_sndFireID_auto;
	int		m_sndFireID_single_player; // for local player
	int		m_sndFireID_auto_player; // for local player

	// durability & repairing
	float	m_DurabilityUse;	// per use, in %
	float	m_RepairAmount;
	float	m_PremRepairAmount;
	int	m_RepairPriceGD;

public:
	WeaponAttachmentConfig(uint32_t id) : ModelItemConfig(id)
	{
		category = storecat_FPSAttachment;
		m_type = WPN_ATTM_INVALID;
		m_specID = 0;
		m_Model_AIM = NULL;
		m_MuzzleParticle = NULL;

		m_ScopeAnimPath = NULL;

		m_Damage = 0.0f;
		m_Range = 0.0f;
		m_Firerate = 0;
		m_Recoil = 0.0f;
		m_Spread = 0.0f;
		m_Clipsize = 0;
		m_scopeConfig = NULL;
		m_scopeConfigTPS = NULL;
		m_scopeZoom= 1.0f; 

		m_sndFireID_single = -1;
		m_sndFireID_auto   = -1;
		m_sndFireID_single_player = -1;
		m_sndFireID_auto_player = -1;

		m_DurabilityUse = 0.0f;
		m_RepairAmount = 0.0f;
		m_PremRepairAmount = 0.0f;
		m_RepairPriceGD = 0;

	}
	virtual ~WeaponAttachmentConfig() 
	{
		free(m_MuzzleParticle);
		free(m_ScopeAnimPath);
	}
	virtual bool loadBaseFromXml(pugi::xml_node& xmlAttachment);

	void copyParametersTo(GBAttmInfo& wi) const
	{
#define DO(XX) wi.XX = XX
		DO(m_DurabilityUse);
		DO(m_RepairAmount);
		DO(m_PremRepairAmount);
		DO(m_RepairPriceGD);
#undef DO
	}
	void copyParametersFrom(const GBAttmInfo& wi)
	{
#define DO(XX) XX = wi.XX
		DO(m_DurabilityUse);
		DO(m_RepairAmount);
		DO(m_PremRepairAmount);
		DO(m_RepairPriceGD);
#undef DO
	}

	r3dMesh* getMesh( bool allow_async_loading, bool aim_model ) const;

	int getAimMeshRefs() const ;

	// called when unloading level
	virtual void resetMesh() { ModelItemConfig::resetMesh(); m_Model_AIM = 0; }
};

class WeaponConfig : public ModelItemConfig
{
private:
	mutable r3dMesh* m_Model_FPS;
	mutable int	m_ModelRefCount ;

public:

	mutable r3dSkeleton* m_Model_FPS_Skeleton;
	mutable r3dAnimPool*  m_AnimPool_FPS;
	
	// PTUMIK: if adding new skill based items, make sure to add them to DB FN_VALIDATE_LOADOUT proc and also to CUserProfile::isValidInventoryItem()
	enum EUsableItemIDs
	{
		// UNARMED - MELEE : Don't touch this
		ITEMID_UnarmedMelee				= 101398,
		ITEMID_Garden_Box 				= 111361, // not ingame
		ITEMID_AirdropContainer 		= 1996, // not ingame
		
		
		/// CONSTRUCTION COMPONENTS - ROTB
		ITEMID_ConstructorBlockSmall 	= 103000,
		ITEMID_ConstructorBlockBig 		= 103001,
		ITEMID_ConstructorBlockCircle 	= 103002,
		ITEMID_ConstructorColum1 		= 103003,
		ITEMID_ConstructorColum2 		= 103004,
		ITEMID_ConstructorColum3 		= 103005,
		ITEMID_ConstructorFloor1 		= 103006,
		ITEMID_ConstructorFloor2 		= 103007,
		ITEMID_ConstructorCeiling1 		= 103008,
		ITEMID_ConstructorCeiling2 		= 103009,
		ITEMID_ConstructorCeiling3 		= 103010,
		ITEMID_ConstructorWallMetalic 	= 103011,
		ITEMID_ConstructorSlope 		= 103012,
		ITEMID_ConstructorWall1 		= 103013,
		ITEMID_ConstructorWall2 		= 103014,
		ITEMID_ConstructorWall3 		= 103015,
		ITEMID_ConstructorWall4 		= 103016,
		ITEMID_ConstructorWall5 		= 103017,
		ITEMID_ConstructorWODBOX 		= 103018,
		ITEMID_ConstructorBaseBunker 	= 103019,
		
		/// CONSTRUCTION COMPONENTS - INB
		ITEMID_ConstructionINB000 		= 104000,
		ITEMID_ConstructionINB001		= 104001,
		ITEMID_ConstructionINB002 		= 104002,
		ITEMID_ConstructionINB003 		= 104003,
		ITEMID_ConstructionINB004 		= 104004,
		ITEMID_ConstructionINB005 		= 104005,
		ITEMID_ConstructionINB006 		= 104006,
		ITEMID_ConstructionINB007 		= 104007,
		ITEMID_ConstructionINB008 		= 104008,
		ITEMID_ConstructionINB009 		= 104009,
		ITEMID_ConstructionINB010 		= 104010,
		ITEMID_ConstructionINB011 		= 104011,
		ITEMID_ConstructionINB012 		= 104012,
		ITEMID_ConstructionINB013 		= 104013,
		ITEMID_ConstructionINB014 		= 104014,
		ITEMID_ConstructionINB015 		= 104015,
		ITEMID_ConstructionINB016 		= 104016,
		ITEMID_ConstructionINB017 		= 104017,
		ITEMID_ConstructionINB018 		= 104018,
		ITEMID_ConstructionINB019 		= 104019,
		ITEMID_ConstructionINB020 		= 104020,
		ITEMID_ConstructionINB021 		= 104021,
		ITEMID_ConstructionINB022 		= 104022,
		ITEMID_ConstructionINB023 		= 104023,
		ITEMID_ConstructionINB024 		= 104024,
		
		/// BARRICADES COMPONENTS - INB
		ITEMID_BarBlockwallINB01 		= 104025,
		ITEMID_BarBlockwallINB02 		= 104026,
		ITEMID_BarConcreteINB01  		= 104027,
		ITEMID_BarConcreteINB02  		= 104028,
		ITEMID_BarConcreteINB03  		= 104029,
		ITEMID_BarConcreteINB04  		= 104030,
		ITEMID_BarConcreteINB05  		= 104031,
		ITEMID_BarPlywoodINB	 		= 104032,
		ITEMID_BarSandbagINB 	 		= 104033,
		ITEMID_BarScrapMetalINB  		= 104034,
		
		/// BARRICADES COMPONENTS - SS
		ITEMID_SS_Arc_Shield 			= 104235,
		ITEMID_SS_Bar_Concrete 			= 104136,
		ITEMID_SS_Bar_MetalWood 		= 104137,
		ITEMID_SS_Bar_Shield_Meta		= 104138, // destroy fx bug
		ITEMID_SS_Bar_Shield_Wood 		= 104139,
		ITEMID_SS_Bar_WoodWall 			= 104140,
		ITEMID_SS_Bar_FenceWall 		= 104141,
		
		/// BARRICADES COMPONENTS - BURSTFIRE
		ITEMID_BarPortablecoverBF 		= 104035,

		// BARRICADE W/ DAMAGE COMPONENTS - INB
		ITEMID_BarbWireINB_01	  		= 105000, //barricades with damage - based on trap
		ITEMID_BarbWireINB_02 	  		= 105001, //barricades with damage - based on trap
		ITEMID_WoodSpikeINB_01 	  		= 105002, //barricades with damage - based on trap
		ITEMID_WoodSpikeINB_02 	  		= 105003, //barricades with damage - based on trap
		
		// BARRICADE W/ DAMAGE COMPONENTS - SS
		ITEMID_SS_Bar_MetalW_Spike	  	= 105020, // dmg trap
		
		/// TRAPS COMPONENTS - SS
		ITEMID_SS_Traps_Spike_Mat	  	= 105017, // dmg trap
		ITEMID_SS_Traps_BarbWire	  	= 105018, // dmg trap
		ITEMID_SS_Spike_Trap	  		= 105019, // active trap
		
		/// TRAPS COMPONENTS - BURSTFIRE
		ITEMID_BarbWireBF   	  		= 104036,
		
		/// TRAPS COMPONENTS - INB
		ITEMID_SpikeStripINB_01   		= 105004, // trap
		ITEMID_Campfire 		  		= 105006,
		ITEMID_Traps_Bear 		  		= 105007,
		ITEMID_Traps_Spikes		 		= 105008,
		
		/// HARVEST COMPONENTS
		ITEMID_GardenTrap_Rabbit  		= 103023, // farm item - based on trap
		ITEMID_Garden_Box_Lettuce 		= 105010, // farm item - based on trap
		ITEMID_Garden_Box_Tomatos 		= 111363, // farm item - based on trap
		ITEMID_Garden_Box_Beets   		= 111364, // farm item - based on trap
		ITEMID_Garden_Box_Broccoli		= 111365, // farm item - based on trap
		ITEMID_Garden_Box_Carrots 		= 111366, // farm item - based on trap
		ITEMID_Garden_Box_Potatos 		= 111367, // farm item - based on trap
		ITEMID_Garden_Box_Chicken 		= 111368, // chicken farm
		ITEMID_SolarWaterPurifier 		= 101360, // farm water just in the day : 6am - 20pm
		
		/// SAFE LOCKER VAULT COMPONENTS - INB
		ITEMID_Locker 			  		= 101348,
		ITEMID_SS_Locker 			  	= 101349,
		
		/// COOKING RACK COMPONENTS
		// INB FOOD FOR COOK ---
		ITEMID_INB_Cons_Can_Beans 		= 100105,
		ITEMID_INB_Cons_Can_Cheese		= 100103,
		ITEMID_INB_Cons_Can_Chowder 	= 100098,
		ITEMID_INB_Cons_Can_Mushroom 	= 100101,
		ITEMID_INB_Cons_Can_Stew 		= 100052,
		ITEMID_INB_Cons_Meat_Raw 		= 100062,
		ITEMID_INB_Cons_Veg_Beet 		= 100085,
		ITEMID_INB_Cons_Veg_Broccoli 	= 100068,
		ITEMID_INB_Cons_Veg_Potato 		= 100073,
		// DIRTY WATER
		ITEMID_INB_Cons_water_S_Dirty 	= 100060, // INB Dirty Water - CookingRack
		ITEMID_INB_Cons_water_L_Dirty 	= 100058, // INB Dirty Water - CookingRack
		ITEMID_SS_Food_Water_Dirty	 	= 100235, // SS Dirty Water - CookingRack
		// INB FOOD FOR COOK ---
		// SS FOOD FOR COOK ---
		ITEMID_SS_Food_Can_Ham_01 		= 100218,
		ITEMID_SS_Food_Can_Stew_01 		= 100219,
		ITEMID_SS_Food_Can_Peachs_01 	= 100220,
		ITEMID_SS_Food_Can_Tuna_01 		= 100221,
		ITEMID_SS_Food_Can_Mango_01 	= 100222,
		ITEMID_SS_Food_Can_CHKNSoup_01 	= 100223,
		// SS FOOD FOR COOK ---
		
		/// FILL BOTTLE COMPONENTS
		ITEMID_INB_Cons_water_L_Empty 	= 100059,
		ITEMID_INB_Cons_water_S_Empty 	= 100160,
		// SS EMPTY BOTTLES
		ITEMID_SS_Food_Flask_Empty 		= 100236,
		ITEMID_SS_Food_Water_Empty_01 	= 100237,
		
		/// MEDICAL COMPONENTS
		ITEMID_Defibrillator 			= 101300,
		ITEMID_ZombieRepellent 			= 101301, // need fix the anim
		ITEMID_C01Vaccine 	   			= 101302, // need fix the anim
		ITEMID_Bandages  	   			= 101261, // need obj pivot / position
		ITEMID_Medkit 		   			= 101304, // need obj pivot / position but can be use by now
		ITEMID_Suture_Kit 	   			= 101256, // need fix the anim
		ITEMID_Homebrew_Bandage			= 101262, // need fix the anim
		
		ITEMID_Med_Charcoal				= 100033,
		ITEMID_Med_Alcohol				= 100031,
		ITEMID_Med_Antibiotics			= 100028,
		ITEMID_Med_Tylenol				= 100035,
		ITEMID_Med_Pills				= 100124,
		ITEMID_GauzePads				= 100125,
		ITEMID_Med_Stomach				= 100027,
		

		// EXPLOSIVES COMPONENTS - BURSTFIRE
		ITEMID_FragGrenadeBF	  		= 101152,
		ITEMID_SmokeGrenadeBF	  		= 101153,
		ITEMID_MiniChargeBF	  			= 101154,
		
		// EXPLOSIVES COMPONENTS - INB
		ITEMID_FlashBang	  			= 101403,
		ITEMID_Grenade_HB 	  			= 101405,
		ITEMID_IncenGrenade   			= 101416,
		ITEMID_FragGrenade 	  			= 101310,
		ITEMID_FragGrenade_02 			= 101404,
				// SMOKE EXPLOSIVES - INB
		ITEMID_smokeg_white   			= 101147, 
		ITEMID_smokeg_red     			= 101148,
		ITEMID_smokeg_blue    			= 101149,
		ITEMID_smokeg_green   			= 101150, 
		ITEMID_smokeg_orange  			= 101151,
				// CHEMLIGHTS FLARE - INB
		ITEMID_ChemLight      			= 101311,
		ITEMID_ChemLightBlue  			= 101325,
		ITEMID_ChemLightGreen 			= 101326,
		ITEMID_ChemLightOrange			= 101327,
		ITEMID_ChemLightRed   			= 101328,
		ITEMID_ChemLightYellow			= 101329,
		ITEMID_Flare 		  			= 101312,
		
		// SPECIAL GUNS - WARZ
		ITEMID_FlareGun 				= 101320,
		
		// FUEL COMPONENTS - INB
		ITEMID_Gas 						= 301321,
		ITEMID_EmptyJerryCan 			= 301325,
		
		// Resource collection tool (melee weapon that allows to collect resources)
		ITEMID_ResourceCollectionTool 	= 101408,
		
		// SURVIVAL COMPONENTS
		ITEMID_PlaceableLight 			= 101361, // INB -- floodlight
		
		ITEMID_SmallPowerGenerator 		= 101359, // WARZ - NEED REWORK
		ITEMID_BigPowerGenerator 		= 101358, // WARZ - NEED REWORK
		ITEMID_RangeFinder 				= 101319, // INB - ITEM ???
		ITEMID_AirHorn 					= 101323, // WARZ - NEED REWORK
		ITEMID_Binoculars 				= 101315, // WARZ - NEED REWORK
		
		// REPAIR BOXES
		ITEMID_RepairKit			 	= 101399,
		ITEMID_PremRepairKit 			= 101400,
		
		// DIARIE
		ITEMID_Diarie 					= 101305, // BURSTFIRE ITEM
		
		// CARS SPAWN ITEMS
		ITEMID_TruckCar 				= 101259,
		ITEMID_PoliceCar 				= 101260,
		
		
		// SCANS COMPONENTS
		ITEMID_SpyDrone 				= 103025, // AIRDRONE
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////// DONE - COMPONENTS ABOVE
		
		// I-GAME CASE SYSTEM COMPONENTS
		ITEMID_AssaultCase 				= 800130,
		ITEMID_SniperCase 				= 800131,
		ITEMID_ShotgunCase 				= 800132,
		ITEMID_HandgunCase 				= 800134,
		ITEMID_GearCase 				= 800135,
		ITEMID_MeleeCase 				= 800136,
		ITEMID_SurvivalCase 			= 800137,
		ITEMID_CraftingCase 			= 800138,
		ITEMID_GlobalCase 				= 800139,
		
		
		// AIRDROP COMPONENTS
		ITEMID_AerialDelivery 			= 103020, // aerial delivery controller
		ITEMID_BombPhone	 			= 103021, // aerial delivery controller
		
		ITEMID_SurvivalContainer 		= 101131, // airdrop box
		ITEMID_SurvivalContainerV2  	= 101132, // airdrop box
		ITEMID_GunnerContainer      	= 101133, // airdrop box
		ITEMID_KnifeContainer 	 		= 101134, // airdrop box
		ITEMID_GearContainer 	  		= 101135, // airdrop box
		ITEMID_PremiumContainer 		= 101136, // airdrop box

		// ADD BACKPACKS TO DROPPED BACKPACK SYSTEM
		ITEMID_Backpack1  				= 20176, // SLING Backpack Lv. 1
		ITEMID_Backpack2  				= 20277, // Backpack Lv. 2
		ITEMID_Backpack3 			 	= 20478, // Backpack Lv. 3
		ITEMID_Backpack4  				= 20279, // Backpack Lv. 4
		ITEMID_Backpack5  				= 20284, // TRAILMAKER PACK Lv. 4
		ITEMID_Backpack6  				= 20285, // LEGACY PACK Lv. 4
		ITEMID_Backpack7  				= 20286, // SUPERBREAK PACK Lv. 4
		ITEMID_Backpack8  				= 20280, // ASSAULT PACK Lv. 5
		ITEMID_Backpack9  				= 20281, // FOREST PACK Lv. 5
		ITEMID_Backpack10  				= 20287, // DEATHSKULL PACK Lv. 4
		ITEMID_Backpack11  				= 20282, // WHITENOISE PACK Lv. 5

		///////////////NON TRAPS - INB
		ITEMID_BarrDoorINB_01    	 	= 105005, // IN-COMPLETE

		// INPROGRESS
		ITEMID_Claymore 				= 101139,
		// INPROGRESS
		
		// BONUS +++ TANK AMMO
		ITEMID_TankShell 				= 400160,
	};

	char* m_MuzzleParticle;

	char* FNAME;

	int	  m_isConsumable; // for usableitems
	int	  m_ShopStackSize;

	char* m_ModelPath_1st;
	char* m_AnimPrefix; // used for FPS mode, for grips


	class Ammo*	m_PrimaryAmmo;

	float m_AmmoMass;
	float m_AmmoSpeed;
	float m_AmmoDamage;
	float m_AmmoDecay;
	float m_AmmoArea;
	float m_AmmoDelay;
	float m_AmmoTimeout;
	bool  m_AmmoImmediate;

	float m_reloadActiveTick; // when active reload becomes available. Duration is not longer than 10% of reload time or 0.4sec maximum; for grenades used as a time when grenade should be launched from a hand
	r3dSec_type<float, 0xAB434A6C> m_spread;  // spread set as diameter at 50meter range

	WeaponAnimTypeEnum m_AnimType;
	r3dSec_type<float, 0xC2A9FAC3> m_recoil;
	uint32_t m_fireModeAvailable; // flags
	r3dSec_type<float, 0xAF1DA2F9> m_fireDelay;
	const struct ScopeConfig* m_scopeConfig;
	float						m_scopeZoom; // 0..1; 0 - no zoom. 1 - maximum zoom
	r3dSec_type<float, 0xAC56F451> m_reloadTime;

	mutable r3dPoint3D	muzzlerOffset;
	mutable bool		muzzleOffsetDetected;

	r3dPoint3D adjMuzzlerOffset; // used privately, do not use it
	mutable r3dPoint3D shellOffset; // where shells are coming out

	int		m_sndReloadID;

	int		m_sndFireID_single;
	int		m_sndFireID_auto;
	int		m_sndFireID_single_player; // for local player
	int		m_sndFireID_auto_player; // for local player
	
	int*		m_animationIds;
	int*		m_animationIds_FPS;
	
	// fps item attachments
	int		IsFPS;
	int		FPSSpecID[WPN_ATTM_MAX];	// m_specID in WeaponAttachmentConfig for each attachment slot
	int		FPSDefaultID[WPN_ATTM_MAX];	// default attachment item ids in each slot

	// durability & repairing
	float	m_DurabilityUse;	// per use, in %
	float	m_RepairAmount;
	float	m_PremRepairAmount;
	int	m_RepairPriceGD;

  private:	
	// make copy constructor and assignment operator inaccessible
	WeaponConfig(const WeaponConfig& rhs);
	WeaponConfig& operator=(const WeaponConfig& rhs);

  public:
	WeaponConfig(uint32_t id) : ModelItemConfig(id)
	{
		m_ModelRefCount = 0 ;

		muzzleOffsetDetected = false ;

		m_Model_FPS = NULL;
		m_Model_FPS_Skeleton = NULL;
		m_AnimPool_FPS = NULL;
		m_MuzzleParticle = NULL;
		FNAME = NULL;
		m_ModelPath_1st = NULL;
		m_PrimaryAmmo = NULL;
		m_scopeConfig = NULL;

		m_AnimPrefix = NULL;

		m_isConsumable = 0;
		m_ShopStackSize = 1;

		m_AmmoMass			= 0.1f;
		m_AmmoSpeed			= 100.f;
		m_AmmoDamage		= 1.0f;
		m_AmmoDecay			= 0.1f;
		m_AmmoArea			= 0.1f;
		m_AmmoDelay			= 0.f;
		m_AmmoTimeout		= 1.0f;
		m_AmmoImmediate		= true;

		m_reloadTime		= 1.0f;
		m_reloadActiveTick	= 0.f; // when active reload becomes available. Duration is not longer than 10% of reload time or 0.4sec maximum; for grenades used as a time when grenade should be launched from a hand
		m_fireDelay			= 0.5f;
		m_spread			= 0.01f; 
		m_recoil			= 0.1f;
		m_AnimType			= WPN_ANIM_ASSAULT;
		m_fireModeAvailable	= WPN_FRM_SINGLE; // flags
		m_scopeZoom			= 0; // 0..1; 0 - no zoom. 1 - maximum zoom

		muzzlerOffset		= r3dPoint3D( 0.25f, 0.f, 0.f );
		adjMuzzlerOffset	= muzzlerOffset ; // used privately, do not use it
		shellOffset			= r3dPoint3D( 0, 0, 0 ); // where shells are coming out

		m_sndReloadID		= -1 ;

		m_sndFireID_single = -1;
		m_sndFireID_auto   = -1;
		m_sndFireID_single_player = -1;
		m_sndFireID_auto_player = -1;
		
		m_animationIds          = NULL;
		m_animationIds_FPS		= NULL;
		
		IsFPS = 0;
		memset(FPSSpecID, 0, sizeof(FPSSpecID));
		memset(FPSDefaultID, 0, sizeof(FPSDefaultID));

		m_DurabilityUse = 0.0f;
		m_RepairAmount = 0.0f;
		m_PremRepairAmount = 0.0f;
		m_RepairPriceGD = 0;
	}
	virtual ~WeaponConfig() 
	{
		free(m_MuzzleParticle);
		free(FNAME);
		free(m_ModelPath_1st);
		free(m_AnimPrefix);
		SAFE_DELETE_ARRAY(m_animationIds);
		SAFE_DELETE_ARRAY(m_animationIds_FPS);
	}
	virtual bool loadBaseFromXml(pugi::xml_node& xmlWeapon);

	bool isAttachmentValid(const WeaponAttachmentConfig* wac) const;
	
	__forceinline void copyParametersTo(GBWeaponInfo& wi) const
	{
		#define DO(XX) wi.XX = XX
		DO(m_AmmoMass);
		DO(m_AmmoSpeed);
		DO(m_AmmoDamage);
		DO(m_AmmoDecay);
		DO(m_AmmoArea);
		DO(m_AmmoDelay);
		DO(m_AmmoTimeout);

		DO(m_reloadTime);
		DO(m_reloadActiveTick);
		DO(m_fireDelay);
		DO(m_spread); 
		DO(m_recoil);

		DO(m_DurabilityUse);
		DO(m_RepairAmount);
		DO(m_PremRepairAmount);
		DO(m_RepairPriceGD);
		#undef DO
	}
	__forceinline void copyParametersFrom(const GBWeaponInfo& wi)
	{
		#define DO(XX) XX = wi.XX
		DO(m_AmmoMass);
		DO(m_AmmoSpeed);
		DO(m_AmmoDamage);
		DO(m_AmmoDecay);
		DO(m_AmmoArea);
		DO(m_AmmoDelay);
		DO(m_AmmoTimeout);

		DO(m_reloadTime);
		DO(m_reloadActiveTick);
		DO(m_fireDelay);
		DO(m_spread); 
		DO(m_recoil);

		DO(m_DurabilityUse);
		DO(m_RepairAmount);
		DO(m_PremRepairAmount);
		DO(m_RepairPriceGD);
		#undef DO
	}

	__forceinline DWORD GetClientParametersHash() const
	{
		//VMPROTECT_BeginVirtualization("GetClientParametersHash");
		// hold copy of variables to hash, work with r3dSecType
#pragma pack(push,1)
		struct hash_s 
		{
			float m_spread;
			float m_fireDelay;
			float m_recoil;
			float m_AmmoSpeed;
			float m_reloadTime;
		};
#pragma pack(pop)

		hash_s h;
		h.m_reloadTime = m_reloadTime;
		h.m_fireDelay  = m_fireDelay;
		h.m_spread     = m_spread;
		h.m_recoil     = m_recoil;
		h.m_AmmoSpeed  = m_AmmoSpeed;
		DWORD crc32 = r3dCRC32((BYTE*)&h, sizeof(h));
		//VMPROTECT_End();
		return crc32;
	}

	r3dMesh* getMesh( bool allow_async_loading, bool first_person ) const;

	int getConfigMeshRefs() const ;

	// called when unloading level
	virtual void resetMesh() { 
		ModelItemConfig::resetMesh();
		m_Model_FPS = 0; 
		SAFE_DELETE(m_Model_FPS_Skeleton); 
		SAFE_DELETE(m_AnimPool_FPS); 
	}
	void detectMuzzleOffset(bool first_person) const;

	// because mesh can be delay-loaded
	void updateMuzzleOffset(bool first_person) const ;

	r3dSkeleton* getSkeleton() const { return m_Model_FPS_Skeleton; }

	void aquireMesh( bool allow_async_loading ) const ;
	void releaseMesh() const ;

	R3D_FORCEINLINE bool hasFPSModel() const
	{
		return m_Model_FPS ? true : false ;
	}

	R3D_FORCEINLINE bool isFPSModelSkeletal() const
	{
		return m_Model_FPS->IsSkeletal() ? true : false ;
	}
};

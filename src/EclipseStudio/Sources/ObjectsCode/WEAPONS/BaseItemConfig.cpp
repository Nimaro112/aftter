#include "r3dPCH.h"
#include "r3d.h"

#include "BaseItemConfig.h"
#include "LangMngr.h"

bool BaseItemConfig::loadBaseFromXml(pugi::xml_node& xmlItem)
{
	r3d_assert(m_Description==NULL);
	r3d_assert(m_StoreIcon==NULL);
	r3d_assert(m_StoreName==NULL);

	category = (STORE_CATEGORIES)xmlItem.attribute("category").as_int();

	m_Weight = xmlItem.attribute("Weight").as_float()/1000.0f; // convert from grams into kg

	m_ModelFile = strdup(xmlItem.child("Model").attribute("file").value());
	m_StoreIcon = strdup(xmlItem.child("Store").attribute("icon").value());
	m_LevelRequired = xmlItem.child("Store").attribute("LevelRequired").as_int();
	
	m_ResWood  = xmlItem.child("Resource").attribute("Wood").as_int();
	m_ResStone = xmlItem.child("Resource").attribute("Stone").as_int();
	m_ResMetal = xmlItem.child("Resource").attribute("Metal").as_int();

	bool loadNameFromLangFile = true;
	if(category == storecat_LootBox)
		loadNameFromLangFile = false;

#ifndef WO_SERVER
	if(loadNameFromLangFile)
	{
		char tmpStr[64];
		sprintf(tmpStr, "%d_name", m_itemID);
		m_StoreName = strdup(gLangMngr.getString(tmpStr));
		sprintf(tmpStr, "%d_desc", m_itemID);
		m_Description = strdup(gLangMngr.getString(tmpStr));
	}
	else
#endif
	{
		const char* desc = xmlItem.child("Store").attribute("desc").value();
		r3d_assert(desc);
		m_Description = strdup(desc);
		m_StoreName = strdup(xmlItem.child("Store").attribute("name").value());
	}

	/*FILE* tempFile = fopen_for_write("lang.txt", "ab");
	char tmpStr[2048];
	sprintf(tmpStr, "%d_name=%s\n", m_itemID, m_StoreName);
	fwrite(tmpStr, 1, strlen(tmpStr), tempFile);
	sprintf(tmpStr, "%d_desc=%s\n", m_itemID, m_Description);
	fwrite(tmpStr, 1, strlen(tmpStr), tempFile);
	fclose(tempFile);*/


	return true;
}

bool ModelItemConfig::loadBaseFromXml(pugi::xml_node& xmlItem)
{
	BaseItemConfig::loadBaseFromXml(xmlItem);
	r3d_assert(m_ModelPath==NULL);
	if(!xmlItem.child("Model").attribute("file").empty())
		m_ModelPath = strdup(xmlItem.child("Model").attribute("file").value());

	return true;
}

bool LootBoxConfig::loadBaseFromXml(pugi::xml_node& xmlItem)
{
	BaseItemConfig::loadBaseFromXml(xmlItem);
	return true;
}

bool FoodConfig::loadBaseFromXml(pugi::xml_node& xmlItem)
{
	ModelItemConfig::loadBaseFromXml(xmlItem);
	Health = xmlItem.child("Property").attribute("health").as_float();
	Toxicity = xmlItem.child("Property").attribute("toxicity").as_float();
	Water= xmlItem.child("Property").attribute("thirst").as_float();
	Food = xmlItem.child("Property").attribute("hunger").as_float();
	Stamina = R3D_CLAMP(xmlItem.child("Property").attribute("stamina").as_float()/100.0f, 0.0f, 1.0f);

	m_ShopStackSize = xmlItem.child("Property").attribute("shopSS").as_int();

	return true;
}

bool CraftComponentConfig::loadBaseFromXml(pugi::xml_node& xmlItem)
{
	return ModelItemConfig::loadBaseFromXml(xmlItem);
}

bool SkinRecipeConfig::loadBaseFromXml(pugi::xml_node& xmlItem)
{
	ModelItemConfig::loadBaseFromXml(xmlItem);
	
	switch(m_itemID)
	{
	// FN SCAR SKINS
	case 330004: //FN Scar NightWalker Skin
	{
		ItemSKN = DefaultSkin(101193, 1, m_itemID);
		break;
	}
	case 330005: //FN Scar Green Snake Skin
	{
		ItemSKN = DefaultSkin(101193, 2, m_itemID);
		break;
	}
	case 330006:  //FN Scar Australia Skin
	{
		ItemSKN = DefaultSkin(101193, 3, m_itemID);
		break;
	}
	case 330007:   //FN Scar Elite Skin
	{
		ItemSKN = DefaultSkin(101193, 4, m_itemID);
		break;
	}
	///////
	case 330008:   //FN SCAR W-LTD Skin
	{
		ItemSKN = DefaultSkin(101193, 5, m_itemID);
		break;
	}
	case 330009:   //Reggae FN SCAR Skin
	{
		ItemSKN = DefaultSkin(101193, 6, m_itemID);
		break;
	}
	case 330010:   //FN SCAR Nvidia Skin
	{
		ItemSKN = DefaultSkin(101193, 7, m_itemID);
		break;
	}
	case 330011:   //FN Scar God of Clearview Skin
	{
		ItemSKN = DefaultSkin(101193, 8, m_itemID);
		break;
	}
	case 330012:   //FN SCAR Love DAD Skin
	{
		ItemSKN = DefaultSkin(101193, 9, m_itemID);
		break;
	}
	// AK74M SKINS
	case 330013:   // AK-74M_Elite
	{
		ItemSKN = DefaultSkin(101022, 1, m_itemID);
		break;
	}
	// AN 94 SKINS
	case 330014:   // Kronos AN94
	{
		ItemSKN = DefaultSkin(101029, 1, m_itemID);
		break;
	}
	case 330015:   // Logitech AN 94
	{
		ItemSKN = DefaultSkin(101029, 2, m_itemID);
		break;
	}
	// ANACONDA SKINS
	case 330016:   // Anaconda Intel
	{
		ItemSKN = DefaultSkin(101331, 1, m_itemID);
		break;
	}
	case 330017:   // Tiger Colt Anaconda
	{
		ItemSKN = DefaultSkin(101331, 2, m_itemID);
		break;
	}
	// L85 ASR SKINS
	case 330018:   // God Ares L85
	{
		ItemSKN = DefaultSkin(101027, 1, m_itemID);
		break;
	}
	case 330019:   // Tt eSPORTS L85
	{
		ItemSKN = DefaultSkin(101027, 2, m_itemID);
		break;
	}
	case 330020:   // L85 NVIDIA
	{
		ItemSKN = DefaultSkin(101027, 3, m_itemID);
		break;
	}
	// TAR21 ASR SKINS
	case 330021:   // [+1] IMI TAR-21
	{
		ItemSKN = DefaultSkin(101173, 1, m_itemID);
		break;
	}
	case 330022:   // Red Dragon IMI TAR-21
	{
		ItemSKN = DefaultSkin(101173, 2, m_itemID);
		break;
	}
	case 330023:   // IMI TAR-21 Nvidia Custom
	{
		ItemSKN = DefaultSkin(101173, 3, m_itemID);
		break;
	}
	case 330024:   // Norad IMI TAR-21
	{
		ItemSKN = DefaultSkin(101173, 4, m_itemID);
		break;
	}
	// AR HK G3 SKINS
	case 330025:   // God Poseidon HK G3
	{
		ItemSKN = DefaultSkin(101214, 1, m_itemID);
		break;
	}
	case 330026:   // God Hades HK G3
	{
		ItemSKN = DefaultSkin(101214, 2, m_itemID);
		break;
	}
	case 330027:   // God Ares HK G3
	{
		ItemSKN = DefaultSkin(101214, 3, m_itemID);
		break;
	}
	case 330028:   // God Athena HK G3
	{
		ItemSKN = DefaultSkin(101214, 4, m_itemID);
		break;
	}
	case 330029:   // Kronos HK G3
	{
		ItemSKN = DefaultSkin(101214, 5, m_itemID);
		break;
	}
	// Spike Bat SKINS
	case 330030:   // Death Squad Bat
	{
		ItemSKN = DefaultSkin(101313, 1, m_itemID);
		break;
	}
	// BERRETA ARX SKINS
	case 330031:   // AR 2600 NVIDIA
	{
		ItemSKN = DefaultSkin(101028, 1, m_itemID);
		break;
	}
	case 330032:   // Airport Legend AR 2600
	{
		ItemSKN = DefaultSkin(101028, 2, m_itemID);
		break;
	}
	case 330033:   // God of Clearview AR 2600
	{
		ItemSKN = DefaultSkin(101028, 3, m_itemID);
		break;
	}
	case 330034:   // St. Frosty Pines AR 2600
	{
		ItemSKN = DefaultSkin(101028, 4, m_itemID);
		break;
	}
	case 330035:   // Norads Hero AR 2600
	{
		ItemSKN = DefaultSkin(101028, 5, m_itemID);
		break;
	}
	case 330036:   // Rocky Fords Guardian AR 2600
	{
		ItemSKN = DefaultSkin(101028, 6, m_itemID);
		break;
	}
	// BIZON SMG SKINS
	case 330037:  // BIZON SMG ELITE
	{
		ItemSKN = DefaultSkin(101109, 1, m_itemID);
		break;
	}
	// CHAINSAW SKINS
	case 330039:  // SNOW WOLF CHAINSAW
	{
		ItemSKN = DefaultSkin(101347, 1, m_itemID);
		break;
	}
	// CROSSBOW SKINS
	case 330040:  // SNP CROSSBOW AIRPORT
	{
		ItemSKN = DefaultSkin(101322, 1, m_itemID);
		break;
	}
	case 330041:  // SNP CROSSBOW VAL
	{
		ItemSKN = DefaultSkin(101322, 2, m_itemID);
		break;
	}
	case 330042:  // SNP CROSSBOW TIGER
	{
		ItemSKN = DefaultSkin(101322, 3, m_itemID);
		break;
	}
	// DESERT EAGLE SKINS
	case 330043:  // DESERT EAGLE ELITE
	{
		ItemSKN = DefaultSkin(101180, 1, m_itemID);
		break;
	}
	case 330044:  // SNOW WOLF DESERT EAGLE
	{
		ItemSKN = DefaultSkin(101180, 2, m_itemID);
		break;
	}
	// FAMAS SKINS
	case 330045:  // FAMAS GOD POSEIDON
	{
		ItemSKN = DefaultSkin(101011, 1, m_itemID);
		break;
	}
	case 330046:  // FAMAS LOGITECH
	{
		ItemSKN = DefaultSkin(101011, 2, m_itemID);
		break;
	}
	case 330047:  // FAMAS SPORT TT
	{
		ItemSKN = DefaultSkin(101011, 3, m_itemID);
		break;
	}
	case 330048:  // FAMAS TIGER
	{
		ItemSKN = DefaultSkin(101011, 4, m_itemID);
		break;
	}
	// FLASHLIGHT SKINS
	case 330049:  // Flashlight Burning
	{
		ItemSKN = DefaultSkin(101306, 1, m_itemID);
		break;
	}
	// G3 SKINS
	case 330050:  // G3 ELITE
	{
		ItemSKN = DefaultSkin(101005, 1, m_itemID);
		break;
	}
	// HARDCORE GUITTAR SKINS
	case 330051:  // Hardcore Guitar Black
	{
		ItemSKN = DefaultSkin(101110, 1, m_itemID);
		break;
	}
	// HG SIG SAUER SKINS
	case 330052:  // Reggae Sig Sauer P226
	{
		ItemSKN = DefaultSkin(101120, 1, m_itemID);
		break;
	}
	// HONEY BADGER SKINS
	case 330053:  // Goddess Athena HONEY BADGER
	{
		ItemSKN = DefaultSkin(101106, 1, m_itemID);
		break;
	}
	case 330054:  // Marine Force Honey Badger
	{
		ItemSKN = DefaultSkin(101106, 2, m_itemID);
		break;
	}
	// HG BERETTA 93R SKINS
	case 330055:  // Marine Force B93R
	{
		ItemSKN = DefaultSkin(101112, 1, m_itemID);
		break;
	}
	case 330056:  // Airport Legend B93R
	{
		ItemSKN = DefaultSkin(101112, 2, m_itemID);
		break;
	}
	case 330057:  // God of Clearview B93R
	{
		ItemSKN = DefaultSkin(101112, 3, m_itemID);
		break;
	}
	case 330058:  // St. Frosty Pines B93R
	{
		ItemSKN = DefaultSkin(101112, 4, m_itemID);
		break;
	}
	case 330059:  // Norads Hero B93R
	{
		ItemSKN = DefaultSkin(101112, 5, m_itemID);
		break;
	}
	case 330060:  // Rocky Fords Guardian B93R
	{
		ItemSKN = DefaultSkin(101112, 6, m_itemID);
		break;
	}
	case 330061:  // Galaxy B93R
	{
		ItemSKN = DefaultSkin(101112, 7, m_itemID);
		break;
	}
	// KATANA SKINS
	case 330062:  // Marine Force Katana
	{
		ItemSKN = DefaultSkin(101336, 1, m_itemID);
		break;
	}
	case 330063:  // Airport Legend Katana
	{
		ItemSKN = DefaultSkin(101336, 2, m_itemID);
		break;
	}
	case 330064:  // God of Clearview Katana
	{
		ItemSKN = DefaultSkin(101336, 3, m_itemID);
		break;
	}
	case 330065:  // St. Frosty Pines Katana
	{
		ItemSKN = DefaultSkin(101336, 4, m_itemID);
		break;
	}
	case 330066:  // Norads Hero Katana
	{
		ItemSKN = DefaultSkin(101336, 5, m_itemID);
		break;
	}
	case 330067:  // Rocky Fords Guardian Katana
	{
		ItemSKN = DefaultSkin(101336, 6, m_itemID);
		break;
	}
	case 330068:  // Jokoto Burning
	{
		ItemSKN = DefaultSkin(101336, 7, m_itemID);
		break;
	}
	// KRISS VECTOR SKINS
	case 330069:  // God Hades Kriss SV
	{
		ItemSKN = DefaultSkin(100288, 1, m_itemID);
		break;
	}
	case 330070:  // Corsair Kriss-SV
	{
		ItemSKN = DefaultSkin(100288, 2, m_itemID);
		break;
	}
	case 330071:  // Death Squad Kriss-SV
	{
		ItemSKN = DefaultSkin(100288, 3, m_itemID);
		break;
	}
	// M16 SKINS
	case 330072:  // Castle Runner M16
	{
		ItemSKN = DefaultSkin(101002, 1, m_itemID);
		break;
	}
	// SNP VINTOREZ SKINS
	case 330073:  // Air Pay VSS VINTOREZ
	{
		ItemSKN = DefaultSkin(101084, 1, m_itemID);
		break;
	}
	case 330074:  // VSS Let's Comic
	{
		ItemSKN = DefaultSkin(101084, 2, m_itemID);
		break;
	}
	case 330075:  // Galax VSS VINTOREZ
	{
		ItemSKN = DefaultSkin(101084, 3, m_itemID);
		break;
	}
	case 330076:  // VSS DEATHBRINGER
	{
		ItemSKN = DefaultSkin(101084, 4, m_itemID);
		break;
	}
	case 330077:  // VSS VINTOREZ Corsair
	{
		ItemSKN = DefaultSkin(101084, 5, m_itemID);
		break;
	}
	case 330078:  // Gigabyte VSS VINTOREZ
	{
		ItemSKN = DefaultSkin(101084, 6, m_itemID);
		break;
	}
	// SNP SVD SKINS
	case 330079:  // SVD MK2
	{
		ItemSKN = DefaultSkin(101068, 1, m_itemID);
		break;
	}
	case 330080:  // SVD MK.2 Electron
	{
		ItemSKN = DefaultSkin(101068, 2, m_itemID);
		break;
	}
	case 330081:  // SVD MK.2 Phoenix
	{
		ItemSKN = DefaultSkin(101068, 3, m_itemID);
		break;
	}
	case 330082:  // SVD MK.2 Dragon
	{
		ItemSKN = DefaultSkin(101068, 4, m_itemID);
		break;
	}
	case 330083:  // SVD MK.2 Griffon
	{
		ItemSKN = DefaultSkin(101068, 5, m_itemID);
		break;
	}
	// MASADA SKINS
	case 330084:  // Scream MASADA
	{
		ItemSKN = DefaultSkin(101169, 1, m_itemID);
		break;
	}
	case 330085:  // Dragon MASADA
	{
		ItemSKN = DefaultSkin(101169, 2, m_itemID);
		break;
	}
	// P90 SKINS
	case 330086:  // P90 K.6 Edition
	{
		ItemSKN = DefaultSkin(101107, 1, m_itemID);
		break;
	}
	case 330087:  // Griffon FN P90
	{
		ItemSKN = DefaultSkin(101107, 2, m_itemID);
		break;
	}
	// SIG SAUER SKINS
	case 330088:  // [+1] SIG SAUER 556
	{
		ItemSKN = DefaultSkin(101172, 1, m_itemID);
		break;
	}
	case 330089:  // SIG SAUER 556 TM EDITION
	{
		ItemSKN = DefaultSkin(101172, 2, m_itemID);
		break;
	}
	case 330090:  // Rocky Fords Guardian SIG SAUER 556
	{
		ItemSKN = DefaultSkin(101172, 3, m_itemID);
		break;
	}
	case 330091:  // Infest 1st SIG SAUER
	{
		ItemSKN = DefaultSkin(101172, 4, m_itemID);
		break;
	}
	// MOSSBERG 590 SKINS
	case 330092:  // MOSSBERG 590 Elite
	{
		ItemSKN = DefaultSkin(101158, 1, m_itemID);
		break;
	}
	// AW Magnum SKINS
	case 330093:  // AW Magnum NVIDIA
	{
		ItemSKN = DefaultSkin(101087, 1, m_itemID);
		break;
	}
	case 330094:  // AW Magnum TM EDITION
	{
		ItemSKN = DefaultSkin(101087, 2, m_itemID);
		break;
	}
	case 330095:  // AW Magnum Pandora
	{
		ItemSKN = DefaultSkin(101087, 3, m_itemID);
		break;
	}
	// SAIGA SHG SKINS
	case 330096:  // Halloween SAIGA
	{
		ItemSKN = DefaultSkin(101098, 1, m_itemID);
		break;
	}
	// KRUGER ASR SKINS
	case 330097:  // Killer Kruger .22 Rifle
	{
		ItemSKN = DefaultSkin(101330, 1, m_itemID);
		break;
	}
	// MAUSER SP66 SKINS
	case 330098:  // MAUSER SP66 True Money
	{
		ItemSKN = DefaultSkin(101085, 1, m_itemID);
		break;
	}
	case 330099:  // MAUSER SP66_Elite
	{
		ItemSKN = DefaultSkin(101085, 2, m_itemID);
		break;
	}
	case 330100:  // TT SPORTS MAUSER SP66
	{
		ItemSKN = DefaultSkin(101085, 3, m_itemID);
		break;
	}
	case 330101:  // Romance MAUSER SP66
	{
		ItemSKN = DefaultSkin(101085, 4, m_itemID);
		break;
	}
	// SNP BLASER R93 SKINS
	case 330102:  // Galaxy BLASER R93
	{
		ItemSKN = DefaultSkin(101247, 1, m_itemID);
		break;
	}
	
	default:
		r3d_assert(false && "Unsupported Skin recipe!!!");
		break;
	}
  
	return true;
}
bool CraftRecipeConfig::loadBaseFromXml(pugi::xml_node& xmlItem)
{
	ModelItemConfig::loadBaseFromXml(xmlItem);

	// hard coded for now
	switch(m_itemID)
	{
	/////// ROTB COMPONENTS
	case 301340: // Block small
		{
			components[numComponents++] = CraftComponent(301387, 60); // Stone
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 103000;
		}
		break;
	case 301341: // Block big
		{
			components[numComponents++] = CraftComponent(301387, 120); // Stone
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 103001;
		}
		break;
	case 301342: // Block cilinder
		{
			components[numComponents++] = CraftComponent(301387, 115); // Stone
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 103002;
		}
		break;
	case 301343: // Column Block
		{
			components[numComponents++] = CraftComponent(301387, 110); // Stone
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 103003;
		}
		break;
	case 301344: // concrete wall
		{
			components[numComponents++] = CraftComponent(301387, 140); // Stone
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 103004;
		}
		break;
	case 301345: // bow cement
		{
			components[numComponents++] = CraftComponent(301387, 130); // Stone
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 103005; 
		}
		break;
	case 301346: // Medium floor
		{
			components[numComponents++] = CraftComponent(301387, 140); // Stone
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 103006;
		}
		break;
	case 301347: // Small floor
		{
			components[numComponents++] = CraftComponent(301387, 120); // Stone
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 103007;
		}
		break;
	case 301348: // big ceiling
		{
			components[numComponents++] = CraftComponent(301387, 128); // Stone
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 103008;
		}
		break;
	case 301349: // small ceiling
		{
			components[numComponents++] = CraftComponent(301387, 185); // Stone
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 103009;
		}
		break;
	case 301350: // medium ceiling
		{
			components[numComponents++] = CraftComponent(301387, 105); // Stone
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 103010;
		}
		break;
	case 301351: // Metal Fence 
		{
			components[numComponents++] = CraftComponent(301386, 60); // Metal
			components[numComponents++] = CraftComponent(311450, 8); // Metal Pipe
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 103011;
		}
		break;
	case 301352: // Slope concrete
		{
			components[numComponents++] = CraftComponent(301387, 115); // Stone
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 103012;
		}
		break;
	case 301353: // Wall concrete
		{
			components[numComponents++] = CraftComponent(301387, 120); // Stone
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 103013;
		}
		break;
	case 301354: // Wall concrete with window
		{
			components[numComponents++] = CraftComponent(301387, 125); // Stone
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 103014;
		}
		break;
	case 301355: // Wall concrete with door
		{
			components[numComponents++] = CraftComponent(301387, 120); // Stone
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 103015;
		}
		break;
	case 301356: // Wall medium
		{
			components[numComponents++] = CraftComponent(301387, 60); // Stone
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 103016;
		}
		break;
	case 301357: // Wall small
		{
			components[numComponents++] = CraftComponent(301387, 120); // Stone
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 103017;
		}
		break;
	case 301358: // Base Bunker
		{
			components[numComponents++] = CraftComponent(301387, 310); // Stone
			components[numComponents++] = CraftComponent(311450, 15); // Metal Pipe
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 103019;
		}
		break;
	//////// INB COMPONENTS
	case 301359: // Container01
		{
			components[numComponents++] = CraftComponent(301387, 240); // Stone
			components[numComponents++] = CraftComponent(301386, 220); // Metal
			components[numComponents++] = CraftComponent(101399, 3); // Tool Kit
			craftedItemID = 104000;
		}
		break;
	case 301360: // Container02
		{
			components[numComponents++] = CraftComponent(301387, 240); // Stone
			components[numComponents++] = CraftComponent(301386, 220); // Metal
			components[numComponents++] = CraftComponent(101399, 3); // Tool Kit
			craftedItemID = 104001;
		}
		break;
	case 301361: // Container03
		{
			components[numComponents++] = CraftComponent(301387, 240); // Stone
			components[numComponents++] = CraftComponent(301386, 220); // Metal
			components[numComponents++] = CraftComponent(101399, 3); // Tool Kit
			craftedItemID = 104002;
		}
		break;
	case 301362: // Container04
		{
			components[numComponents++] = CraftComponent(301387, 320); // Stone
			components[numComponents++] = CraftComponent(301386, 280); // Metal
			components[numComponents++] = CraftComponent(101399, 3); // Tool Kit
			craftedItemID = 104003;
		}
		break;
	case 301363: // Container_Stairs_01
		{
			components[numComponents++] = CraftComponent(301386, 220); // Metal
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 104004;
		}
		break;
	case 301364: // basebuilding_Wall_01
		{
			components[numComponents++] = CraftComponent(301386, 130); // Metal
			components[numComponents++] = CraftComponent(301388, 115); // Wood
			components[numComponents++] = CraftComponent(311444, 2); // Nails
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 104005;
		}
		break;
	case 301365: // basebuilding_Wall_02
		{
			components[numComponents++] = CraftComponent(301386, 208); // Metal
			components[numComponents++] = CraftComponent(311415, 8); // Bolts
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 104006;
		}
		break;
	case 301366: // basebuilding_Wall_03
		{
			components[numComponents++] = CraftComponent(301386, 208); // Metal
			components[numComponents++] = CraftComponent(103016, 4); // Wall Small
			components[numComponents++] = CraftComponent(311415, 5); // Bolts
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 104007;
		}
		break;
	case 301367: // Greenhouse_01
		{
			components[numComponents++] = CraftComponent(301388, 243); // Wood
			components[numComponents++] = CraftComponent(311444, 12); // Nails
			components[numComponents++] = CraftComponent(301387, 88); // Stone
			components[numComponents++] = CraftComponent(101399, 3); // Tool Kit
			craftedItemID = 104008;
		}
		break;
	case 301368: // Shanty_House
		{
			components[numComponents++] = CraftComponent(301388, 113); // Wood
			components[numComponents++] = CraftComponent(301386, 211); // Metal
			components[numComponents++] = CraftComponent(311444, 8); // Nails
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 104009;
		}
		break;
	case 301369: // WatchTower_Small
		{
			components[numComponents++] = CraftComponent(301388, 164); // Wood
			components[numComponents++] = CraftComponent(301386, 92); // Metal
			components[numComponents++] = CraftComponent(311444, 8); // Nails
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 104010;
		}
		break;
	case 301370: // Catwalks_noramp_01
		{
			components[numComponents++] = CraftComponent(301388, 213); // Wood
			components[numComponents++] = CraftComponent(301386, 92); // Metal
			components[numComponents++] = CraftComponent(311445, 3); // Tuercas
			components[numComponents++] = CraftComponent(311450, 6); // Pipe
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 104011;
		}
		break;
	case 301371: // Catwalks_ramp_01
		{
			components[numComponents++] = CraftComponent(301388, 218); // Wood
			components[numComponents++] = CraftComponent(301386, 112); // Metal
			components[numComponents++] = CraftComponent(311445, 6); // Tuercas
			components[numComponents++] = CraftComponent(311450, 12); // Pipe
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 104012;
		}
		break;
	case 301372: // Foundation_20x20
		{
			components[numComponents++] = CraftComponent(301388, 280); // Wood
			components[numComponents++] = CraftComponent(301386, 310); // Metal
			components[numComponents++] = CraftComponent(103009, 4); // Metal
			components[numComponents++] = CraftComponent(311445, 16); // Tuercas
			components[numComponents++] = CraftComponent(311415, 16); // Bolts
			components[numComponents++] = CraftComponent(101399, 3); // Tool Kit
			craftedItemID = 104013;
		}
		break;
	case 301373: // Foundation_20x40
		{
			components[numComponents++] = CraftComponent(301388, 280); // Wood
			components[numComponents++] = CraftComponent(301386, 310); // Metal
			components[numComponents++] = CraftComponent(103009, 5); // Floor
			components[numComponents++] = CraftComponent(311445, 16); // Tuercas
			components[numComponents++] = CraftComponent(311415, 16); // Bolts
			components[numComponents++] = CraftComponent(101399, 3); // Tool Kit
			craftedItemID = 104014;
		}
		break;
	case 301374: // Foundation_40x40
		{
			components[numComponents++] = CraftComponent(301388, 310); // Wood
			components[numComponents++] = CraftComponent(301386, 343); // Metal
			components[numComponents++] = CraftComponent(103009, 8); // Floor
			components[numComponents++] = CraftComponent(311445, 16); // Tuercas
			components[numComponents++] = CraftComponent(311415, 16); // Bolts
			components[numComponents++] = CraftComponent(101399, 3); // Tool Kit
			craftedItemID = 104015;
		}
		break;
	case 301375: // SafeZone_Walls_01
		{
			components[numComponents++] = CraftComponent(301386, 89); // Metal
			components[numComponents++] = CraftComponent(311444, 1); // Nails
			components[numComponents++] = CraftComponent(311450, 4); // Pipe
			components[numComponents++] = CraftComponent(311443, 4); // Metal_Scrap_04
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 104016;
		}
		break;
	case 301376: // SafeZone_Walls_02
		{
			components[numComponents++] = CraftComponent(301387, 113); // Stone
			components[numComponents++] = CraftComponent(311444, 2); // Nails
			components[numComponents++] = CraftComponent(301388, 93); // Wood
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 104017;
		}
		break;
	case 301377: // SafeZone_Walls_03
		{
			components[numComponents++] = CraftComponent(301387, 133); // Stone
			components[numComponents++] = CraftComponent(311444, 2); // Nails
			components[numComponents++] = CraftComponent(301388, 63); // Wood
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 104018;
		}
		break;
	case 301378: // Shack_Survival_L_01
		{
			components[numComponents++] = CraftComponent(301388, 118); // Wood
			components[numComponents++] = CraftComponent(311444, 2); // Nails
			components[numComponents++] = CraftComponent(311441, 4); // Metal_Scrap_02
			components[numComponents++] = CraftComponent(311420, 3); // Cloth_Piece_02
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 104019;
		}
		break;
	case 301379: // Shack_Survival_S_01
		{
			components[numComponents++] = CraftComponent(301388, 109); // Wood
			components[numComponents++] = CraftComponent(311444, 2); // Nails
			components[numComponents++] = CraftComponent(311441, 3); // Metal_Scrap_02
			components[numComponents++] = CraftComponent(311420, 2); // Cloth_Piece_02
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 104020;
		}
		break;
	case 301380: // Statue_GOLD_01
		{
			components[numComponents++] = CraftComponent(301387, 103); // Stone
			components[numComponents++] = CraftComponent(301386, 120); // Metal
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 104021;
		}
		break;
	case 301381: // Statue_Marble_01
		{
			components[numComponents++] = CraftComponent(301387, 160); // Stone
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 104022;
		}
		break;
	case 301382: // Statue_Metal_01
		{
			components[numComponents++] = CraftComponent(301387, 160); // Stone
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 104023;
		}
		break;
	case 301384: // Tent_Shack_01
		{
			components[numComponents++] = CraftComponent(301386, 115); // Metal
			components[numComponents++] = CraftComponent(311415, 3); // Bolts
			components[numComponents++] = CraftComponent(311420, 4); // Cloth_Piece_02
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 104024;
		}
		break;
		
		//////////////// BARRICADES COMPONENTS - INB
	case 301385: // INB_Barricade_Blockwall_01
		{
			components[numComponents++] = CraftComponent(301387, 97); // Stone
			components[numComponents++] = CraftComponent(311458, 3); // Glue
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 104025;
		}
		break;
	case 302386: // INB_Barricade_Blockwall_DX_01
		{
			components[numComponents++] = CraftComponent(301387, 117); // Stone
			components[numComponents++] = CraftComponent(311458, 4); // Glue
			components[numComponents++] = CraftComponent(311440, 3); // Metal_Scrap_01
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 104026;
		}
		break;
	case 302387: // INB_Barricade_Concrete_01
		{
			components[numComponents++] = CraftComponent(301387, 128); // Stone
			components[numComponents++] = CraftComponent(301386, 18); // Metal
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 104027;
		}
		break;
	case 302388: // INB_Barricade_Concrete_DX_01
		{
			components[numComponents++] = CraftComponent(301387, 138); // Stone
			components[numComponents++] = CraftComponent(301386, 18); // Metal
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 104028;
		}
		break;
	case 301389: // INB_Barricade_Concrete_DX_02
		{
			components[numComponents++] = CraftComponent(301387, 142); // Stone
			components[numComponents++] = CraftComponent(301386, 18); // Metal
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 104029;
		}
		break;
	case 301390: // INB_Barricade_Concrete_DX_03
		{
			components[numComponents++] = CraftComponent(301387, 122); // Stone
			components[numComponents++] = CraftComponent(301386, 18); // Metal
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 104030;
		}
		break;
	case 301391: // INB_Barricade_Concrete_DX_04
		{
			components[numComponents++] = CraftComponent(301387, 128); // Stone
			components[numComponents++] = CraftComponent(301386, 18); // Metal
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 104031;
		}
		break;
	case 301392: // INB_Barricade_Plywood_01
		{
			components[numComponents++] = CraftComponent(301388, 85); // Wood
			components[numComponents++] = CraftComponent(311444, 3); // Nails
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 104032;
		}
		break;
	case 301393: // INB_Barricade_Sandbag_01
		{
			components[numComponents++] = CraftComponent(301387, 105); // Wood
			components[numComponents++] = CraftComponent(311421, 6); // Cloth_Rag
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 104033;
		}
		break;
	case 301394: // INB_Barricade_ScrapMetal_01
		{
			components[numComponents++] = CraftComponent(301386, 98); // Wood
			components[numComponents++] = CraftComponent(311450, 1); // Pipe
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 104034;
		}
		break;
	case 301395: // SS_Barricade_Concrete
		{
			components[numComponents++] = CraftComponent(301387, 137); // Stone
			components[numComponents++] = CraftComponent(301386, 28); // Metal
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 104136;
		}
		break;
	case 301396: // SS_Barricade_MetalWood
		{
			components[numComponents++] = CraftComponent(301388, 102); // Wood
			components[numComponents++] = CraftComponent(301386, 15); // Metal
			components[numComponents++] = CraftComponent(311415, 2); // Bolts
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 104137;
		}
		break;
	case 301397: // SS_Barricade_Shield_Metal
		{
			components[numComponents++] = CraftComponent(301386, 154); // Metal
			components[numComponents++] = CraftComponent(311415, 1); // Bolts
			components[numComponents++] = CraftComponent(311445, 1); // Tuercas
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 104138;
		}
		break;
	case 301398: // SS_Barricade_Shield_Wood
		{
			components[numComponents++] = CraftComponent(301388, 158); // Wood
			components[numComponents++] = CraftComponent(311415, 1); // Bolts
			components[numComponents++] = CraftComponent(311445, 1); // Tuercas
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 104139;
		}
		break;
	case 302399: // SS_Barricade_WoodWall
		{
			components[numComponents++] = CraftComponent(301388, 104); // Wood
			components[numComponents++] = CraftComponent(311444, 2); // Nails
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 104140;
		}
		break;
	case 301400: // SS_Barricade_FenceWall
		{
			components[numComponents++] = CraftComponent(301386, 193); // Metal
			components[numComponents++] = CraftComponent(311415, 1); // Bolts
			components[numComponents++] = CraftComponent(311445, 1); // Tuercas
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 104141;
		}
		break;
		//////// TRAP BARRICADES COMPONENTS - BURSTFIRE
	case 301401: // TS_BarbWire
		{
			components[numComponents++] = CraftComponent(301386, 122); // Metal
			components[numComponents++] = CraftComponent(311461, 2); // Wire
			components[numComponents++] = CraftComponent(311450, 1); // Pipe
			components[numComponents++] = CraftComponent(311408, 8); // Empty Can
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 104036;
		}
		break;
	case 301402: // INB_barricade_BarbWire_01
		{
			components[numComponents++] = CraftComponent(301388, 163); // Wood
			components[numComponents++] = CraftComponent(311461, 1); // Wire
			components[numComponents++] = CraftComponent(311408, 3); // Empty Can
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 105000;
		}
		break;
	case 301403: // INB_barricade_BarbWire_DX_01
		{
			components[numComponents++] = CraftComponent(301388, 186); // Wood
			components[numComponents++] = CraftComponent(311461, 2); // Wire
			components[numComponents++] = CraftComponent(311408, 5); // Empty Can
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 105001;
		}
		break;
	case 301404: // INB_Barricade_WoodSpike_01
		{
			components[numComponents++] = CraftComponent(301388, 133); // Wood
			components[numComponents++] = CraftComponent(311461, 1); // Wire
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 105002;
		}
		break;
	case 301405: // INB_Barricade_WoodSpike_DX_01
		{
			components[numComponents++] = CraftComponent(301388, 162); // Wood
			components[numComponents++] = CraftComponent(311461, 2); // Wire
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 105003;
		}
		break;
	case 301406: // INB_Barricade_SpikeStrip_01
		{
			components[numComponents++] = CraftComponent(301386, 113); // Metal
			components[numComponents++] = CraftComponent(311408, 3); // Can_Empty
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 105004;
		}
		break;
	case 301407: // INB_Traps_Bear_01_Armed
		{
			components[numComponents++] = CraftComponent(301386, 143); // Metal
			components[numComponents++] = CraftComponent(311415, 1); // Bolts
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 105007;
		}
		break;
	case 301408: // INB_Traps_Spikes_01_Armed
		{
			components[numComponents++] = CraftComponent(301386, 176); // Metal
			components[numComponents++] = CraftComponent(311442, 2); // Metal_Scrap_03
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 105008;
		}
		break;
	case 301409: // INB_prop_campfire_01
		{
			components[numComponents++] = CraftComponent(311438, 8); // Log_Small_04
			components[numComponents++] = CraftComponent(311401, 2); // Chemicals
			components[numComponents++] = CraftComponent(311417, 1); // Can_Oil
			craftedItemID = 105006;
		}
		break;
	case 301410: // SS_Barricade_Spike_Mat
		{
			components[numComponents++] = CraftComponent(301386, 46); // Metal
			components[numComponents++] = CraftComponent(311430, 8); // Glass_Cracked_03
			components[numComponents++] = CraftComponent(311411, 1); // Acid
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 105017;
		}
		break;
	case 301411: // SS_Barricade_BarbWire
		{
			components[numComponents++] = CraftComponent(311461, 3); // Wire
			components[numComponents++] = CraftComponent(311408, 8); // Can_Empty
			components[numComponents++] = CraftComponent(311423, 2); // Tape
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 105018;
		}
		break;
	case 301412: // SS_Survival_Spike_Trap
		{
			components[numComponents++] = CraftComponent(301386, 149); // Metal
			components[numComponents++] = CraftComponent(311443, 3); // Metal_Scrap_04
			components[numComponents++] = CraftComponent(311415, 1); // Bolts
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 105019;
		}
		break;
	case 301413: // SS_Barricade_MetalWood_01_Spike
		{
			components[numComponents++] = CraftComponent(301388, 133); // Wood
			components[numComponents++] = CraftComponent(301386, 111); // Metal
			components[numComponents++] = CraftComponent(311441, 4); // Metal_Scrap_02
			components[numComponents++] = CraftComponent(311415, 1); // Bolts
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 105020;
		}
		break;
		///////////// INB HARVEST COMPONENTS
	case 301414: // INB_Garden_Rabbit_01
		{
			components[numComponents++] = CraftComponent(301388, 136); // Wood
			components[numComponents++] = CraftComponent(311444, 2); // Nails
			components[numComponents++] = CraftComponent(311461, 1); // Wire
			components[numComponents++] = CraftComponent(100070, 2); // Lettuce
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 103023;
		}
		break;
	case 301415: // INB_Garden_Chicken_Coop_01
		{
			components[numComponents++] = CraftComponent(301388, 136); // Wood
			components[numComponents++] = CraftComponent(301386, 87); // Metal
			components[numComponents++] = CraftComponent(311444, 2); // Nails
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 111368;
		}
		break;
	case 301416: // INB_Garden_Box_Lettuce
		{
			components[numComponents++] = CraftComponent(301388, 96); // Wood
			components[numComponents++] = CraftComponent(301387, 110); // Stone
			components[numComponents++] = CraftComponent(311444, 2); // Nails
			components[numComponents++] = CraftComponent(311465, 1); // Seed Lettuce
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 105010;
		}
		break;
	case 301417: // INB_Garden_Box_Tomatos
		{
			components[numComponents++] = CraftComponent(301388, 96); // Wood
			components[numComponents++] = CraftComponent(301387, 110); // Stone
			components[numComponents++] = CraftComponent(311444, 2); // Nails
			components[numComponents++] = CraftComponent(311467, 1); // Seed Tomatos
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 111363;
		}
		break;
	case 301418: // INB_Garden_Box_Beets
		{
			components[numComponents++] = CraftComponent(301388, 96); // Wood
			components[numComponents++] = CraftComponent(301387, 110); // Stone
			components[numComponents++] = CraftComponent(311444, 2); // Nails
			components[numComponents++] = CraftComponent(311462, 1); // Seed Beets
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 111364;
		}
		break;
	case 301419: // INB_Garden_Box_Broccoli
		{
			components[numComponents++] = CraftComponent(301388, 96); // Wood
			components[numComponents++] = CraftComponent(301387, 110); // Stone
			components[numComponents++] = CraftComponent(311444, 2); // Nails
			components[numComponents++] = CraftComponent(311463, 1); // Seed Broccoli
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 111365;
		}
		break;
	case 301420: // INB_Garden_Box_Carrots
		{
			components[numComponents++] = CraftComponent(301388, 96); // Wood
			components[numComponents++] = CraftComponent(301387, 110); // Stone
			components[numComponents++] = CraftComponent(311444, 2); // Nails
			components[numComponents++] = CraftComponent(311464, 1); // Seed Carrots
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 111366;
		}
		break;
	case 301421: // INB_Garden_Box_Potatos
		{
			components[numComponents++] = CraftComponent(301388, 96); // Wood
			components[numComponents++] = CraftComponent(301387, 110); // Stone
			components[numComponents++] = CraftComponent(311444, 2); // Nails
			components[numComponents++] = CraftComponent(311466, 1); // Seed Potatos
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 111367;
		}
		break;
	case 301422: // INB_Well_Clean_01
		{
			components[numComponents++] = CraftComponent(301388, 107); // Metal
			components[numComponents++] = CraftComponent(311403, 3); // Glass Cubes
			components[numComponents++] = CraftComponent(311415, 1); // Bolts
			components[numComponents++] = CraftComponent(311449, 1); // Cups
			components[numComponents++] = CraftComponent(311468, 2); // WaterTablet
			components[numComponents++] = CraftComponent(101399, 1); // Tool Kit
			craftedItemID = 101360;
		}
		break;
	////////// MEDICAL COMPONENTS
	case 301423: // Bandages
		{
			components[numComponents++] = CraftComponent(311402, 1); // Cloth
			components[numComponents++] = CraftComponent(311454, 1); // Salt
			components[numComponents++] = CraftComponent(311455, 1); // Soap / Jabon
			craftedItemID = 101261;
		}
		break;
	case 301424: // INB_Med_Homebrew_Bandage
		{
			components[numComponents++] = CraftComponent(311402, 1); // Cloth
			components[numComponents++] = CraftComponent(311454, 1); // Salt
			components[numComponents++] = CraftComponent(311455, 1); // Soap / Jabon
			components[numComponents++] = CraftComponent(311423, 1); // Tape
			craftedItemID = 101262;
		}
		break;
		
	/////////////// EXPLOSIVES
	case 301425: // INB_EXP_Grenade_HB_01
		{
			components[numComponents++] = CraftComponent(311410, 1); // Empty Soda
			components[numComponents++] = CraftComponent(311401, 1); // Chemical
			components[numComponents++] = CraftComponent(301321, 1); // Gasoline
			components[numComponents++] = CraftComponent(311423, 1); // Tape
			craftedItemID = 101405;
		}
		break;
	default:
		r3d_assert(false && "Unsupported craft recipe!!!");
		break;
	}

	return true;
}

bool VehicleInfoConfig::loadBaseFromXml(pugi::xml_node& xmlItem)
{
	BaseItemConfig::loadBaseFromXml(xmlItem);
	
	m_FNAME = strdup(xmlItem.child("Property").attribute("fname").value());

	//r3dOutToLog("########## !!! Properties: Name:%s, weight:%.4f\n", m_FNAME, m_Weight);

	return true;
}

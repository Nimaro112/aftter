#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "obj_StoreNPC.h"

#include "ObjectsCode/weapons/WeaponArmory.h"

IMPLEMENT_CLASS(obj_StoreNPC, "obj_StoreNPC", "Object");
AUTOREGISTER_CLASS(obj_StoreNPC);

obj_StoreNPC::obj_StoreNPC()
{
	m_ActionUI_Title = gLangMngr.getString("$FR_Store");
	m_ActionUI_Msg = gLangMngr.getString("HoldEToAccessRepairBench");
}

obj_StoreNPC::~obj_StoreNPC()
{
}

BOOL obj_StoreNPC::Load(const char *fname)
{
	const char* cpMeshName = "Data\\ObjectsDepot\\INB_Gameplay\\inb_terminal_store_01.sco";
	if(!parent::Load(cpMeshName)) 
		return FALSE;

	return TRUE;
}

BOOL obj_StoreNPC::OnCreate()
{
	m_spawnPos = GetPosition();

	return parent::OnCreate();
}

BOOL obj_StoreNPC::OnDestroy()
{
	return parent::OnDestroy();
}

BOOL obj_StoreNPC::Update()
{
	return parent::Update();
}

void obj_StoreNPC::ReadSerializedData(pugi::xml_node& node)
{
	parent::ReadSerializedData(node);
//	pugi::xml_node dataNode = node.child("RepairBench");
}

void obj_StoreNPC::WriteSerializedData(pugi::xml_node& node)
{
	parent::WriteSerializedData(node);
//	pugi::xml_node dataNode = node.append_child();
//	dataNode.set_name("RepairBench");
}

#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "obj_VaultNPC.h"

#include "ObjectsCode/weapons/WeaponArmory.h"

IMPLEMENT_CLASS(obj_VaultNPC, "obj_VaultNPC", "Object");
AUTOREGISTER_CLASS(obj_VaultNPC);

obj_VaultNPC::obj_VaultNPC()
{
	m_ActionUI_Title = gLangMngr.getString("$FR_Vault");
	m_ActionUI_Msg = gLangMngr.getString("HoldEToAccessRepairBench");
}

obj_VaultNPC::~obj_VaultNPC()
{
}

BOOL obj_VaultNPC::Load(const char *fname)
{
	const char* cpMeshName = "Data\\ObjectsDepot\\INB_Gameplay\\inb_terminal_storage_01.sco";
	if(!parent::Load(cpMeshName)) 
		return FALSE;

	return TRUE;
}

BOOL obj_VaultNPC::OnCreate()
{
	m_spawnPos = GetPosition();

	return parent::OnCreate();
}

BOOL obj_VaultNPC::OnDestroy()
{
	return parent::OnDestroy();
}

BOOL obj_VaultNPC::Update()
{
	return parent::Update();
}

void obj_VaultNPC::ReadSerializedData(pugi::xml_node& node)
{
	parent::ReadSerializedData(node);
//	pugi::xml_node dataNode = node.child("RepairBench");
}

void obj_VaultNPC::WriteSerializedData(pugi::xml_node& node)
{
	parent::WriteSerializedData(node);
//	pugi::xml_node dataNode = node.append_child();
//	dataNode.set_name("RepairBench");
}

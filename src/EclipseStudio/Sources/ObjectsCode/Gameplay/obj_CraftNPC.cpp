#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "obj_CraftNPC.h"

#include "ObjectsCode/weapons/WeaponArmory.h"

IMPLEMENT_CLASS(obj_CraftNPC, "obj_CraftNPC", "Object");
AUTOREGISTER_CLASS(obj_CraftNPC);

obj_CraftNPC::obj_CraftNPC()
{
	m_ActionUI_Title = gLangMngr.getString("ActionUI_CraftTitle");
	m_ActionUI_Msg = gLangMngr.getString("HoldEToAccessCraftBench");
}

obj_CraftNPC::~obj_CraftNPC()
{
}

BOOL obj_CraftNPC::Load(const char *fname)
{
	const char* cpMeshName = "Data\\ObjectsDepot\\SS_Gameplay\\Crafting_WorkBench_01.sco";
	if(!parent::Load(cpMeshName)) 
		return FALSE;

	return TRUE;
}

BOOL obj_CraftNPC::OnCreate()
{
	m_spawnPos = GetPosition();

	return parent::OnCreate();
}

BOOL obj_CraftNPC::OnDestroy()
{
	return parent::OnDestroy();
}

BOOL obj_CraftNPC::Update()
{
	return parent::Update();
}

void obj_CraftNPC::ReadSerializedData(pugi::xml_node& node)
{
	parent::ReadSerializedData(node);
//	pugi::xml_node dataNode = node.child("RepairBench");
}

void obj_CraftNPC::WriteSerializedData(pugi::xml_node& node)
{
	parent::WriteSerializedData(node);
//	pugi::xml_node dataNode = node.append_child();
//	dataNode.set_name("RepairBench");
}

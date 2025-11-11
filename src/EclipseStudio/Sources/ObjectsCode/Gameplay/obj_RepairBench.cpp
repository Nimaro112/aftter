#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "obj_RepairBench.h"

#include "ObjectsCode/weapons/WeaponArmory.h"

IMPLEMENT_CLASS(obj_RepairBench, "obj_RepairBench", "Object");
AUTOREGISTER_CLASS(obj_RepairBench);

obj_RepairBench::obj_RepairBench()
{
	m_ActionUI_Title = gLangMngr.getString("$FR_RepairBench");
	m_ActionUI_Msg = gLangMngr.getString("HoldEToAccessRepairBench");
}

obj_RepairBench::~obj_RepairBench()
{
}

BOOL obj_RepairBench::Load(const char *fname)
{
	const char* cpMeshName = "Data\\ObjectsDepot\\SS_Gameplay\\Crafting_WorkBench_01.sco";
	if(!parent::Load(cpMeshName)) 
		return FALSE;

	return TRUE;
}

BOOL obj_RepairBench::OnCreate()
{
	m_spawnPos = GetPosition();

	return parent::OnCreate();
}

BOOL obj_RepairBench::OnDestroy()
{
	return parent::OnDestroy();
}

BOOL obj_RepairBench::Update()
{
	return parent::Update();
}

void obj_RepairBench::ReadSerializedData(pugi::xml_node& node)
{
	parent::ReadSerializedData(node);
}

void obj_RepairBench::WriteSerializedData(pugi::xml_node& node)
{
	parent::WriteSerializedData(node);
//	pugi::xml_node dataNode = node.append_child();
//	dataNode.set_name("RepairBench");
}

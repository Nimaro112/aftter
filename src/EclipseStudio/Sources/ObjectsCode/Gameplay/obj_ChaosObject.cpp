//=========================================================================
//	Module: obj_ChaosObject.cpp
//	Copyright (C) Owl Production Group Inc. 2018.
//=========================================================================
#include "r3dPCH.h"
#include "r3d.h"

#include "obj_ChaosObject.h"
#include "../../EclipseStudio/Sources/multiplayer/ClientGameLogic.h"
#include "../../EclipseStudio/Sources/ObjectsCode/world/MaterialTypes.h"
#include "../../EclipseStudio/Sources/ObjectsCode/world/DecalChief.h"
#include "../../EclipseStudio/Sources/ObjectsCode/weapons/ExplosionVisualController.h"

extern bool g_bEditMode;

IMPLEMENT_CLASS(obj_ChaosObject, "obj_ChaosObject", "Object");
AUTOREGISTER_CLASS(obj_ChaosObject);


obj_ChaosObject::obj_ChaosObject()
	: m_ChaosEnable( false )
{
	DisablePhysX = false;
	SelectChaosObj = 0;
	RemoveChaosPhyx = false;
	EnableChaosPhyx = false;
	isChaosDestroyed = 0;
	/*OriginalRotation.x = 0;
	OriginalRotation.y = 0;
	OriginalRotation.z = 0;*/
}

obj_ChaosObject::~obj_ChaosObject()
{
}

BOOL obj_ChaosObject::Load(const char *fname)
{
	const char* cpMeshName = "Data\\ObjectsDepot\\G3_Chaos\\Chaos.sco";

	if(!parent::Load(cpMeshName))
		return FALSE;

	return TRUE;
}

BOOL obj_ChaosObject::OnCreate()
{
	m_spawnPos = GetPosition();
	//OpenOrClose = 0.0f;

	if(!g_bEditMode)
	{
		SetNetworkID(gClientLogic().net_lastFreeId++);
	}

	SelectionObj(SelectChaosObj);

	if(DisablePhysX)
	{
		ReadPhysicsConfig();
		PhysicsConfig.group = PHYSCOLL_TINY_GEOMETRY; // skip collision with players
		PhysicsConfig.requireNoBounceMaterial = false;
		PhysicsConfig.isFastMoving = false;
	}
	//SetRotationVector(OriginalRotation);
	return parent::OnCreate();
}

BOOL obj_ChaosObject::OnDestroy()
{
	return parent::OnDestroy();
}

BOOL obj_ChaosObject::Update()
{
	if (RemoveChaosPhyx)
	{
		ReadPhysicsConfig();
		SAFE_DELETE(PhysicsObject);
		ReadPhysicsConfig();
		SetObjFlags(OBJFLAG_SkipDraw | OBJFLAG_DisableShadows); 
		PhysicsConfig.group = PHYSCOLL_TINY_GEOMETRY; // skip collision with players
		PhysicsConfig.requireNoBounceMaterial = false;
		PhysicsConfig.isFastMoving = false; // default value is false
		RemoveChaosPhyx = false;
	}
	if (EnableChaosPhyx)
	{
		ReadPhysicsConfig();
		SAFE_DELETE(PhysicsObject);
		ReadPhysicsConfig();
		ResetObjFlags(OBJFLAG_SkipDraw | OBJFLAG_DisableShadows);
		PhysicsConfig.group = PHYSCOLL_STATIC_GEOMETRY; // skip collision with players
		PhysicsConfig.requireNoBounceMaterial = true;
		PhysicsConfig.isFastMoving = true;
		EnableChaosPhyx=false;
	}

	return parent::Update();
}
void obj_ChaosObject::Set(bool Open)
{
	PKT_S2C_ChaosTrack_s n;
	n.ChaosID = toP2pNetId(GetNetworkID());
	n.m_ChaosEnable = m_ChaosEnable;
	n.isChaosDestroyed = isChaosDestroyed;
	n.SpawnParticles = 0;
	p2pSendToHost(this, &n, sizeof(n));
}

void obj_ChaosObject::SelectionObj(int Selection)
{
	switch(Selection)
	{
		case 0:
			parent::MeshLOD[0] = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_Chaos\\Chaos.sco");
			break;
		case 1:
			parent::MeshLOD[0] = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_Chaos\\Chaos2.sco"); // to be finished
			break;
		case 2:
			parent::MeshLOD[0] = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_Chaos\\Chaos3.sco"); // to be finished
			break;
	}
}

void obj_ChaosObject::ExeParticle()
{
	m_spawnPos = GetPosition();
	
	PKT_S2C_SpawnExplosion_s n;
	// add decal
	DecalParams params;
	params.Dir		= r3dPoint3D(0,1,0);
	params.Pos		= m_spawnPos;
	params.TypeID	= GetDecalID( r3dHash::MakeHash(""), r3dHash::MakeHash("grenade") );
	SpawnImpactParticle(r3dHash::MakeHash(""), r3dHash::MakeHash("ChaosObject"), GetPosition(), r3dPoint3D(0,1,0));
	if( params.TypeID != INVALID_DECAL_ID )
		g_pDecalChief->Add( params );

	//	Start radial blur effect
	gExplosionVisualController.AddExplosion(m_spawnPos, n.radius);
}

void obj_ChaosObject::ReadSerializedData(pugi::xml_node& node)
{
	GameObject::ReadSerializedData(node);
	pugi::xml_node cNode = node.child("Chaos");
	SelectChaosObj = cNode.attribute("Number").as_int();
	pugi::xml_node cNodeRot = node.child("gameObject");
}

void obj_ChaosObject::WriteSerializedData(pugi::xml_node& node)
{
	GameObject::WriteSerializedData(node);
	pugi::xml_node cNode = node.append_child();
	cNode.set_name("Chaos");
	cNode.append_attribute("Number") = SelectChaosObj;
}

GameObject * obj_ChaosObject::Clone()
{
	obj_ChaosObject * newSpawnPoint = static_cast<obj_ChaosObject*>(srv_CreateGameObject("obj_ChaosObject", "Data\\ObjectsDepot\\G3_Chaos\\Chaos.sco", GetPosition()));
	newSpawnPoint->CloneParameters(this);
	return newSpawnPoint;
}

//////////////////////////////////////////////////////////////////////////

void obj_ChaosObject::CloneParameters(obj_ChaosObject *o)
{
	m_spawnPos = o->m_spawnPos;
	SelectionObj(o->SelectChaosObj);
}

#ifndef FINAL_BUILD
float obj_ChaosObject::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	float y = scry + parent::DrawPropertyEditor(scrx, scry, scrw, scrh, startClass, selected );

	if( !IsParentOrEqual( &ClassData, startClass ))
		return y;

	y += 10.0f;
	y += imgui_Static(scrx, y, "Chaos Selection");
	y += imgui_Value_SliderI(scrx, y, "Chaos Number", &SelectChaosObj, 0, 2, "%d");

	SelectionObj(SelectChaosObj);

	return y - scry;
}
#endif
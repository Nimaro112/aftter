//=========================================================================
//	Module: obj_AirBombSpawn.cpp
//	Copyright (C) Online Warmongers Group Inc. 2012.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"

#include "obj_AirBombSpawn.h"
#include "../../XMLHelpers.h"
#include "../../Editors/LevelEditor.h"
#include "../WEAPONS/WeaponArmory.h"
#include "../../../../GameEngine/ai/AI_Brain.h"

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_CLASS(obj_AirBombSpawn, "obj_AirBombSpawn", "Object");
AUTOREGISTER_CLASS(obj_AirBombSpawn);

extern bool g_bEditMode;

//////////////////////////////////////////////////////////////////////////

namespace
{
//////////////////////////////////////////////////////////////////////////

	struct AirBombSpawnCompositeRenderable: public Renderable
	{
		void Init()
		{
			DrawFunc = Draw;
		}

		static void Draw( Renderable* RThis, const r3dCamera& Cam )
		{
			AirBombSpawnCompositeRenderable *This = static_cast<AirBombSpawnCompositeRenderable*>(RThis);

			r3dRenderer->SetRenderingMode(R3D_BLEND_NZ | R3D_BLEND_PUSH);

			r3dDrawLine3D(This->Parent->GetPosition(), This->Parent->GetPosition() + r3dPoint3D(0, 20.0f, 0), Cam, 0.4f, r3dColor24::yellow);
			r3dDrawCircle3D(This->Parent->GetPosition(), This->Parent->spawnRadius, Cam, 0.1f, r3dColor::blue);

			r3dRenderer->Flush();
			r3dRenderer->SetRenderingMode(R3D_BLEND_POP);
		}

		obj_AirBombSpawn *Parent;
	};
}

//////////////////////////////////////////////////////////////////////////

obj_AirBombSpawn::obj_AirBombSpawn()
: spawnRadius(30.0f)
{
	serializeFile = SF_ServerData;
	m_bEnablePhysics = false;
}

//////////////////////////////////////////////////////////////////////////

obj_AirBombSpawn::~obj_AirBombSpawn()
{

}

//////////////////////////////////////////////////////////////////////////

#define RENDERABLE_OBJ_USER_SORT_VALUE (3*RENDERABLE_USER_SORT_VALUE)
void obj_AirBombSpawn::AppendRenderables(RenderArray (& render_arrays  )[ rsCount ], const r3dCamera& Cam)
{
#ifdef FINAL_BUILD
	return;
#else
	if ( !g_bEditMode )
		return;

	if(r_hide_icons->GetInt())
		return;

	float idd = r_icons_draw_distance->GetFloat();
	idd *= idd;

	if( ( Cam - GetPosition() ).LengthSq() > idd )
		return;

	AirBombSpawnCompositeRenderable rend;

	rend.Init();
	rend.Parent		= this;
	rend.SortValue	= RENDERABLE_OBJ_USER_SORT_VALUE;

	render_arrays[ rsDrawDebugData ].PushBack( rend );
#endif
}

//////////////////////////////////////////////////////////////////////////

BOOL obj_AirBombSpawn::OnCreate()
{
	parent::OnCreate();

	DrawOrder = OBJ_DRAWORDER_LAST;

	ObjFlags |= OBJFLAG_DisableShadows;

	r3dBoundBox bboxLocal ;
	bboxLocal.Size = r3dPoint3D(2, 2, 2);
	bboxLocal.Org = -bboxLocal.Size * 0.5f;
	SetBBoxLocal(bboxLocal) ;
	UpdateTransform();

	return 1;
}

//////////////////////////////////////////////////////////////////////////

BOOL obj_AirBombSpawn::Update()
{
	return parent::Update();
}

//////////////////////////////////////////////////////////////////////////

BOOL obj_AirBombSpawn::OnDestroy()
{
	return parent::OnDestroy();
}

//////////////////////////////////////////////////////////////////////////

#ifndef FINAL_BUILD
struct tempS
{
	char* name;
	uint32_t id;
};
static bool SortLootboxesByName(const tempS d1, const tempS d2)
{
	return stricmp(d1.name, d2.name)<0;
}
float obj_AirBombSpawn::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	float y = scry;

	y += parent::DrawPropertyEditor(scrx, scry, scrw, scrh, startClass, selected);
	y += 5.0f;

	y += imgui_Static(scrx, y, "AirBomb parameters:");

	y += imgui_Value_Slider(scrx, y, "Radius", &spawnRadius, 5.0f, 2000.0f, "%0.2f");

	return y - scry;
}
#endif

//////////////////////////////////////////////////////////////////////////

void obj_AirBombSpawn::WriteSerializedData(pugi::xml_node& node)
{
	parent::WriteSerializedData(node);
	pugi::xml_node AirBombSpawnNode = node.append_child();
	AirBombSpawnNode.set_name("LootID_parameters");
	SetXMLVal("spawn_radius", AirBombSpawnNode, &spawnRadius);
}

// NOTE: this function must stay in sync with server version
void obj_AirBombSpawn::ReadSerializedData(pugi::xml_node& node)
{
	parent::ReadSerializedData(node);
	pugi::xml_node AirBombSpawnNode = node.child("LootID_parameters");
	GetXMLVal("spawn_radius", AirBombSpawnNode, &spawnRadius);
}

//////////////////////////////////////////////////////////////////////////

GameObject * obj_AirBombSpawn::Clone()
{
	obj_AirBombSpawn * newSpawnPoint = static_cast<obj_AirBombSpawn*>(srv_CreateGameObject("obj_AirBombSpawn", FileName.c_str(), GetPosition()));
	newSpawnPoint->CloneParameters(this);
	return newSpawnPoint;
}

//////////////////////////////////////////////////////////////////////////

void obj_AirBombSpawn::CloneParameters(obj_AirBombSpawn *o)
{
	spawnRadius = o->spawnRadius;
}

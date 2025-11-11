#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "obj_BattleArena.h"
#include "XMLHelpers.h"

extern bool g_bEditMode;

IMPLEMENT_CLASS(obj_BattleArena, "obj_BattleArena", "Object");
AUTOREGISTER_CLASS(obj_BattleArena);

r3dgameVector(obj_BattleArena*) obj_BattleArena::LoadedArenaboxes;

namespace
{
	struct obj_BattleArenaCompositeRenderable: public Renderable
	{
		void Init()
		{
			DrawFunc = Draw;
		}

		static void Draw( Renderable* RThis, const r3dCamera& Cam )
		{
			obj_BattleArenaCompositeRenderable *This = static_cast<obj_BattleArenaCompositeRenderable*>(RThis);

			r3dRenderer->SetRenderingMode(R3D_BLEND_NZ | R3D_BLEND_PUSH);

			r3dDrawLine3D(This->Parent->GetPosition(), This->Parent->GetPosition() + r3dPoint3D(0, 20.0f, 0), Cam, 0.4f, r3dColor24::green);
			r3dDrawCircle3D(This->Parent->GetPosition(), This->Parent->useRadius, Cam, 0.1f, r3dColor(3, 133, 170));

			r3dRenderer->Flush();
			r3dRenderer->SetRenderingMode(R3D_BLEND_POP);
		}

		obj_BattleArena *Parent;	
	};
}

obj_BattleArena::obj_BattleArena()
{
	useRadius = 10.0f;
}

obj_BattleArena::~obj_BattleArena()
{
}

#define RENDERABLE_OBJ_USER_SORT_VALUE (3*RENDERABLE_USER_SORT_VALUE)
void obj_BattleArena::AppendRenderables(RenderArray (& render_arrays  )[ rsCount ], const r3dCamera& Cam)
{
	parent::AppendRenderables(render_arrays, Cam);
#ifdef FINAL_BUILD
	return;
#else
	if(g_bEditMode)
	{
		obj_BattleArenaCompositeRenderable rend;
		rend.Init();
		rend.Parent		= this;
		rend.SortValue	= RENDERABLE_OBJ_USER_SORT_VALUE;
		render_arrays[ rsDrawDebugData ].PushBack( rend );
	}
#endif
}

void obj_BattleArena::ReadSerializedData(pugi::xml_node& node)
{
	GameObject::ReadSerializedData(node);

	pugi::xml_node objNode = node.child("post_arena");
	GetXMLVal("useRadius", objNode, &useRadius);
}

void obj_BattleArena::WriteSerializedData(pugi::xml_node& node)
{
	GameObject::WriteSerializedData(node);

	pugi::xml_node objNode = node.append_child();
	objNode.set_name("post_arena");
	SetXMLVal("useRadius", objNode, &useRadius);
}

BOOL obj_BattleArena::Load(const char *fname)
{
	const char* cpMeshName = "Data\\ObjectsDepot\\Capture_Points\\Flag_Pole_01.sco";

	if(!parent::Load(cpMeshName)) 
		return FALSE;

	return TRUE;
}

BOOL obj_BattleArena::OnCreate()
{
	parent::OnCreate();

	ObjFlags |= OBJFLAG_DisableShadows;

	LoadedArenaboxes.push_back(this);
	return 1;
}


BOOL obj_BattleArena::OnDestroy()
{
	LoadedArenaboxes.erase(std::find(LoadedArenaboxes.begin(), LoadedArenaboxes.end(), this));
	return parent::OnDestroy();
}

BOOL obj_BattleArena::Update()
{
	return parent::Update();
}

//------------------------------------------------------------------------
#ifndef FINAL_BUILD
float obj_BattleArena::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	float starty = scry;

	starty += parent::DrawPropertyEditor(scrx, scry, scrw,scrh, startClass, selected );

	if( IsParentOrEqual( &ClassData, startClass ) )
	{		
		starty += imgui_Static ( scrx, starty, "Battle Arena Parameters" );
		starty += imgui_Value_Slider(scrx, starty, "Battle Arena Radius", &useRadius, 0, 500.0f, "%.0f");
	}

	return starty-scry;
}
#endif

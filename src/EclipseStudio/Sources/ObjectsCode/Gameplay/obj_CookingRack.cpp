#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "obj_CookingRack.h"
#include "XMLHelpers.h"

extern bool g_bEditMode;

IMPLEMENT_CLASS(obj_CookingRack, "obj_CookingRack", "Object");
AUTOREGISTER_CLASS(obj_CookingRack);

std::vector<obj_CookingRack*> obj_CookingRack::LoadedCookboxes;

namespace
{
	struct obj_CookingRackCompositeRenderable: public Renderable
	{
		void Init()
		{
			DrawFunc = Draw;
		}

		static void Draw( Renderable* RThis, const r3dCamera& Cam )
		{
			obj_CookingRackCompositeRenderable *This = static_cast<obj_CookingRackCompositeRenderable*>(RThis);

			r3dRenderer->SetRenderingMode(R3D_BLEND_NZ | R3D_BLEND_PUSH);

			r3dDrawLine3D(This->Parent->GetPosition(), This->Parent->GetPosition() + r3dPoint3D(0, 20.0f, 0), Cam, 0.4f, r3dColor24::grey);
			r3dDrawCircle3D(This->Parent->GetPosition(), This->Parent->useRadius, Cam, 0.1f, r3dColor(255, 0, 0));

			r3dRenderer->Flush();
			r3dRenderer->SetRenderingMode(R3D_BLEND_POP);
		}

		obj_CookingRack *Parent;	
	};
}

obj_CookingRack::obj_CookingRack()
{
	useRadius = 10.0f;
}

obj_CookingRack::~obj_CookingRack()
{
}

#define RENDERABLE_OBJ_USER_SORT_VALUE (3*RENDERABLE_USER_SORT_VALUE)
void obj_CookingRack::AppendRenderables(RenderArray (& render_arrays  )[ rsCount ], const r3dCamera& Cam)
{
	parent::AppendRenderables(render_arrays, Cam);
#ifdef FINAL_BUILD
	return;
#else
	if(g_bEditMode)
	{
		obj_CookingRackCompositeRenderable rend;
		rend.Init();
		rend.Parent		= this;
		rend.SortValue	= RENDERABLE_OBJ_USER_SORT_VALUE;
		render_arrays[ rsDrawDebugData ].PushBack( rend );
	}
#endif
}

void obj_CookingRack::ReadSerializedData(pugi::xml_node& node)
{
	GameObject::ReadSerializedData(node);

	pugi::xml_node objNode = node.child("cook_box");
	GetXMLVal("useRadius", objNode, &useRadius);
}

void obj_CookingRack::WriteSerializedData(pugi::xml_node& node)
{
	GameObject::WriteSerializedData(node);

	pugi::xml_node objNode = node.append_child();
	objNode.set_name("cook_box");
	SetXMLVal("useRadius", objNode, &useRadius);
}

BOOL obj_CookingRack::Load(const char *fname)
{
	const char* cpMeshName = "Data\\ObjectsDepot\\Capture_Points\\Flag_Pole_01.sco";

	if(!parent::Load(cpMeshName)) 
		return FALSE;

	return TRUE;
}

BOOL obj_CookingRack::OnCreate()
{
	parent::OnCreate();

	LoadedCookboxes.push_back(this);
	return 1;
}


BOOL obj_CookingRack::OnDestroy()
{
	LoadedCookboxes.erase(std::find(LoadedCookboxes.begin(), LoadedCookboxes.end(), this));
	return parent::OnDestroy();
}

BOOL obj_CookingRack::Update()
{
	return parent::Update();
}

//------------------------------------------------------------------------
#ifndef FINAL_BUILD
float obj_CookingRack::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	float starty = scry;

	starty += parent::DrawPropertyEditor(scrx, scry, scrw,scrh, startClass, selected );

	if( IsParentOrEqual( &ClassData, startClass ) )
	{		
		starty += imgui_Static ( scrx, starty, "Cook Parameters" );
		starty += imgui_Value_Slider(scrx, starty, "Cook Zone Radius", &useRadius, 0, 500.0f, "%.0f");
	}

	return starty-scry;
}
#endif

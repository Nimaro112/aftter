#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "obj_WaterWell.h"
#include "XMLHelpers.h"

extern bool g_bEditMode;

IMPLEMENT_CLASS(obj_WaterWell, "obj_WaterWell", "Object");
AUTOREGISTER_CLASS(obj_WaterWell);

std::vector<obj_WaterWell*> obj_WaterWell::LoadedWaterboxes;

namespace
{
	struct obj_WaterWellCompositeRenderable: public Renderable
	{
		void Init()
		{
			DrawFunc = Draw;
		}

		static void Draw( Renderable* RThis, const r3dCamera& Cam )
		{
			obj_WaterWellCompositeRenderable *This = static_cast<obj_WaterWellCompositeRenderable*>(RThis);

			r3dRenderer->SetRenderingMode(R3D_BLEND_NZ | R3D_BLEND_PUSH);

			r3dDrawLine3D(This->Parent->GetPosition(), This->Parent->GetPosition() + r3dPoint3D(0, 20.0f, 0), Cam, 0.4f, r3dColor24::grey);
			r3dDrawCircle3D(This->Parent->GetPosition(), This->Parent->useRadius, Cam, 0.1f, r3dColor(0, 186, 255));

			r3dRenderer->Flush();
			r3dRenderer->SetRenderingMode(R3D_BLEND_POP);
		}

		obj_WaterWell *Parent;	
	};
}

obj_WaterWell::obj_WaterWell()
{
	useRadius = 10.0f;
}

obj_WaterWell::~obj_WaterWell()
{
}

#define RENDERABLE_OBJ_USER_SORT_VALUE (3*RENDERABLE_USER_SORT_VALUE)
void obj_WaterWell::AppendRenderables(RenderArray (& render_arrays  )[ rsCount ], const r3dCamera& Cam)
{
	parent::AppendRenderables(render_arrays, Cam);
#ifdef FINAL_BUILD
	return;
#else
	if(g_bEditMode)
	{
		obj_WaterWellCompositeRenderable rend;
		rend.Init();
		rend.Parent		= this;
		rend.SortValue	= RENDERABLE_OBJ_USER_SORT_VALUE;
		render_arrays[ rsDrawDebugData ].PushBack( rend );
	}
#endif
}

void obj_WaterWell::ReadSerializedData(pugi::xml_node& node)
{
	GameObject::ReadSerializedData(node);

	pugi::xml_node objNode = node.child("water_box");
	GetXMLVal("useRadius", objNode, &useRadius);
}

void obj_WaterWell::WriteSerializedData(pugi::xml_node& node)
{
	GameObject::WriteSerializedData(node);

	pugi::xml_node objNode = node.append_child();
	objNode.set_name("water_box");
	SetXMLVal("useRadius", objNode, &useRadius);
}

BOOL obj_WaterWell::Load(const char *fname)
{
	const char* cpMeshName = "Data\\ObjectsDepot\\Capture_Points\\Flag_Pole_01.sco";

	if(!parent::Load(cpMeshName)) 
		return FALSE;

	return TRUE;
}

BOOL obj_WaterWell::OnCreate()
{
	parent::OnCreate();

	LoadedWaterboxes.push_back(this);
	return 1;
}


BOOL obj_WaterWell::OnDestroy()
{
	LoadedWaterboxes.erase(std::find(LoadedWaterboxes.begin(), LoadedWaterboxes.end(), this));
	return parent::OnDestroy();
}

BOOL obj_WaterWell::Update()
{
	return parent::Update();
}

//------------------------------------------------------------------------
#ifndef FINAL_BUILD
float obj_WaterWell::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	float starty = scry;

	starty += parent::DrawPropertyEditor(scrx, scry, scrw,scrh, startClass, selected );

	if( IsParentOrEqual( &ClassData, startClass ) )
	{		
		starty += imgui_Static ( scrx, starty, "Water Parameters" );
		starty += imgui_Value_Slider(scrx, starty, "Water Zone Radius", &useRadius, 0, 500.0f, "%.0f");
	}

	return starty-scry;
}
#endif

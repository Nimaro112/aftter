#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "obj_RadBox.h"
#include "XMLHelpers.h"

extern bool g_bEditMode;

IMPLEMENT_CLASS(obj_RadBox, "obj_RadBox", "Object");
AUTOREGISTER_CLASS(obj_RadBox);

std::vector<obj_RadBox*> obj_RadBox::LoadedRadboxes;

namespace
{
	struct obj_RadBoxCompositeRenderable: public Renderable
	{
		void Init()
		{
			DrawFunc = Draw;
		}

		static void Draw( Renderable* RThis, const r3dCamera& Cam )
		{
			obj_RadBoxCompositeRenderable *This = static_cast<obj_RadBoxCompositeRenderable*>(RThis);

			r3dRenderer->SetRenderingMode(R3D_BLEND_NZ | R3D_BLEND_PUSH);

			r3dDrawLine3D(This->Parent->GetPosition(), This->Parent->GetPosition() + r3dPoint3D(0, 20.0f, 0), Cam, 0.4f, r3dColor24::grey);
			r3dDrawCircle3D(This->Parent->GetPosition(), This->Parent->useRadius, Cam, 0.1f, r3dColor(3, 170, 14));

			r3dRenderer->Flush();
			r3dRenderer->SetRenderingMode(R3D_BLEND_POP);
		}

		obj_RadBox *Parent;	
	};
}

obj_RadBox::obj_RadBox()
{
	useRadius = 10.0f;
}

obj_RadBox::~obj_RadBox()
{
}

#define RENDERABLE_OBJ_USER_SORT_VALUE (3*RENDERABLE_USER_SORT_VALUE)
void obj_RadBox::AppendRenderables(RenderArray (& render_arrays  )[ rsCount ], const r3dCamera& Cam)
{
	parent::AppendRenderables(render_arrays, Cam);
#ifdef FINAL_BUILD
	return;
#else
	if(g_bEditMode)
	{
		obj_RadBoxCompositeRenderable rend;
		rend.Init();
		rend.Parent		= this;
		rend.SortValue	= RENDERABLE_OBJ_USER_SORT_VALUE;
		render_arrays[ rsDrawDebugData ].PushBack( rend );
	}
#endif
}

void obj_RadBox::ReadSerializedData(pugi::xml_node& node)
{
	GameObject::ReadSerializedData(node);

	pugi::xml_node objNode = node.child("rad_box");
	GetXMLVal("useRadius", objNode, &useRadius);
}

void obj_RadBox::WriteSerializedData(pugi::xml_node& node)
{
	GameObject::WriteSerializedData(node);

	pugi::xml_node objNode = node.append_child();
	objNode.set_name("rad_box");
	SetXMLVal("useRadius", objNode, &useRadius);
}

BOOL obj_RadBox::Load(const char *fname)
{
	const char* cpMeshName = "Data\\ObjectsDepot\\G3_Toxic_Props\\RadioActiv_crates_stack_Burrels_Random.sco";

	if(!parent::Load(cpMeshName)) 
		return FALSE;

	return TRUE;
}

BOOL obj_RadBox::OnCreate()
{
	parent::OnCreate();

	LoadedRadboxes.push_back(this);
	return 1;
}


BOOL obj_RadBox::OnDestroy()
{
	LoadedRadboxes.erase(std::find(LoadedRadboxes.begin(), LoadedRadboxes.end(), this));
	return parent::OnDestroy();
}

BOOL obj_RadBox::Update()
{
	return parent::Update();
}

//------------------------------------------------------------------------
#ifndef FINAL_BUILD
float obj_RadBox::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	float starty = scry;

	starty += parent::DrawPropertyEditor(scrx, scry, scrw,scrh, startClass, selected );

	if( IsParentOrEqual( &ClassData, startClass ) )
	{		
		starty += imgui_Static ( scrx, starty, "Rad Box Parameters" );
		starty += imgui_Value_Slider(scrx, starty, "Rad Zone Radius", &useRadius, 0, 500.0f, "%.0f");
	}

	return starty-scry;
}
#endif

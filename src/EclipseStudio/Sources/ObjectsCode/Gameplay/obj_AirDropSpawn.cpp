//=========================================================================
//	Module: obj_AirDropSpawn.cpp
//	Copyright (C) Online Warmongers Group Inc. 2012.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"

#include "obj_AirDropSpawn.h"
#include "../../XMLHelpers.h"
#include "../../Editors/LevelEditor.h"
#include "../WEAPONS/WeaponArmory.h"
#include "../../../../GameEngine/ai/AI_Brain.h"

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_CLASS(obj_AirDropSpawn, "obj_AirDropSpawn", "Object");
AUTOREGISTER_CLASS(obj_AirDropSpawn);

extern bool g_bEditMode;

//////////////////////////////////////////////////////////////////////////

namespace
{
//////////////////////////////////////////////////////////////////////////

	struct AirDropSpawnCompositeRenderable: public Renderable
	{
		void Init()
		{
			DrawFunc = Draw;
		}

		static void Draw( Renderable* RThis, const r3dCamera& Cam )
		{
			AirDropSpawnCompositeRenderable *This = static_cast<AirDropSpawnCompositeRenderable*>(RThis);

			r3dRenderer->SetRenderingMode(R3D_BLEND_NZ | R3D_BLEND_PUSH);

			r3dDrawLine3D(This->Parent->GetPosition(), This->Parent->GetPosition() + r3dPoint3D(0, 20.0f, 0), Cam, 0.4f, r3dColor24::yellow);
			r3dDrawCircle3D(This->Parent->GetPosition(), This->Parent->spawnRadius, Cam, 0.1f, r3dColor::orange);

			r3dRenderer->Flush();
			r3dRenderer->SetRenderingMode(R3D_BLEND_POP);
		}

		obj_AirDropSpawn *Parent;
	};
}

//////////////////////////////////////////////////////////////////////////

obj_AirDropSpawn::obj_AirDropSpawn()
: spawnRadius(30.0f)
{
	serializeFile = SF_ServerData;
	m_bEnablePhysics = false;
	m_DefaultItems = 1;
}

//////////////////////////////////////////////////////////////////////////

obj_AirDropSpawn::~obj_AirDropSpawn()
{

}

//////////////////////////////////////////////////////////////////////////

#define RENDERABLE_OBJ_USER_SORT_VALUE (3*RENDERABLE_USER_SORT_VALUE)
void obj_AirDropSpawn::AppendRenderables(RenderArray (& render_arrays  )[ rsCount ], const r3dCamera& Cam)
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

	AirDropSpawnCompositeRenderable rend;

	rend.Init();
	rend.Parent		= this;
	rend.SortValue	= RENDERABLE_OBJ_USER_SORT_VALUE;

	render_arrays[ rsDrawDebugData ].PushBack( rend );
#endif
}

//////////////////////////////////////////////////////////////////////////

BOOL obj_AirDropSpawn::OnCreate()
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

BOOL obj_AirDropSpawn::Update()
{
	return parent::Update();
}

//////////////////////////////////////////////////////////////////////////

BOOL obj_AirDropSpawn::OnDestroy()
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
float obj_AirDropSpawn::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	float y = scry;

	y += parent::DrawPropertyEditor(scrx, scry, scrw, scrh, startClass, selected);
	y += 5.0f;

	y += imgui_Static(scrx, y, "AirDrop parameters:");

	y += imgui_Value_Slider(scrx, y, "Radius", &spawnRadius, 5.0f, 2000.0f, "%0.2f");
	y += imgui_Checkbox(scrx, y, "Use Default Items", &m_DefaultItems, 1);
///////////////////////////////
		if(m_DefaultItems == 0)
		{
			static stringlist_t lootBoxNames1,lootBoxNames2,lootBoxNames3,lootBoxNames4,lootBoxNames5,lootBoxNames6,lootBoxNames7
			,lootBoxNames8,lootBoxNames9,lootBoxNames10,lootBoxNames11,lootBoxNames12,lootBoxNames13,lootBoxNames14,lootBoxNames15
			,lootBoxNames16,lootBoxNames17,lootBoxNames18,lootBoxNames19,lootBoxNames20;
			static int* lootBoxIDs = NULL;
			static int numLootBoxes = 0;
			if(numLootBoxes == 0)
			{
				r3dgameVector(tempS) lootBoxes;
				{
					tempS holder;
					holder.name = "EMPTY";
					holder.id = 0;
					lootBoxes.push_back(holder);		
				}

				g_pWeaponArmory->startItemSearch();
				while(g_pWeaponArmory->searchNextItem())
				{
					uint32_t itemID = g_pWeaponArmory->getCurrentSearchItemID();
					const BaseItemConfig* cfg = g_pWeaponArmory->getConfig(itemID);
					if( cfg->category == storecat_LootBox )
					{
						tempS holder;
						holder.name = cfg->m_StoreName;
						holder.id = cfg->m_itemID;
						lootBoxes.push_back(holder);						 
					}
				}
				// sort loot boxes by name (but keep first entry as EMPTY)
				if(lootBoxes.size()>3)
					std::sort(lootBoxes.begin()+1, lootBoxes.end(), SortLootboxesByName);

				numLootBoxes = (int)lootBoxes.size();
				lootBoxIDs = game_new int[numLootBoxes];
				for(int i=0; i<numLootBoxes; ++i)
				{
					lootBoxNames1.push_back(lootBoxes[i].name);
					lootBoxNames2.push_back(lootBoxes[i].name);
					lootBoxNames3.push_back(lootBoxes[i].name);
					lootBoxNames4.push_back(lootBoxes[i].name);
					lootBoxNames5.push_back(lootBoxes[i].name);
					lootBoxNames6.push_back(lootBoxes[i].name);
					lootBoxNames7.push_back(lootBoxes[i].name);
					lootBoxNames8.push_back(lootBoxes[i].name);
					lootBoxNames9.push_back(lootBoxes[i].name);
					lootBoxNames10.push_back(lootBoxes[i].name);
					lootBoxNames11.push_back(lootBoxes[i].name);
					lootBoxNames12.push_back(lootBoxes[i].name);
					lootBoxNames13.push_back(lootBoxes[i].name);
					lootBoxNames14.push_back(lootBoxes[i].name);
					lootBoxNames15.push_back(lootBoxes[i].name);
					lootBoxNames16.push_back(lootBoxes[i].name);
					lootBoxNames17.push_back(lootBoxes[i].name);
					lootBoxNames18.push_back(lootBoxes[i].name);
					lootBoxNames19.push_back(lootBoxes[i].name);
					lootBoxNames20.push_back(lootBoxes[i].name);
					lootBoxIDs[i] = lootBoxes[i].id;
				}
			}
			int sel = 0;
			int sel2 = 0;
			int sel3 = 0;
			int sel4 = 0;
			int sel5 = 0;
			int sel6 = 0;
			int sel7 = 0;
			int sel8 = 0;
			int sel9 = 0;
			int sel10 = 0;
			int sel11 = 0;
			int sel12 = 0;
			int sel13 = 0;
			int sel14 = 0;
			int sel15 = 0;
			int sel16 = 0;
			int sel17 = 0;
			int sel18 = 0;
			int sel19 = 0;
			int sel20 = 0;
			static float offset = 0;
			static float offset2 = 0;
			static float offset3 = 0;
			static float offset4 = 0;
			static float offset5 = 0;
			static float offset6 = 0;
			static float offset7 = 0;
			static float offset8 = 0;
			static float offset9 = 0;
			static float offset10 = 0;
			static float offset11 = 0;
			static float offset12 = 0;
			static float offset13 = 0;
			static float offset14 = 0;
			static float offset15 = 0;
			static float offset16 = 0;
			static float offset17 = 0;
			static float offset18 = 0;
			static float offset19 = 0;
			static float offset20 = 0;
			for(int i=0; i<numLootBoxes; ++i)
			{
				if(m_LootBoxID1 == lootBoxIDs[i])
					sel = i;
				if(m_LootBoxID2 == lootBoxIDs[i])
					sel2 = i;
				if(m_LootBoxID3 == lootBoxIDs[i])
					sel3 = i;
				if(m_LootBoxID4 == lootBoxIDs[i])
					sel4 = i;
				if(m_LootBoxID5 == lootBoxIDs[i])
					sel5 = i;
				if(m_LootBoxID6 == lootBoxIDs[i])
					sel6 = i;
				if(m_LootBoxID7 == lootBoxIDs[i])
					sel7 = i;
				if(m_LootBoxID8 == lootBoxIDs[i])
					sel8 = i;
				if(m_LootBoxID9 == lootBoxIDs[i])
					sel9 = i;
				if(m_LootBoxID10 == lootBoxIDs[i])
					sel10 = i;
				if(m_LootBoxID11 == lootBoxIDs[i])
					sel11 = i;
				if(m_LootBoxID12 == lootBoxIDs[i])
					sel12 = i;
				if(m_LootBoxID13 == lootBoxIDs[i])
					sel13 = i;
				if(m_LootBoxID14 == lootBoxIDs[i])
					sel14 = i;
				if(m_LootBoxID15 == lootBoxIDs[i])
					sel15 = i;
				if(m_LootBoxID16 == lootBoxIDs[i])
					sel16 = i;
				if(m_LootBoxID17 == lootBoxIDs[i])
					sel17 = i;
				if(m_LootBoxID18 == lootBoxIDs[i])
					sel18 = i;
				if(m_LootBoxID19 == lootBoxIDs[i])
					sel19 = i;
				if(m_LootBoxID20 == lootBoxIDs[i])
					sel20 = i;
			}
			y += imgui_Static ( scrx, y, "Loot box 1:" );
			if(imgui_DrawList(scrx, y, 360, 122, lootBoxNames1, &offset, &sel))
			{
				m_LootBoxID1 = lootBoxIDs[sel];
				PropagateChange( m_LootBoxID1, &obj_AirDropSpawn::m_LootBoxID1, this, selected ) ;
			}
			y += 122;
			y += imgui_Static ( scrx, y, "Loot box 2:" );
			if(imgui_DrawList(scrx, y, 360, 122, lootBoxNames2, &offset2, &sel2))
			{
				m_LootBoxID2 = lootBoxIDs[sel2];
				PropagateChange( m_LootBoxID2, &obj_AirDropSpawn::m_LootBoxID2, this, selected ) ;
			}
			y += 122;
			y += imgui_Static ( scrx, y, "Loot box 3:" );
			if(imgui_DrawList(scrx, y, 360, 122, lootBoxNames3, &offset3, &sel3))
			{
				m_LootBoxID3 = lootBoxIDs[sel3];
				PropagateChange( m_LootBoxID3, &obj_AirDropSpawn::m_LootBoxID3, this, selected ) ;
			}
			y += 122;
			y += imgui_Static ( scrx, y, "Loot box 4:" );
			if(imgui_DrawList(scrx, y, 360, 122, lootBoxNames4, &offset4, &sel4))
			{
				m_LootBoxID4 = lootBoxIDs[sel4];
				PropagateChange( m_LootBoxID4, &obj_AirDropSpawn::m_LootBoxID4, this, selected ) ;
			}
			y += 122;
			y += imgui_Static ( scrx, y, "Loot box 5:" );
			if(imgui_DrawList(scrx, y, 360, 122, lootBoxNames5, &offset5, &sel5))
			{
				m_LootBoxID5 = lootBoxIDs[sel5];
				PropagateChange( m_LootBoxID5, &obj_AirDropSpawn::m_LootBoxID5, this, selected ) ;
			}
			y += 122;
			y += imgui_Static ( scrx, y, "Loot box 6:" );
			if(imgui_DrawList(scrx, y, 360, 122, lootBoxNames6, &offset6, &sel6))
			{
				m_LootBoxID6 = lootBoxIDs[sel6];
				PropagateChange( m_LootBoxID6, &obj_AirDropSpawn::m_LootBoxID6, this, selected ) ;
			}
			y += 122;
			y += imgui_Static ( scrx, y, "Loot box 7:" );
			if(imgui_DrawList(scrx, y, 360, 122, lootBoxNames7, &offset7, &sel7))
			{
				m_LootBoxID7 = lootBoxIDs[sel7];
				PropagateChange( m_LootBoxID7, &obj_AirDropSpawn::m_LootBoxID7, this, selected ) ;
			}
			y += 122;
			y += imgui_Static ( scrx, y, "Loot box 8:" );
			if(imgui_DrawList(scrx, y, 360, 122, lootBoxNames8, &offset8, &sel8))
			{
				m_LootBoxID8 = lootBoxIDs[sel8];
				PropagateChange( m_LootBoxID8, &obj_AirDropSpawn::m_LootBoxID8, this, selected ) ;
			}
			y += 122;
			y += imgui_Static ( scrx, y, "Loot box 9:" );
			if(imgui_DrawList(scrx, y, 360, 122, lootBoxNames9, &offset9, &sel9))
			{
				m_LootBoxID9 = lootBoxIDs[sel9];
				PropagateChange( m_LootBoxID9, &obj_AirDropSpawn::m_LootBoxID9, this, selected ) ;
			}
			y += 122;
			y += imgui_Static ( scrx, y, "Loot box 10:" );
			if(imgui_DrawList(scrx, y, 360, 122, lootBoxNames10, &offset10, &sel10))
			{
				m_LootBoxID10 = lootBoxIDs[sel10];
				PropagateChange( m_LootBoxID10, &obj_AirDropSpawn::m_LootBoxID10, this, selected ) ;
			}
			y += 122;
			y += imgui_Static ( scrx, y, "Loot box 11:" );
			if(imgui_DrawList(scrx, y, 360, 122, lootBoxNames11, &offset11, &sel11))
			{
				m_LootBoxID11 = lootBoxIDs[sel11];
				PropagateChange( m_LootBoxID11, &obj_AirDropSpawn::m_LootBoxID11, this, selected ) ;
			}
			y += 122;
			y += imgui_Static ( scrx, y, "Loot box 12:" );
			if(imgui_DrawList(scrx, y, 360, 122, lootBoxNames12, &offset12, &sel12))
			{
				m_LootBoxID12 = lootBoxIDs[sel12];
				PropagateChange( m_LootBoxID12, &obj_AirDropSpawn::m_LootBoxID12, this, selected ) ;
			}
			y += 122;
			y += imgui_Static ( scrx, y, "Loot box 13:" );
			if(imgui_DrawList(scrx, y, 360, 122, lootBoxNames13, &offset13, &sel13))
			{
				m_LootBoxID13 = lootBoxIDs[sel13];
				PropagateChange( m_LootBoxID13, &obj_AirDropSpawn::m_LootBoxID13, this, selected ) ;
			}
			y += 122;
			y += imgui_Static ( scrx, y, "Loot box 14:" );
			if(imgui_DrawList(scrx, y, 360, 122, lootBoxNames14, &offset14, &sel14))
			{
				m_LootBoxID14 = lootBoxIDs[sel14];
				PropagateChange( m_LootBoxID14, &obj_AirDropSpawn::m_LootBoxID14, this, selected ) ;
			}
			y += 122;
			y += imgui_Static ( scrx, y, "Loot box 15:" );
			if(imgui_DrawList(scrx, y, 360, 122, lootBoxNames15, &offset15, &sel15))
			{
				m_LootBoxID15 = lootBoxIDs[sel15];
				PropagateChange( m_LootBoxID15, &obj_AirDropSpawn::m_LootBoxID15, this, selected ) ;
			}
			y += 122;
			y += imgui_Static ( scrx, y, "Loot box 16:" );
			if(imgui_DrawList(scrx, y, 360, 122, lootBoxNames16, &offset16, &sel16))
			{
				m_LootBoxID16 = lootBoxIDs[sel16];
				PropagateChange( m_LootBoxID16, &obj_AirDropSpawn::m_LootBoxID16, this, selected ) ;
			}
			y += 122;
			y += imgui_Static ( scrx, y, "Loot box 17:" );
			if(imgui_DrawList(scrx, y, 360, 122, lootBoxNames17, &offset17, &sel17))
			{
				m_LootBoxID17 = lootBoxIDs[sel17];
				PropagateChange( m_LootBoxID17, &obj_AirDropSpawn::m_LootBoxID17, this, selected ) ;
			}
			y += 122;
			y += imgui_Static ( scrx, y, "Loot box 18:" );
			if(imgui_DrawList(scrx, y, 360, 122, lootBoxNames18, &offset18, &sel18))
			{
				m_LootBoxID18 = lootBoxIDs[sel18];
				PropagateChange( m_LootBoxID18, &obj_AirDropSpawn::m_LootBoxID18, this, selected ) ;
			}
			y += 122;
			y += imgui_Static ( scrx, y, "Loot box 19:" );
			if(imgui_DrawList(scrx, y, 360, 122, lootBoxNames19, &offset19, &sel19))
			{
				m_LootBoxID19 = lootBoxIDs[sel19];
				PropagateChange( m_LootBoxID19, &obj_AirDropSpawn::m_LootBoxID19, this, selected ) ;
			}
			y += 122;
			y += imgui_Static ( scrx, y, "Loot box 20:" );
			if(imgui_DrawList(scrx, y, 360, 122, lootBoxNames20, &offset20, &sel20))
			{
				m_LootBoxID20 = lootBoxIDs[sel20];
				PropagateChange( m_LootBoxID20, &obj_AirDropSpawn::m_LootBoxID20, this, selected ) ;
			}
			y += 122;
		}
///////////////////////////////

	return y - scry;
}
#endif

//////////////////////////////////////////////////////////////////////////

void obj_AirDropSpawn::WriteSerializedData(pugi::xml_node& node)
{
	parent::WriteSerializedData(node);
	pugi::xml_node AirDropSpawnNode = node.append_child();
	AirDropSpawnNode.set_name("LootID_parameters");
	SetXMLVal("spawn_radius", AirDropSpawnNode, &spawnRadius);
	SetXMLVal("m_DefaultItems", AirDropSpawnNode, &m_DefaultItems);
	if (m_DefaultItems == 1)
	{
		m_LootBoxID1 = 0;
		m_LootBoxID2 = 0;
		m_LootBoxID3 = 0;
		m_LootBoxID4 = 0;
		m_LootBoxID5 = 0;
		m_LootBoxID6 = 0;
		m_LootBoxID7 = 0;
		m_LootBoxID8 = 0;
		m_LootBoxID9 = 0;
		m_LootBoxID10 = 0;
		m_LootBoxID11 = 0;
		m_LootBoxID12 = 0;
		m_LootBoxID13 = 0;
		m_LootBoxID14 = 0;
		m_LootBoxID15 = 0;
		m_LootBoxID16 = 0;
		m_LootBoxID17 = 0;
		m_LootBoxID18 = 0;
		m_LootBoxID19 = 0;
		m_LootBoxID20 = 0;
	}
	SetXMLVal("m_LootBoxID1", AirDropSpawnNode, &m_LootBoxID1);
	SetXMLVal("m_LootBoxID2", AirDropSpawnNode, &m_LootBoxID2);
	SetXMLVal("m_LootBoxID3", AirDropSpawnNode, &m_LootBoxID3);
	SetXMLVal("m_LootBoxID4", AirDropSpawnNode, &m_LootBoxID4);
	SetXMLVal("m_LootBoxID5", AirDropSpawnNode, &m_LootBoxID5);
	SetXMLVal("m_LootBoxID6", AirDropSpawnNode, &m_LootBoxID6);
	SetXMLVal("m_LootBoxID7", AirDropSpawnNode, &m_LootBoxID7);
	SetXMLVal("m_LootBoxID8", AirDropSpawnNode, &m_LootBoxID8);
	SetXMLVal("m_LootBoxID9", AirDropSpawnNode, &m_LootBoxID9);
	SetXMLVal("m_LootBoxID10", AirDropSpawnNode, &m_LootBoxID10);
	SetXMLVal("m_LootBoxID11", AirDropSpawnNode, &m_LootBoxID11);
	SetXMLVal("m_LootBoxID12", AirDropSpawnNode, &m_LootBoxID12);
	SetXMLVal("m_LootBoxID13", AirDropSpawnNode, &m_LootBoxID13);
	SetXMLVal("m_LootBoxID14", AirDropSpawnNode, &m_LootBoxID14);
	SetXMLVal("m_LootBoxID15", AirDropSpawnNode, &m_LootBoxID15);
	SetXMLVal("m_LootBoxID16", AirDropSpawnNode, &m_LootBoxID16);
	SetXMLVal("m_LootBoxID17", AirDropSpawnNode, &m_LootBoxID17);
	SetXMLVal("m_LootBoxID18", AirDropSpawnNode, &m_LootBoxID18);
	SetXMLVal("m_LootBoxID19", AirDropSpawnNode, &m_LootBoxID19);
	SetXMLVal("m_LootBoxID20", AirDropSpawnNode, &m_LootBoxID20);
}

// NOTE: this function must stay in sync with server version
void obj_AirDropSpawn::ReadSerializedData(pugi::xml_node& node)
{
	parent::ReadSerializedData(node);
	pugi::xml_node AirDropSpawnNode = node.child("LootID_parameters");
	GetXMLVal("spawn_radius", AirDropSpawnNode, &spawnRadius);
	GetXMLVal("m_DefaultItems", AirDropSpawnNode, &m_DefaultItems);
	GetXMLVal("m_LootBoxID1", AirDropSpawnNode, &m_LootBoxID1);
	GetXMLVal("m_LootBoxID2", AirDropSpawnNode, &m_LootBoxID2);
	GetXMLVal("m_LootBoxID3", AirDropSpawnNode, &m_LootBoxID3);
	GetXMLVal("m_LootBoxID4", AirDropSpawnNode, &m_LootBoxID4);
	GetXMLVal("m_LootBoxID5", AirDropSpawnNode, &m_LootBoxID5);
	GetXMLVal("m_LootBoxID6", AirDropSpawnNode, &m_LootBoxID6);
	GetXMLVal("m_LootBoxID7", AirDropSpawnNode, &m_LootBoxID7);
	GetXMLVal("m_LootBoxID8", AirDropSpawnNode, &m_LootBoxID8);
	GetXMLVal("m_LootBoxID9", AirDropSpawnNode, &m_LootBoxID9);
	GetXMLVal("m_LootBoxID10", AirDropSpawnNode, &m_LootBoxID10);
	GetXMLVal("m_LootBoxID11", AirDropSpawnNode, &m_LootBoxID11);
	GetXMLVal("m_LootBoxID12", AirDropSpawnNode, &m_LootBoxID12);
	GetXMLVal("m_LootBoxID13", AirDropSpawnNode, &m_LootBoxID13);
	GetXMLVal("m_LootBoxID14", AirDropSpawnNode, &m_LootBoxID14);
	GetXMLVal("m_LootBoxID15", AirDropSpawnNode, &m_LootBoxID15);
	GetXMLVal("m_LootBoxID16", AirDropSpawnNode, &m_LootBoxID16);
	GetXMLVal("m_LootBoxID17", AirDropSpawnNode, &m_LootBoxID17);
	GetXMLVal("m_LootBoxID18", AirDropSpawnNode, &m_LootBoxID18);
	GetXMLVal("m_LootBoxID19", AirDropSpawnNode, &m_LootBoxID19);
	GetXMLVal("m_LootBoxID20", AirDropSpawnNode, &m_LootBoxID20);
}

//////////////////////////////////////////////////////////////////////////

GameObject * obj_AirDropSpawn::Clone()
{
	obj_AirDropSpawn * newSpawnPoint = static_cast<obj_AirDropSpawn*>(srv_CreateGameObject("obj_AirDropSpawn", FileName.c_str(), GetPosition()));
	newSpawnPoint->CloneParameters(this);
	return newSpawnPoint;
}

//////////////////////////////////////////////////////////////////////////

void obj_AirDropSpawn::CloneParameters(obj_AirDropSpawn *o)
{
	spawnRadius = o->spawnRadius;
}

#pragma once

#include "GameCommon.h"

class obj_Dmg : public MeshGameObject
{
	DECLARE_CLASS(obj_Dmg, MeshGameObject)
	
public:
	float		useRadius;
		
	static std::vector<obj_Dmg*> LoadedRadboxes;
public:
	obj_Dmg();
	virtual ~obj_Dmg();

	virtual	BOOL		Load(const char *name);

	virtual	BOOL		OnCreate();
	virtual	BOOL		OnDestroy();

	virtual	BOOL		Update();
 #ifndef FINAL_BUILD
 	virtual	float		DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected) OVERRIDE;
 #endif
	virtual	void		AppendRenderables(RenderArray (& render_arrays  )[ rsCount ], const r3dCamera& Cam);
	virtual void		WriteSerializedData(pugi::xml_node& node);
	virtual	void		ReadSerializedData(pugi::xml_node& node);

};

#pragma once

#include "GameCommon.h"

class obj_RadBox : public MeshGameObject
{
	DECLARE_CLASS(obj_RadBox, MeshGameObject)
	
public:
	float		useRadius;
		
	static std::vector<obj_RadBox*> LoadedRadboxes;
public:
	obj_RadBox();
	virtual ~obj_RadBox();

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

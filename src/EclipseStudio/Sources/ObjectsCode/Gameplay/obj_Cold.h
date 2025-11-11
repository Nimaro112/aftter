#pragma once

#include "GameCommon.h"

class obj_Cold : public MeshGameObject
{
	DECLARE_CLASS(obj_Cold, MeshGameObject)
	
public:
	float		useRadius;
		
	static std::vector<obj_Cold*> LoadedCold;
public:
	obj_Cold();
	virtual ~obj_Cold();

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

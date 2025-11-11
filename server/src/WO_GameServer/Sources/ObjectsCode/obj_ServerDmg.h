#pragma once

#include "GameCommon.h"

class obj_ServerDmg : public GameObject
{
	DECLARE_CLASS(obj_ServerDmg, GameObject)

public:
	float		useRadius;

public:
	obj_ServerDmg();
	~obj_ServerDmg();

	virtual BOOL	OnCreate();
	virtual	void	ReadSerializedData(pugi::xml_node& node);
};

class DmgBoxesMgr
{
public:
	enum { MAX_DMG_BOXES = 256 }; // 256 should be more than enough, if not, will redo into vector
	obj_ServerDmg* dmgBoxes_[MAX_DMG_BOXES];
	int		numDmgBoxes_;

	void RegisterDmgBox(obj_ServerDmg* dbox) 
	{
		r3d_assert(numDmgBoxes_ < MAX_DMG_BOXES);
		dmgBoxes_[numDmgBoxes_++] = dbox;
	}

public:
	DmgBoxesMgr() { numDmgBoxes_ = 0; }
	~DmgBoxesMgr() {}
};

extern	DmgBoxesMgr gDmgBoxesMngr;

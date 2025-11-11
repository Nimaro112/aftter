#pragma once

#include "GameCommon.h"

class obj_ServerCookingRack : public GameObject
{
	DECLARE_CLASS(obj_ServerCookingRack, GameObject)

public:
	float		useRadius;

public:
	obj_ServerCookingRack();
	~obj_ServerCookingRack();

	virtual BOOL	OnCreate();
	virtual	void	ReadSerializedData(pugi::xml_node& node);
};

class CookBoxesMgr
{
public:
	enum { MAX_COOK_BOXES = 256 }; // 256 should be more than enough, if not, will redo into vector
	obj_ServerCookingRack* cookBoxes_[MAX_COOK_BOXES];
	int		numcookBoxes_;

	void RegisterCookBox(obj_ServerCookingRack* cbox) 
	{
		r3d_assert(numcookBoxes_ < MAX_COOK_BOXES);
		cookBoxes_[numcookBoxes_++] = cbox;
	}

public:
	CookBoxesMgr() { numcookBoxes_ = 0; }
	~CookBoxesMgr() {}
};

extern	CookBoxesMgr gCookBoxesMngr;

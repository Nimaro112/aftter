#pragma once

#include "GameCommon.h"

class obj_ServerRadBox : public GameObject
{
	DECLARE_CLASS(obj_ServerRadBox, GameObject)

public:
	float		useRadius;

public:
	obj_ServerRadBox();
	~obj_ServerRadBox();

	virtual BOOL	OnCreate();
	virtual	void	ReadSerializedData(pugi::xml_node& node);
};

class RadBoxesMgr
{
public:
	enum { MAX_RAD_BOXES = 256 }; // 256 should be more than enough, if not, will redo into vector
	obj_ServerRadBox* radBoxes_[MAX_RAD_BOXES];
	int		numRadBoxes_;

	void RegisterRadBox(obj_ServerRadBox* rbox) 
	{
		r3d_assert(numRadBoxes_ < MAX_RAD_BOXES);
		radBoxes_[numRadBoxes_++] = rbox;
	}

public:
	RadBoxesMgr() { numRadBoxes_ = 0; }
	~RadBoxesMgr() {}
};

extern	RadBoxesMgr gRadBoxesMngr;

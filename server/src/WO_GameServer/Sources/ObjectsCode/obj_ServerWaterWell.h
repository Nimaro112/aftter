#pragma once

#include "GameCommon.h"

class obj_ServerWaterWell : public GameObject
{
	DECLARE_CLASS(obj_ServerWaterWell, GameObject)

public:
	float		useRadius;

public:
	obj_ServerWaterWell();
	~obj_ServerWaterWell();

	virtual BOOL	OnCreate();
	virtual	void	ReadSerializedData(pugi::xml_node& node);
};

class WaterBoxesMgr
{
public:
	enum { MAX_WATER_BOXES = 256 }; // 256 should be more than enough, if not, will redo into vector
	obj_ServerWaterWell* waterBoxes_[MAX_WATER_BOXES];
	int		numwaterBoxes_;

	void RegisterWaterBox(obj_ServerWaterWell* wbox) 
	{
		r3d_assert(numwaterBoxes_ < MAX_WATER_BOXES);
		waterBoxes_[numwaterBoxes_++] = wbox;
	}

public:
	WaterBoxesMgr() { numwaterBoxes_ = 0; }
	~WaterBoxesMgr() {}
};

extern	WaterBoxesMgr gWaterBoxesMngr;

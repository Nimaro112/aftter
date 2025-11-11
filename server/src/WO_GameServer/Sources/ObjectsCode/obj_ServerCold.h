#pragma once

#include "GameCommon.h"

class obj_ServerCold : public GameObject
{
	DECLARE_CLASS(obj_ServerCold, GameObject)

public:
	float		useRadius;

public:
	obj_ServerCold();
	~obj_ServerCold();

	virtual BOOL	OnCreate();
	virtual	void	ReadSerializedData(pugi::xml_node& node);
};

class ColdMgr
{
public:
	enum { MAX_COLD_BOXES = 256 }; // 256 should be more than enough, if not, will redo into vector
	obj_ServerCold* Cold_[MAX_COLD_BOXES];
	int		numCold_;

	void RegisterCold(obj_ServerCold* rbox) 
	{
		r3d_assert(numCold_ < MAX_COLD_BOXES);
		Cold_[numCold_++] = rbox;
	}

public:
	ColdMgr() { numCold_ = 0; }
	~ColdMgr() {}
};

extern	ColdMgr gColdMngr;

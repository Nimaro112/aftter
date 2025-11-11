#pragma once

#include "GameCommon.h"

class obj_ServerStairs : public GameObject
{
	DECLARE_CLASS(obj_ServerStairs, GameObject)

public:
	float		useRadius;

public:
	obj_ServerStairs();
	~obj_ServerStairs();

	virtual BOOL	OnCreate();
	virtual	void	ReadSerializedData(pugi::xml_node& node);
};

class StairsBoxesMgr
{
public:
	enum { MAX_STAIRS_BOXES = 256 }; // 256 should be more than enough, if not, will redo into vector
	obj_ServerStairs* stairsBoxes_[MAX_STAIRS_BOXES];
	int		numstairsBoxes_;

	void RegisterStairsBox(obj_ServerStairs* sbox) 
	{
		r3d_assert(numstairsBoxes_ < MAX_STAIRS_BOXES);
		stairsBoxes_[numstairsBoxes_++] = sbox;
	}

public:
	StairsBoxesMgr() { numstairsBoxes_ = 0; }
	~StairsBoxesMgr() {}
};

extern	StairsBoxesMgr gStairsBoxesMngr;

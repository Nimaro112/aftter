#pragma once

#include "GameCommon.h"

class obj_ServerBattleArena : public GameObject
{
	DECLARE_CLASS(obj_ServerBattleArena, GameObject)

public:
	float		useRadius;

public:
	obj_ServerBattleArena();
	virtual ~obj_ServerBattleArena();

	virtual BOOL	OnCreate();
	virtual	void	ReadSerializedData(pugi::xml_node& node);
};

class ArenaBoxesMgr
{
public:
	enum { MAX_ARENA_BOXES = 32 }; // 32 should be more than enough, if not, will redo into vector
	obj_ServerBattleArena* arenaBoxes_[MAX_ARENA_BOXES];
	int		numArenaBoxes_;

	void RegisterArenaBox(obj_ServerBattleArena* abox) 
	{
		r3d_assert(numArenaBoxes_ < MAX_ARENA_BOXES);
		arenaBoxes_[numArenaBoxes_++] = abox;
	}

public:
	ArenaBoxesMgr() { numArenaBoxes_ = 0; }
	~ArenaBoxesMgr() {}
};

extern	ArenaBoxesMgr gArenaBoxesMngr;

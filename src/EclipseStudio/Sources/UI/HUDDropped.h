#pragma once

#include "APIScaleformGfx.h"
#include "GameCode/UserProfile.h"

class HUDDropped
{
	bool	isActive_;
	bool	isInit;

	bool	isLockedUI;
	int		lockedOp;

private:
	r3dScaleformMovie gfxMovie;
	r3dScaleformMovie* prevKeyboardCaptureMovie;
public:
	enum { MAX_DROPPED_SIZE = 64, };
	uint32_t	NumItems;
	uint32_t	CurrentDroppedID;
	char		CurrentAccessCodeString[32];
	wiInventoryItem	DroppedContent[MAX_DROPPED_SIZE];

public:
	HUDDropped();
	~HUDDropped();

	bool 	Init();
	bool 	Unload();

	bool	IsInited() const { return isInit; }

	void 	Update();
	void 	Draw();

	bool	isActive() const { return isActive_; }
	void	Activate();
	void	Deactivate();

	bool	isUILocked() const { return isLockedUI; }
	void	setErrorMsg(const char* errMsg);
	void	unlockUI();

	void	reloadBackpackInfo();
	void	reloadDroppedInfo();

	void	updateSurvivorTotalWeight();

	void	eventBackpackFromInventory(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventBackpackToInventory(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventBackpackGridSwap(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventReturnToGame(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventChangeKeyCode(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventPickupLockbox(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
};

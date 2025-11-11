#pragma once

#include "GameCommon.h"

class obj_ServerMarketNpc : public GameObject
{
	DECLARE_CLASS(obj_ServerMarketNpc, GameObject)

public:
	obj_ServerMarketNpc();
	virtual ~obj_ServerMarketNpc();

	virtual BOOL	OnCreate();
	virtual BOOL	OnDestroy();

	static bool isCloseToMarketNpc(const r3dPoint3D& pos);
};

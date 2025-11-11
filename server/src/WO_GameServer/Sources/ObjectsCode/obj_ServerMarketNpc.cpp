#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "XMLHelpers.h"

#include "multiplayer/P2PMessages.h"
#include "ServerGameLogic.h"

#include "obj_ServerMarketNpc.h"

IMPLEMENT_CLASS(obj_ServerMarketNpc, "obj_MarketNpc", "Object");
AUTOREGISTER_CLASS(obj_ServerMarketNpc);

static std::vector<obj_ServerMarketNpc*> s_Sellers;

bool obj_ServerMarketNpc::isCloseToMarketNpc(const r3dPoint3D& pos)
{
	float minDist = 9999999.0f;
	for(size_t i=0; i<s_Sellers.size(); ++i)
	{
		float d = (pos-s_Sellers[i]->GetPosition()).Length();
		if(d < minDist)
			minDist = d;
	}

	return minDist <= 4.0f;
}

obj_ServerMarketNpc::obj_ServerMarketNpc() 
{
}

obj_ServerMarketNpc::~obj_ServerMarketNpc()
{
}

BOOL obj_ServerMarketNpc::OnCreate()
{
	s_Sellers.push_back(this);
	return parent::OnCreate();
}

BOOL obj_ServerMarketNpc::OnDestroy()
{
	s_Sellers.erase(std::find(s_Sellers.begin(), s_Sellers.end(), this));
	return parent::OnDestroy();
}
#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "XMLHelpers.h"

#include "multiplayer/P2PMessages.h"
#include "ServerGameLogic.h"

#include "obj_ServerBattleArena.h"

IMPLEMENT_CLASS(obj_ServerBattleArena, "obj_BattleArena", "Object");
AUTOREGISTER_CLASS(obj_ServerBattleArena);

ArenaBoxesMgr gArenaBoxesMngr;

obj_ServerBattleArena::obj_ServerBattleArena()
{
	useRadius = 2.0f;
}

obj_ServerBattleArena::~obj_ServerBattleArena()
{
}

BOOL obj_ServerBattleArena::OnCreate()
{
	parent::OnCreate();

	gArenaBoxesMngr.RegisterArenaBox(this);
	return 1;
}

// copy from client version
void obj_ServerBattleArena::ReadSerializedData(pugi::xml_node& node)
{
	parent::ReadSerializedData(node);
	pugi::xml_node objNode = node.child("post_arena");
	GetXMLVal("useRadius", objNode, &useRadius);
}

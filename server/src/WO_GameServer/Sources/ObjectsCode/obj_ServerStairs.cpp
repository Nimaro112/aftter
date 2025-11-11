#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "XMLHelpers.h"

#include "multiplayer/P2PMessages.h"
#include "ServerGameLogic.h"

#include "obj_ServerStairs.h"

IMPLEMENT_CLASS(obj_ServerStairs, "obj_Stairs", "Object");
AUTOREGISTER_CLASS(obj_ServerStairs);

StairsBoxesMgr gStairsBoxesMngr;

obj_ServerStairs::obj_ServerStairs()
{
	useRadius = 3.0f;
}

obj_ServerStairs::~obj_ServerStairs()
{
}

BOOL obj_ServerStairs::OnCreate()
{
	parent::OnCreate();

	gStairsBoxesMngr.RegisterStairsBox(this);
	return 1;
}

// copy from client version
void obj_ServerStairs::ReadSerializedData(pugi::xml_node& node)
{
	parent::ReadSerializedData(node);
	pugi::xml_node objNode = node.child("stairs_box");
	GetXMLVal("useRadius", objNode, &useRadius);
}

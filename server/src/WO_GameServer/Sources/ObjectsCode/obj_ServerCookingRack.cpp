#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "XMLHelpers.h"

#include "multiplayer/P2PMessages.h"
#include "ServerGameLogic.h"

#include "obj_ServerCookingRack.h"

IMPLEMENT_CLASS(obj_ServerCookingRack, "obj_CookingRack", "Object");
AUTOREGISTER_CLASS(obj_ServerCookingRack);

CookBoxesMgr gCookBoxesMngr;

obj_ServerCookingRack::obj_ServerCookingRack()
{
	useRadius = 2.0f;
}

obj_ServerCookingRack::~obj_ServerCookingRack()
{
}

BOOL obj_ServerCookingRack::OnCreate()
{
	parent::OnCreate();

	gCookBoxesMngr.RegisterCookBox(this);
	return 1;
}

// copy from client version
void obj_ServerCookingRack::ReadSerializedData(pugi::xml_node& node)
{
	parent::ReadSerializedData(node);
	pugi::xml_node objNode = node.child("cook_box");
	GetXMLVal("useRadius", objNode, &useRadius);
}

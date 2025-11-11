#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "XMLHelpers.h"

#include "multiplayer/P2PMessages.h"
#include "ServerGameLogic.h"

#include "obj_ServerCold.h"

IMPLEMENT_CLASS(obj_ServerCold, "obj_Cold", "Object");
AUTOREGISTER_CLASS(obj_ServerCold);

ColdMgr gColdMngr;

obj_ServerCold::obj_ServerCold()
{
	useRadius = 2.0f;
}

obj_ServerCold::~obj_ServerCold()
{
}

BOOL obj_ServerCold::OnCreate()
{
	parent::OnCreate();

	gColdMngr.RegisterCold(this);
	return 1;
}

// copy from client version
void obj_ServerCold::ReadSerializedData(pugi::xml_node& node)
{
	parent::ReadSerializedData(node);
	pugi::xml_node objNode = node.child("cold_box");
	GetXMLVal("useRadius", objNode, &useRadius);
}

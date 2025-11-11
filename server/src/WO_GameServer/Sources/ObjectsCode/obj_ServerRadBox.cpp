#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "XMLHelpers.h"

#include "multiplayer/P2PMessages.h"
#include "ServerGameLogic.h"

#include "obj_ServerRadBox.h"

IMPLEMENT_CLASS(obj_ServerRadBox, "obj_RadBox", "Object");
AUTOREGISTER_CLASS(obj_ServerRadBox);

RadBoxesMgr gRadBoxesMngr;

obj_ServerRadBox::obj_ServerRadBox()
{
	useRadius = 2.0f;
}

obj_ServerRadBox::~obj_ServerRadBox()
{
}

BOOL obj_ServerRadBox::OnCreate()
{
	parent::OnCreate();

	gRadBoxesMngr.RegisterRadBox(this);
	return 1;
}

// copy from client version
void obj_ServerRadBox::ReadSerializedData(pugi::xml_node& node)
{
	parent::ReadSerializedData(node);
	pugi::xml_node objNode = node.child("rad_box");
	GetXMLVal("useRadius", objNode, &useRadius);
}

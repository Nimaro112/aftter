#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "XMLHelpers.h"

#include "multiplayer/P2PMessages.h"
#include "ServerGameLogic.h"

#include "obj_ServerDmg.h"

IMPLEMENT_CLASS(obj_ServerDmg, "obj_Dmg", "Object");
AUTOREGISTER_CLASS(obj_ServerDmg);

DmgBoxesMgr gDmgBoxesMngr;

obj_ServerDmg::obj_ServerDmg()
{
	useRadius = 2.0f;
}

obj_ServerDmg::~obj_ServerDmg()
{
}

BOOL obj_ServerDmg::OnCreate()
{
	parent::OnCreate();

	gDmgBoxesMngr.RegisterDmgBox(this);
	return 1;
}

// copy from client version
void obj_ServerDmg::ReadSerializedData(pugi::xml_node& node)
{
	parent::ReadSerializedData(node);
	pugi::xml_node objNode = node.child("rad_box");
	GetXMLVal("useRadius", objNode, &useRadius);
}

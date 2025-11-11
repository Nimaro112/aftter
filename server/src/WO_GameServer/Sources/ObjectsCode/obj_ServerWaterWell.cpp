#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "XMLHelpers.h"

#include "multiplayer/P2PMessages.h"
#include "ServerGameLogic.h"

#include "obj_ServerWaterWell.h"

IMPLEMENT_CLASS(obj_ServerWaterWell, "obj_WaterWell", "Object");
AUTOREGISTER_CLASS(obj_ServerWaterWell);

WaterBoxesMgr gWaterBoxesMngr;

obj_ServerWaterWell::obj_ServerWaterWell()
{
	useRadius = 2.0f;
}

obj_ServerWaterWell::~obj_ServerWaterWell()
{
}

BOOL obj_ServerWaterWell::OnCreate()
{
	parent::OnCreate();

	gWaterBoxesMngr.RegisterWaterBox(this);
	return 1;
}

// copy from client version
void obj_ServerWaterWell::ReadSerializedData(pugi::xml_node& node)
{
	parent::ReadSerializedData(node);
	pugi::xml_node objNode = node.child("water_box");
	GetXMLVal("useRadius", objNode, &useRadius);
}

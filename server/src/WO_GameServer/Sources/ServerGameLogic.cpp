#include "r3dPCH.h"
#include "r3d.h"
#include "GameLevel.h"

#include "ServerGameLogic.h"
#include "MasterServerLogic.h"
#include "GameObjects/ObjManag.h"

#include "ObjectsCode/obj_ServerPlayer.h"
#include "ObjectsCode/obj_ServerGravestone.h"
#include "ObjectsCode/obj_ServerGrenade.h"
#include "ObjectsCode/obj_ServerLightMesh.h"
#include "ObjectsCode/sobj_SpawnedItem.h"
#include "ObjectsCode/sobj_DroppedItem.h"
#include "ObjectsCode/obj_ServerUAV.h"
#include "ObjectsCode/sobj_Note.h"
#include "ObjectsCode/Zombies/sobj_Zombie.h"
#include "ObjectsCode/Vehicles/obj_Vehicle.h"
#include "ObjectsCode/obj_ServerPlayerSpawnPoint.h"
#include "ObjectsCode/Zombies/sobj_ZombieSpawn.h"
#include "ObjectsCode/obj_ServerBarricade.h"
#include "ObjectsCode/obj_ServerLockbox.h"
#include "ObjectsCode/obj_ServerDropped.h"
#include "ObjectsCode/obj_ServerARDBOX.h"
#include "ObjectsCode/obj_ServerFarmBlock.h"
#include "ObjectsCode/Missions/MissionManager.h"
#include "ObjectsCode/Missions/MissionTrigger.h"
#include "ObjectsCode/obj_ServerDoor.h"
#include "ObjectsCode/obj_ServerChaosObject.h"
#include "ObjectsCode/obj_ServerItemSpawnPoint.h"

#include "../EclipseStudio/Sources/GameCode/UserProfile.h"
#include "../EclipseStudio/Sources/ObjectsCode/weapons/WeaponArmory.h"
#include "../EclipseStudio/Sources/ObjectsCode/Gameplay/ZombieStates.h"

#include "ServerWeapons/ServerWeapon.h"

#include "../../../../src/EclipseStudio/Sources/backend/WOBackendAPI.h"
#include "ObjectsCode/obj_ServerPostBox.h"
#include "ObjectsCode/obj_ServerBattleArena.h"

#include "NetworkHelper.h"
#include "AsyncFuncs.h"
#include "Async_ServerObjects.h"
#include "Async_ServerState.h"
#include "TeamSpeakServer.h"

// PunkBuster SDK
#ifdef __WITH_PB__
#include "PunkBuster/pbcommon.h"
#endif

ServerGameLogic	gServerLogic;

static const char* configFile = "MasterServer.cfg";

const float RESPAWN_TIME_AIRDROP = 2.3f * 60.0f * 60.0f; // 2:30 hours
const float RESPAWN_TIME_AIRBOMB = 1.0f * 60.0f; // 5 min

const float RESPAWN_TIME_AIRDROPBR = 1.0f * 60.0f; // 15min for br mode
const float RESPAWN_TIME_AIRBOMBBR = 0.3f * 60.0f; // 60 sec for br mode

CVAR_FLOAT(	_glm_SpawnRadius,	 1.0f, "");

extern	__int64 cfg_sessionId;

#include "../EclipseStudio/Sources/Gameplay_Params.h"
	CGamePlayParams		GPP_Data;
	DWORD			GPP_Seed = GetTickCount();	// seed used to xor CRC of gpp_data

extern float getWaterDepthAtPos(const r3dPoint3D& pos);


bool IsNullTerminated(const char* data, int size)
{
  for(int i=0; i<size; i++) {
    if(data[i] == 0)
      return true;
  }

  return false;
}

//
//
//
//
static void preparePacket(const GameObject* from, DefaultPacket* packetData)
{
	r3d_assert(packetData);
	r3d_assert(packetData->EventID >= 0);

	if(from) {
		r3d_assert(from->GetNetworkID());
		//r3d_assert(from->NetworkLocal);

		packetData->FromID = toP2pNetId(from->GetNetworkID());
	} else {
		packetData->FromID = 0; // world event
	}

	return;
}

ServerGameLogic::ServerGameLogic()
{
	curPlayers_ = 0;
	maxLoggedPlayers_ = 0;
	curPeersConnected = 0;

	m_StartGameTime = 0; //battleroyale
	m_StartGameTimeR = 0; //battleroyale
	countTimeFinish = 0; //battleroyale

	if(ginfo_.isBattleRoyaleMap())
		AirDropSpawnTime = r3dGetTime() + RESPAWN_TIME_AIRDROP;

	if(ginfo_.isBattleRoyaleMap())
		AirBombSpawnTime = r3dGetTime() + RESPAWN_TIME_AIRBOMB;

	// init index to players table
	for(int i=0; i<MAX_NUM_PLAYERS; i++) {
		plrToPeer_[i] = NULL;
	}

	// init peer to player table
	for(int i=0; i<MAX_PEERS_COUNT; i++) {
		peers_[i].Clear();
	}

	memset(&netRecvPktSize, 0, sizeof(netRecvPktSize));
	memset(&netSentPktSize, 0, sizeof(netSentPktSize));

	net_lastFreeId    = NETID_OBJECTS_START;
	net_mapLoaded_LastNetID = 0;


	weaponStats_.reserve(128);
}

ServerGameLogic::~ServerGameLogic()
{
}

void ServerGameLogic::Init(const GBGameInfo& ginfo, uint32_t creatorID)
{
	r3dOutToLog("Game: Initializing with %d players\n", ginfo.maxPlayers); CLOG_INDENT;
	r3d_assert(curPlayers_ == 0);
	r3d_assert(curPeersConnected == 0);

	creatorID_	= creatorID;
	ginfo_      = ginfo;
	curPlayers_ = 0;
	curPeersConnected = 0;

	secsWithoutPlayers_ = 0;
	hibernateStarted_   = false;
	gameStartTimeAdminOffset = 0;

	if(ginfo_.isBattleRoyaleMap())
	{
		ginfo_.isBattleRoyaleMode = 0;//battleroyale
		m_StartGameTime = 0;//battleroyale
		m_StartGameTimeR = 0;//battleroyale
		countTimeFinish = 0;//battleroyale
		ForceStarGame = false;//battleroyale
	}


	nextGroupID = 1;

	nextLootboxUpdate_ = r3dGetTime() + u_GetRandom(10*60, 15*60); // spread lootbox update a bit across all servers

	// init game time
	gameStartTime_     = r3dGetTime();

	__int64 utcTime = GetUtcGameTime();
	struct tm* tm = _gmtime64(&utcTime);
	r3d_assert(tm);

	char buf[128];
	asctime_s(buf, sizeof(buf), tm);
	r3dOutToLog("Server time is %s", buf);

	return;
}

void ServerGameLogic::SetNewGameInfo(DWORD flags, int gameTimeLimit)
{
	ginfo_.flags         = flags;
	ginfo_.gameTimeLimit = gameTimeLimit;

	PKT_S2C_SetGameInfoFlags_s n;
	n.gameInfoFlags = ginfo_.flags;
	p2pBroadcastToAll(&n, sizeof(n), true);
}

void ServerGameLogic::SetPVEinfo(DWORD isPVE) // for PVE maps
{
	ginfo_.isPVE    = isPVE;
}

void ServerGameLogic::CreateHost(int port)
{
	r3dOutToLog("Starting server on port %d\n", port);

	g_net.Initialize(this, "p2pNet");
	g_net.CreateHost(port, MAX_PEERS_COUNT);
	//g_net.dumpStats_ = 2;

	return;
}

void ServerGameLogic::Disconnect()
{
	r3dOutToLog("Disconnect server\n");
	g_net.Deinitialize();

	return;
}

void ServerGameLogic::OnGameStart()
{
	/*
	if(1)
	{
		r3dOutToLog("Main World objects\n"); CLOG_INDENT;
		for(GameObject* obj=GameWorld().GetFirstObject(); obj; obj=GameWorld().GetNextObject(obj))
		{
			if(obj->isActive()) r3dOutToLog("obj: %s %s\n", obj->Class->Name.c_str(), obj->Name.c_str());
		}
	}

	if(1)
	{
		extern ObjectManager ServerDummyWorld;
		r3dOutToLog("Temporary World objects\n"); CLOG_INDENT;
		for(GameObject* obj=ServerDummyWorld.GetFirstObject(); obj; obj=ServerDummyWorld.GetNextObject(obj))
		{
			if(obj->isActive()) r3dOutToLog("obj: %s %s\n", obj->Class->Name.c_str(), obj->Name.c_str());
		}
	}*/


	// record last net id
	net_mapLoaded_LastNetID = net_lastFreeId;
	r3dOutToLog("net_mapLoaded_LastNetID: %d\n", net_mapLoaded_LastNetID);

	// start getting server objects
	g_AsyncApiMgr->AddJob(new CJobGetServerObjects());
	// and saved state for objects
	g_AsyncApiMgr->AddJob(new CJobGetSavedServerState());
}

DWORD ServerGameLogic::GetFreeNetId()
{
	DWORD id = net_lastFreeId;

	while(true)
	{
		if(GameWorld().GetNetworkObject(id) == NULL)
		{
			net_lastFreeId = id + 1;
			if(net_lastFreeId > 0xFFF0)
				net_lastFreeId = net_mapLoaded_LastNetID;

			return id;
		}

		id++;
		if(id > 0xFFF0)
			id = net_mapLoaded_LastNetID;

		// check if we looped back
		if(id == net_lastFreeId)
			r3dError("net_lastFreeId overflow, no free slots");
	}
}

void ServerGameLogic::CheckPeerValidity()
{
	const float PEER_CHECK_DELAY  = 0.2f;   // do checks every N seconds
	const float IDLE_PEERS_DELAY  = 5.0f;	// peer have this N seconds to validate itself
	const float LOADING_PEER_PING = 60.0f;  // loading peers must ping API every N seconds to indicate that player still in game
	const float LOADING_MAX_TIME  = 600.0f; // max loading time for client
	const float SECREP1_DELAY     = PKT_C2S_SecurityRep_s::REPORT_PERIOD*4; // x4 time of client reporting time (15sec) to receive security report

	static float nextCheck = -1;
	const float curTime = r3dGetTime();
	if(curTime < nextCheck)
		return;
	nextCheck = curTime + PEER_CHECK_DELAY;
  
	for(int peerId=0; peerId<MAX_PEERS_COUNT; peerId++)
	{
		peerInfo_s& peer = peers_[peerId];
		if(peer.status_ == PEER_FREE)
			continue;
      
		// check againts not validated peers
		if(peer.status_ == PEER_CONNECTED)
		{
			if(curTime < peer.startTime + IDLE_PEERS_DELAY)
				continue;
      
			DisconnectPeer(peerId, false, "no validation, last:%f/%d", peer.lastPacketTime, peer.lastPacketId);
			continue;
		}
    
		// loading peers
		if(peer.status_ == PEER_LOADING)
		{
			if(curTime > peer.startTime + LOADING_MAX_TIME)
			{
				DisconnectPeer(peerId, true, "too long loading");
				continue;
			}
			
			// have profile and joined the game
			if(peer.startGameAns == PKT_S2C_StartGameAns_s::RES_Ok)
			{
				if(peer.nextPingOnLoad < 0)
					peer.nextPingOnLoad = curTime + LOADING_PEER_PING;

				if(curTime > peer.nextPingOnLoad)
				{
					peer.nextPingOnLoad = curTime + LOADING_PEER_PING;
					CJobUserPingGame* job = new CJobUserPingGame(peerId);
					g_AsyncApiMgr->AddJob(job);
				}
			}
			continue;
		}
    
		// for playing peers - check for receiveing security report
		if(peer.player != NULL)
		{
			if(curTime > peer.secRepRecvTime + SECREP1_DELAY)
			{
				DisconnectPeer(peerId, false, "no SecRep, last:%f/%d", peer.lastPacketTime, peer.lastPacketId);
				continue;
			}
		}
	}

	return;
}

void ServerGameLogic::ApiPlayerUpdateWeaponSkins(obj_ServerPlayer* plr)
{
	for( std::map< uint32_t, obj_ServerPlayer::WeaponSkins >::iterator Skinids = plr->WpnSKN.begin();Skinids != plr->WpnSKN.end(); ++Skinids )
	{
		obj_ServerPlayer::WeaponSkins& SkinWpn = Skinids->second;

		if (SkinWpn.itemID != 0)
		{
			//r3dOutToLog("############ GUARDANDO EN LA DB\n");
			for (int i = 0;i<MAX_SKINS_BY_WEAPON;i++)
			{
				if (SkinWpn.SkinsBought[i] != 99)
				{
					CJobChangeWeaponSkins* job = new CJobChangeWeaponSkins(plr);
					job->ItemID = SkinWpn.itemID;
					job->Skin = SkinWpn.SkinsBought[i];
					job->LastSkinUsed = SkinWpn.LastSKINused;
					g_AsyncApiMgr->AddJob(job);
				}
			}
		}
	}
}
void ServerGameLogic::ApiPlayerUpdateChar(obj_ServerPlayer* plr, bool disconnectAfter)
{
	r3d_assert(!plr->inventoryOpActive_);

	// force current GameFlags update
	plr->UpdateGameWorldFlags();

	CJobUpdateChar* job = new CJobUpdateChar(plr);
	job->CharData   = *plr->loadout_;
	job->OldData    = plr->savedLoadout_;
	job->Disconnect = disconnectAfter;
	job->GD_Diff    = plr->profile_.ProfileData.GameDollars - plr->savedGameDollars_;
	job->ResWood    = plr->profile_.ProfileData.ResWood;
	job->ResStone   = plr->profile_.ProfileData.ResStone;
	job->ResMetal   = plr->profile_.ProfileData.ResMetal;
	// add character play time to update data
	job->CharData.Stats.TimePlayed += (int)(r3dGetTime() - plr->startPlayTime_);
	g_AsyncApiMgr->AddJob(job);

	bool needPing=false;
//#ifdef DISABLE_GI_ACCESS_ON_PTE_MAP
//	if(gServerLogic.ginfo_.channel==6) // in PTE game, ping player
//		needPing=true;
//#endif
//#ifdef DISABLE_GI_ACCESS_ON_PTE_STRONGHOLD_MAP
//	if(gServerLogic.ginfo_.channel==6 && gServerLogic.ginfo_.mapId==GBGameInfo::MAPID_AM_Devmap) // in PTE game, ping player
//		needPing=true;
//#endif
//#ifdef DISABLE_GI_ACCESS_FOR_CALI_SERVER
//	if(gServerLogic.ginfo_.mapId==GBGameInfo::MAPID_WZ_California)
//		needPing=true;
//#endif

	if(needPing && !disconnectAfter)
	{
		CJobUserPingGame* pingJob = new CJobUserPingGame(plr->peerId_);
		g_AsyncApiMgr->AddJob(pingJob);
	}
	// replace character saved loadout. if update will fail, we'll disconnect player and keep everything at sync
	plr->savedLoadout_     = *plr->loadout_;
	plr->savedGameDollars_ = plr->profile_.ProfileData.GameDollars;
}

void ServerGameLogic::ApiPlayerLeftGame(DWORD peerId)
{
	const ServerGameLogic::peerInfo_s& peer = GetPeer(peerId);
	r3d_assert(peer.status_ >= PEER_LOADING);

	CJobUserLeftGame* job = new CJobUserLeftGame(peerId);
	job->GameMapId    = ginfo_.mapId;
	job->GameServerId = ginfo_.gameServerId;
	if(peer.player) {
		job->TimePlayed = (int)(r3dGetTime() - peer.player->startPlayTime_);
	}
	g_AsyncApiMgr->AddJob(job);
}

void ServerGameLogic::LogInfo(DWORD peerId, const char* msg, const char* fmt, ...)
{
	char buf[4096];
	va_list ap;
	va_start(ap, fmt);
	StringCbVPrintfA(buf, sizeof(buf), fmt, ap);
	va_end(ap);

	LogCheat(peerId, 0, false, msg, buf);
}

void ServerGameLogic::LogCheat(DWORD peerId, int LogID, int disconnect, const char* msg, const char* fmt, ...)
{
	char buf[4096];
	va_list ap;
	va_start(ap, fmt);
	StringCbVPrintfA(buf, sizeof(buf), fmt, ap);
	va_end(ap);

	const peerInfo_s& peer = GetPeer(peerId);
	DWORD IP = net_->GetPeerIp(peerId);

	//extern int cfg_uploadLogs; // we don't use it anymore
	if(LogID > 0) // always upload cheats
	{
		CJobAddLogInfo* job = new CJobAddLogInfo();
		job->CheatID    = LogID;
		job->CustomerID = peer.CustomerID;
		job->CharID     = peer.CharID;
		job->IP         = IP;
		r3dscpy(job->Gamertag, peer.temp_profile.ProfileData.ArmorySlots[0].Gamertag);
		r3dscpy(job->Msg, msg);
		r3dscpy(job->Data, buf);
		g_AsyncApiMgr->AddJob(job);
  	}

  	const char* screenname = "<NOT_CONNECTED>";
  	if(peer.status_ == PEER_PLAYING)
  		screenname = peer.temp_profile.ProfileData.ArmorySlots[0].Gamertag;

	r3dOutToLog("%s: peer%02d, r:%s %s, CID:%d [%s], ip:%s\n",
		LogID > 0 ? "!!! cheat" : "LogInfo",
		peerId,
		msg, buf,
		peer.CustomerID, screenname,
		inet_ntoa(*(in_addr*)&IP));

	if(disconnect && peer.player)
	{
		// tell client he's cheating.
		// ptumik: no need to make it easier to hack
		//PKT_S2C_CheatWarning_s n2;
		//n2.cheatId = (BYTE)cheatId;
		//p2pSendRawToPeer(peerId, NULL, &n2, sizeof(n2));

		net_->DisconnectPeer(peerId);
		// fire up disconnect event manually, enet might skip if if other peer disconnect as well
		OnNetPeerDisconnected(peerId);
	}

	return;
}

void ServerGameLogic::DisconnectPeer(DWORD peerId, bool cheat, const char* fmt, ...)
{
  char buf[1024];
  va_list ap;
  va_start(ap, fmt);
  StringCbVPrintfA(buf, sizeof(buf), fmt, ap);
  va_end(ap);

  LogCheat(peerId, cheat ? 99 : 0, false, "DisconnectPeer", buf);

  net_->DisconnectPeer(peerId);

  // fire up disconnect event manually, enet might skip if if other peer disconnect as well
  OnNetPeerDisconnected(peerId);
}

void ServerGameLogic::OnNetPeerConnected(DWORD peerId)
{
	// too many connections, do not allow more connects
	if(peerId >= MAX_PEERS_COUNT)
	{
		r3dOutToLog("!!! peer%02d over MAX_PEERS_COUNT connected\n", peerId);
		net_->DisconnectPeer(peerId);
		return;
	}

	if(gMasterServerLogic.shuttingDown_ || gameFinished_)
	{
		r3dOutToLog("peer connected while game is finished or shutting down\n");
		net_->DisconnectPeer(peerId);
		return;
	}

	r3d_assert(ginfo_.maxPlayers > 0);

	//r3dOutToLog("peer%02d connected\n", peerId); CLOG_INDENT;

	peerInfo_s& peer = GetPeer(peerId);
	peer.SetStatus(PEER_CONNECTED);

	curPeersConnected++;
	return;
}

void ServerGameLogic::OnNetPeerDisconnected(DWORD peerId)
{
	// too many connections, do not do anything
	if(peerId >= MAX_PEERS_COUNT)
		return;

	//r3dOutToLog("peer%02d disconnected\n", peerId); CLOG_INDENT;

	peerInfo_s& peer = GetPeer(peerId);

	// debug validation
	switch(peer.status_)
	{
	default:
		r3dError("!!! Invalid status %d in disconnect !!!", peer.status_);
		break;
	case PEER_FREE:
		break;

	case PEER_CONNECTED:
	case PEER_VALIDATED1:
		r3d_assert(peer.player == NULL);
		r3d_assert(peer.playerIdx == -1);
		break;

	case PEER_LOADING:
		r3d_assert(peer.playerIdx != -1);
		r3d_assert(plrToPeer_[peer.playerIdx] != NULL);
		r3d_assert(peer.player == NULL);

		// if we got profile and joined game we need to report that player left to server.
		if(peer.startGameAns == PKT_S2C_StartGameAns_s::RES_Ok)
		{
			ApiPlayerLeftGame(peerId);
		}

		plrToPeer_[peer.playerIdx] = NULL;
		break;

	case PEER_PLAYING:
		r3d_assert(peer.playerIdx != -1);
		r3d_assert(plrToPeer_[peer.playerIdx] != NULL);
		r3d_assert(plrToPeer_[peer.playerIdx] == &peer);
		if(peer.player)
		{
			/*if(peer.player->inWounded && !peer.player->wasDisconnected_ && peer.player->loadout_->Alive !=0) // wounded disabled by now		  
			{
				peer.player->trueDeath = true;
				peer.player->inWounded = false;
				DoKillPlayer(peer.player->sourceObjForinWounded, peer.player, storecat_MELEE, true, false, false);
			}*/
#ifdef VEHICLES_ENABLED
			// check if player is in vehicle, if is, remove player from vehicle
			if (peer.player->IsInVehicle())
			{
				obj_Vehicle* vehicle = obj_Vehicle::GetVehicleById(peer.player->currentVehicleId);
				if (vehicle)
				{
					peer.player->ExitVehicle(true, true, true);
				}
			}
#endif
			gMasterServerLogic.RemovePlayer(peer.playerIdx);

			if(peer.player->loadout_->Alive && !peer.player->wasDisconnected_ && !peer.player->inventoryOpActive_)
			{
				r3dOutToLog("peer%02d player %s is updating his data\n", peerId, peer.player->userName);
				ApiPlayerUpdateChar(peer.player);
			}

			ApiPlayerLeftGame(peerId);

			// report to users
			//if(!(peer.player->profile_.ProfileData.isDevAccount & wiUserProfile::DAA_HIDDEN))
			if (peer.player->m_DevPlayerHide==false)
			{
				PKT_S2C_PlayerNameLeft_s n;
				n.playerIdx = (WORD)peer.playerIdx;

				// send to all, regardless visibility
				for(int i=0; i<MAX_PEERS_COUNT; i++) {
					if(peers_[i].status_ >= PEER_PLAYING && peers_[i].player && i != peerId) {
						net_->SendToPeer(&n, sizeof(n), i, true);
					}
				}
			}

			DeletePlayer(peer.playerIdx, peer.player);
		}

		plrToPeer_[peer.playerIdx] = NULL;
		break;
	}

	if(peer.status_ != PEER_FREE)
	{
		// OnNetPeerDisconnected can fire multiple times, because of forced disconnect
		curPeersConnected--;
	}

	// clear peer status
	peer.Clear();

	return;
}

float ServerGameLogic::getInGameTime()
{
	__int64 gameUtcTime = GetUtcGameTime();
	struct tm* tm = _gmtime64(&gameUtcTime);
	r3d_assert(tm);
	return ((float)tm->tm_hour + (float)tm->tm_min / 59.0f);
}

void ServerGameLogic::OnNetData(DWORD peerId, const r3dNetPacketHeader* packetData, int packetSize)
{
#ifdef __WITH_PB__
	if  ( memcmp ( packetData, "\xff\xff\xff\xffPB_", 7 ) == 0 ) {
		PbSvAddEvent ( PB_EV_PACKET, (int)peerId, packetSize-4, (char*)packetData+4 ) ;
		PbClAddEvent ( PB_EV_PACKET, packetSize-4, (char*)packetData+4 ) ;
		return ;
	}
#endif

	// too many connections, do not do anything
	if(peerId >= MAX_PEERS_COUNT)
		return;

	// we can receive late packets from logically disconnected peer.
	peerInfo_s& peer = GetPeer(peerId);
	if(peer.status_ == PEER_FREE)
		return;

	if(packetSize < sizeof(DefaultPacket))
	{
		DisconnectPeer(peerId, true, "small packetSize %d", packetSize);
		return;
	}
	const DefaultPacket* evt = static_cast<const DefaultPacket*>(packetData);

	// store last packet data for debug
	peer.lastPacketTime = r3dGetTime();
	peer.lastPacketId   = evt->EventID;

	// store received sizes by packets
	if(evt->EventID < 256)
		netRecvPktSize[evt->EventID] += packetSize;

	if(gameFinished_)
	{
		r3dOutToLog("!!! peer%02d got packet %d while game is finished\n", peerId, evt->EventID);
		return;
	}

	GameObject* fromObj = GameWorld().GetNetworkObject(evt->FromID);


	//r3dOutToLog("PACKET(%d): evtID:%d\n", peerId, evt->EventID);

	// pass to world even processor first.
	if(ProcessWorldEvent(fromObj, evt->EventID, peerId, packetData, packetSize)) {
		return;
	}

	if(evt->FromID && fromObj == NULL) {
		DisconnectPeer(peerId, true, "bad event %d from not registered object %d", evt->EventID, evt->FromID);
		return;
	}

	if(fromObj)
	{
		if(IsServerPlayer(fromObj))
		{
			// make sure that sender of that packet is same player on server
			if(((obj_ServerPlayer*)fromObj)->peerId_ != peerId)
			{
				LogCheat(peerId, PKT_S2C_CheatWarning_s::CHEAT_Network, false, "PlayerPeer",
					"peerID: %d, player: %d, packetID: %d",
					peerId, ((obj_ServerPlayer*)fromObj)->peerId_, evt->EventID);
				return;
			}
		}

		if(!fromObj->OnNetReceive(evt->EventID, packetData, packetSize))
		{
			LogCheat(peerId, PKT_S2C_CheatWarning_s::CHEAT_Network, true, "BadObjectEvent",
				"%d for %s %d",
				evt->EventID, fromObj->Name.c_str(), fromObj->GetNetworkID());
		}
		return;
	}

	LogCheat(peerId, PKT_S2C_CheatWarning_s::CHEAT_Network, true, "BadWorldEvent",
		"event %d from %d, obj:%s",
		evt->EventID, evt->FromID, fromObj ? fromObj->Name.c_str() : "NONE");
	return;
}

int ServerGameLogic::getReputationKillEffect(int repFromPlr, int repKilledPlr)
{
	// Reputation system. Splits into three subsystems
	// CIVILIAN LOGIC
	if(repFromPlr > -20 && repFromPlr < 20)
	{
		if(repKilledPlr >= 1000)
			repFromPlr -= 20;
		else if(repKilledPlr >= 500)
			repFromPlr -= 15;
		else if(repKilledPlr >= 250)
			repFromPlr -= 10;
		else if(repKilledPlr >= 80)
			repFromPlr -= 4;
		else if(repKilledPlr >= 20)
			repFromPlr -= 3;
		else if(repKilledPlr >= 10)
			repFromPlr -= 2;
		else if(repKilledPlr <= -1000)
			repFromPlr += 20;
		else if(repKilledPlr <= -600)
			repFromPlr += 15;
		else if(repKilledPlr <= -300)
			repFromPlr += 10;
		else if(repKilledPlr <= -100)
			repFromPlr += 4;
		else if(repKilledPlr <= -20)
			repFromPlr += 3;
		else if(repKilledPlr <= -5)
			repFromPlr += 2;
		else
			repFromPlr -= 1;

	}
	// BANDIT LOGIC (once you are in bandit section, you cannot come up to be neutral)
	else if(repFromPlr <= -20)
	{
		if(repKilledPlr >= 1000)
			repFromPlr -= 20;
		else if(repKilledPlr >= 500)
			repFromPlr -= 15;
		else if(repKilledPlr >= 250)
			repFromPlr -= 10;
		else if(repKilledPlr >= 80)
			repFromPlr -= 4;
		else if(repKilledPlr >= 20)
			repFromPlr -= 3;
		else if(repKilledPlr >= 10)
			repFromPlr -= 2;
		else if(repKilledPlr <= -1000)
			repFromPlr -= 20;
		else if(repKilledPlr <= -600)
			repFromPlr -= 15;
		else if(repKilledPlr <= -300)
			repFromPlr -= 10;
		else if(repKilledPlr <= -100)
			repFromPlr -= 4;
		else if(repKilledPlr <= -20)
			repFromPlr -= 3;
		else if(repKilledPlr <= -5)
			repFromPlr -= 2;
		else
			repFromPlr -= 1;
	}
	// LAWMAN LOGIC
	else if(repFromPlr >= 20)
	{
		if(repKilledPlr <= -1000)
			repFromPlr += 20;
		else if(repKilledPlr <= -600)
			repFromPlr += 15;
		else if(repKilledPlr <= -300)
			repFromPlr += 10;
		else if(repKilledPlr <= -100)
			repFromPlr += 4;
		else if(repKilledPlr <= -20)
			repFromPlr += 3;
		else if(repKilledPlr <= -5)
			repFromPlr += 2;
		// lawman is killing civilian\lawmen, hard penalty depending on how much of a lawmen killer is
		else if(repKilledPlr >= 1000)
			repFromPlr -= 125;
		else if(repKilledPlr >= 500)
			repFromPlr -= 60;
		else if(repKilledPlr >= 250)
			repFromPlr -= 40;
		else if(repKilledPlr >= 80)
			repFromPlr -= 15;
		else if(repKilledPlr >= 20)
			repFromPlr -= 2;
		else if(repKilledPlr >= 10)
			repFromPlr -= 1;
		else
			repFromPlr -= 1;
	}

	return repFromPlr;
}

void ServerGameLogic::DoKillPlayer(GameObject* sourceObj, obj_ServerPlayer* targetPlr, STORE_CATEGORIES weaponCat, bool forced_by_server, bool fromPlayerInAir, bool targetPlayerInAir )
{
	//r3dOutToLog("%s killed by %s, forced: %d\n", targetPlr->userName, sourceObj->Name.c_str(), (int)forced_by_server);

	//if(targetPlr->trueDeath)// wounded disabled by now
	//{
		r3dOutToLog("%s killed by %s, forced: %d\n", targetPlr->userName, sourceObj->Name.c_str(), (int)forced_by_server);
	//}

	/*if(targetPlr->IsInVehicle()) // wounded disabled by now
	{
		targetPlr->inWounded = false;
		targetPlr->trueDeath = true;
	}

	if(ginfo_.isBattleRoyaleMap() || ginfo_.IsDeathMatch())
	{
		targetPlr->inWounded = false;
		targetPlr->trueDeath = true;
	}*/

	if(targetPlr->inBattle)
	{
		targetPlr->inBattle = false;
		PKT_S2C_inBattle_s n;
		n.inBattle = false;
		p2pSendToPeer(targetPlr->peerId_, targetPlr, &n, sizeof(n));
	}

	if(!sourceObj)
		sourceObj = targetPlr;

	// Wounded Sys 
	/*if(!targetPlr->inWounded && !targetPlr->trueDeath) // wounded disabled by now
	{
		obj_ServerPlayer * killedByPlr = ((obj_ServerPlayer*)sourceObj);

		targetPlr->inWounded = true;
		targetPlr->trueDeath = false;
		targetPlr->inWoundedTimer = r3dGetTime();
		if(sourceObj)
			targetPlr->DoKillPlayer(sourceObj, targetPlr, weaponCat, false, fromPlayerInAir, targetPlayerInAir);
		else targetPlr->DoKillPlayer(targetPlr, targetPlr, storecat_MELEE, true, false, false);

		if(!sourceObj)
			sourceObj = targetPlr;
			
		PKT_S2C_KillPlayer_s n;
		n.targetId = toP2pNetId(targetPlr->GetNetworkID());
		n.killerWeaponCat = (BYTE)weaponCat;
		n.forced_by_server = forced_by_server;
		n.trueDeath = targetPlr->trueDeath;
		p2pBroadcastToActive(targetPlr, &n, sizeof(n));

		return;
	}

	if(!targetPlr->trueDeath)
		return;*/

	if(!sourceObj)
		sourceObj = targetPlr;

	obj_ServerPlayer * killedByPlr = ((obj_ServerPlayer*)sourceObj);
	killedByPlr->numKillWithoutDying++;
	char deadplayer[64] = { 0 };
	char killerplayer[64] = { 0 };
	sprintf(deadplayer, targetPlr->userName);
	sprintf(killerplayer, killedByPlr->userName);		
	
	PKT_S2C_KillPlayer_s n;
	r3dscpy(n.victim, deadplayer);
	r3dscpy(n.killer, killerplayer);
	n.Killstreak = killedByPlr->numKillWithoutDying;
	n.lastTimeHitItemID = targetPlr->lastTimeHitItemID;

	if (IsServerPlayer(sourceObj))
	{
		if (targetPlr->profile_.CustomerID == killedByPlr->profile_.CustomerID)//Suicide
		{
			n.KillFlag = 1;	
			r3dscpy(n.killer, deadplayer);
		}
		else if (targetPlr->lastHitBodyPart == 1)// By player headshot
			n.KillFlag = 2;			
		else if (targetPlr->lastTimeHitItemID == WeaponConfig::ITEMID_UnarmedMelee) // By player Unarmed Melee
			n.KillFlag = 3;			
		else//By player		
			n.KillFlag = 4;		
	}
	else if (sourceObj->isObjType(OBJTYPE_Zombie))//By zombie
		n.KillFlag = 5;
	else//Suicide
		n.KillFlag = 1;

	// sent kill packet
	{		     
#ifdef VEHICLES_ENABLED
		if (targetPlr->IsInVehicle())
			targetPlr->ExitVehicle(true, true);
#endif

		//PKT_S2C_KillPlayer_s n;
		n.targetId = toP2pNetId(targetPlr->GetNetworkID());
		n.killerWeaponCat = (BYTE)weaponCat;
		n.forced_by_server = forced_by_server;
		p2pBroadcastToActive(sourceObj, &n, sizeof(n), true);
	}

	/*
	if(!forced_by_server && sourceObj != targetPlr) // do not reward suicides
	{
		DropLootBox(targetPlr);
	}*/


	// Initialize the player's aggressor for gravestone information
	if( sourceObj->isObjType( OBJTYPE_Human ) )
	{
		if( sourceObj != targetPlr )
		{
			r3dscpy( targetPlr->aggressor, ((obj_ServerPlayer*)sourceObj)->userName );
			targetPlr->killedBy = obj_ServerGravestone::KilledBy_Player;
		}
		else
		{
			r3dscpy( targetPlr->aggressor, "" );
			targetPlr->killedBy = obj_ServerGravestone::KilledBy_Self;
		}
	}
	else if( sourceObj->isObjType( OBJTYPE_Zombie ) )
	{
		r3dscpy( targetPlr->aggressor, "" );
		targetPlr->killedBy = obj_ServerGravestone::KilledBy_Zombie;
	}
#ifdef VEHICLES_ENABLED
	else if( sourceObj->isObjType( OBJTYPE_Vehicle ) )
	{
		obj_Vehicle* veh = (obj_Vehicle*)sourceObj;
		obj_ServerPlayer* p = veh->playersInVehicle[0];
		if( p )
		{
			r3dscpy( targetPlr->aggressor, p->userName );
			targetPlr->killedBy = obj_ServerGravestone::KilledBy_Player;
		}
	}
#endif
	targetPlr->DoDeath();

	if(forced_by_server)
		return;

	targetPlr->loadout_->Stats.Deaths++;
	//AddPlayerReward(targetPlr, RWD_Death, "RWD_Death");

	// do not count suicide kills
	if(sourceObj == targetPlr)
		return;

	if(IsServerPlayer(sourceObj))
	{
		// find spawn points within 500 meter radius of this kill and add "danger" to it
		for(int i=0; i<gCPMgr.numControlPoints_; i++)
		{
			BasePlayerSpawnPoint* spawn = gCPMgr.controlPoints_[i];
			for(int j=0; j<spawn->m_NumSpawnPoints; j++)
			{
				float dist = (targetPlr->GetPosition() - spawn->m_SpawnPoints[j].pos).Length();
				if(dist < 500.0f)
				{
					spawn->m_SpawnPoints[j].danger++;
					spawn->m_SpawnPoints[j].lastTimeDangerUpdated = r3dGetTime();
				}
			}
		}
	}

	if(IsServerPlayer(sourceObj))
	{
		obj_ServerPlayer * fromPlr = ((obj_ServerPlayer*)sourceObj);

		if(targetPlr->loadout_->Stats.Reputation > -5)
			fromPlr->loadout_->Stats.KilledSurvivors++;
		else
			fromPlr->loadout_->Stats.KilledBandits++;

		int origRep = fromPlr->loadout_->Stats.Reputation;

		int newRep = getReputationKillEffect(fromPlr->loadout_->Stats.Reputation, targetPlr->loadout_->Stats.Reputation);
		int diffRep = newRep - origRep;
		if(diffRep < 0 && (r3dGetTime() < targetPlr->m_AggressionTimeUntil))
		{
			// do not do anything to fromPlr reputation. Target player had aggression timer on him, so our player is OK killing him without penalty
		}
		else
		{
			fromPlr->loadout_->Stats.Reputation = newRep;
			if(diffRep != 0)
			{
				PKT_S2C_AddScore_s n;
				n.ID = 0;
				n.XP = 0;
				n.GD = 0;
				n.Rep = diffRep;
				p2pSendToPeer(fromPlr->peerId_, fromPlr, &n, sizeof(n));
			}
		}
		RewardKillPlayer(fromPlr, targetPlr, weaponCat);
	}
// 	else if(sourceObj->isObjType(OBJTYPE_GameplayItem) && sourceObj->Class->Name == "obj_ServerAutoTurret")
// 	{
// 		// award kill to owner of the turret
// 		obj_ServerPlayer* turretOwner = IsServerPlayer(GameWorld().GetObject(sourceObj->ownerID));
// 		if(turretOwner)
// 		{
// 			turretOwner->loadout_->Stats.Kills++;
// 		}
// 	}

	return;
}

void ServerGameLogic::RewardKillPlayer(obj_ServerPlayer* fromPlr, obj_ServerPlayer* targetPlr, STORE_CATEGORIES dmgCat)
{
	r3d_assert(fromPlr);
	r3d_assert(targetPlr);
	//r3dOutToLog("RewardKillPlayer %s->%s\n", fromPlr->userName, targetPlr->userName); CLOG_INDENT;
	
	if (fromPlr->numKillWithoutDying == 15)
		AddPlayerReward(fromPlr, RWD_Kill15NotDying);
	else if (fromPlr->numKillWithoutDying == 25)
		AddPlayerReward(fromPlr, RWD_Kill25NotDying);
	else if (fromPlr->numKillWithoutDying == 50)
	{
		AddPlayerReward(fromPlr, RWD_Kill50NotDying);
		LogCheat(fromPlr->peerId_, PKT_S2C_CheatWarning_s::CHEAT_Kills, true, "HighKills 50");
	}
	else if (fromPlr->numKillWithoutDying == 100)
	{
		AddPlayerReward(fromPlr, RWD_Kill100NotDying);
		LogCheat(fromPlr->peerId_, PKT_S2C_CheatWarning_s::CHEAT_Kills, true, "HighKills 100");
	}

	int gotKillReward = 0;
	if (targetPlr->loadout_->ClanID != fromPlr->loadout_->ClanID)
	{
		// count kill streaks
		if ((r3dGetTime() - fromPlr->LastEnemyKillTime) < GPP_Data.c_fKillStreakTimeout)
			fromPlr->Killstreaks++;
		else{
			fromPlr->Killstreaks = 1;
			if (dmgCat != storecat_GRENADE && (targetPlr->lastHitBodyPart == 1 && dmgCat != storecat_MELEE))
				AddPlayerReward(fromPlr, RWD_PlayerKill);	
			
		}
		if (fromPlr->Killstreaks == 2)
		{
			AddPlayerReward(fromPlr, RWD_Killstreak2);
			gotKillReward++;			
		}
		else if (fromPlr->Killstreaks == 3)
		{
			AddPlayerReward(fromPlr, RWD_Killstreak3);
			gotKillReward++;
		}
		else if (fromPlr->Killstreaks == 4)
		{
			AddPlayerReward(fromPlr, RWD_Killstreak4);
			gotKillReward++;
		}
		else if (fromPlr->Killstreaks == 5)
		{
			AddPlayerReward(fromPlr, RWD_Killstreak5);
			gotKillReward++;
		}
		else if (fromPlr->Killstreaks == 6)
		{
			AddPlayerReward(fromPlr, RWD_Killstreak6);
			gotKillReward++;
		}
		else if (fromPlr->Killstreaks == 7)
		{
			AddPlayerReward(fromPlr, RWD_Killstreak7);
			gotKillReward++;
		}
		else if (fromPlr->Killstreaks == 8)
		{
			AddPlayerReward(fromPlr, RWD_Killstreak8);
			gotKillReward++;
		}

		// update timer
		fromPlr->LastEnemyKillTime = r3dGetTime();
	}

	if (dmgCat == storecat_GRENADE)
	{
		AddPlayerReward(fromPlr, RWD_ExplosionKill);
	}
	else if (targetPlr->lastHitBodyPart == 1 && dmgCat != storecat_MELEE) // headshot
	{
		AddPlayerReward(fromPlr, RWD_Headshot);
	}	
}

// make sure this function is the same on client: AI_Player.cpp bool canDamageTarget(const GameObject* obj)
bool ServerGameLogic::CanDamageThisObject(const GameObject* targetObj)
{
	if(IsServerPlayer(targetObj))
	{
		return true;
	}
	else if(targetObj->isObjType(OBJTYPE_Zombie))
	{
		return true;
	}
	else if(targetObj->isObjType(OBJTYPE_UAV))
	{
		
		// can only damage when it's not destroyed.
		// otherwise we can send hit packet to already destroyed object on client
		obj_ServerUAV* targetUav = (obj_ServerUAV*)targetObj;

		if(targetUav->state_ == obj_ServerUAV::UAV_Killed)
			return false;
		return true;
	}
#ifdef VEHICLES_ENABLED
	else if (targetObj->isObjType(OBJTYPE_Vehicle))
	{
		return true;
	}
#endif
	else if(targetObj->Class->Name == "obj_LightMesh")
	{
		return true;
	}
	else if(targetObj->isObjType(OBJTYPE_Barricade))
	{
		return true;
	}
	else if(targetObj->Class->Name == "obj_Door")
	{
		return true;
	}
	else if(targetObj->Class->Name == "obj_ChaosObject")
	{
		return true;
	}

	return false;
}

void ServerGameLogic::ApplyDamage(GameObject* fromObj, GameObject* targetObj, const r3dPoint3D& dmgPos, float damage, bool force_damage, STORE_CATEGORIES damageSource, uint32_t dmgItemID, bool canApplyBleeding)
{
	r3d_assert(fromObj);
	r3d_assert(targetObj);

	if(IsServerPlayer(fromObj))
	{
		obj_ServerPlayer* fromPlr = (obj_ServerPlayer*)fromObj;
		if(damageSource == storecat_MELEE)
		{
			if(fromPlr->loadout_->Skills[CUserSkills::SKILL_Weapons1])
				damage *= 1.05f;
			if(fromPlr->loadout_->Skills[CUserSkills::SKILL_Weapons3])
				damage *= 1.07f;
			if(fromPlr->loadout_->Skills[CUserSkills::SKILL_Weapons6])
				damage *= 1.10f;
		}
	}

	if(IsServerPlayer(targetObj))
	{
		ApplyDamageToPlayer(fromObj, (obj_ServerPlayer*)targetObj, dmgPos, damage, -1, 0, force_damage, damageSource, dmgItemID, 0, canApplyBleeding);
		return;
	}
	else if(targetObj->isObjType(OBJTYPE_UAV))
	{
		obj_ServerUAV* targetUav = (obj_ServerUAV*)targetObj;
		targetUav->DoDamage(damage, fromObj->GetNetworkID());
		return;
	}
	else if(targetObj->isObjType(OBJTYPE_Zombie))
	{
		ApplyDamageToZombie(fromObj, targetObj, dmgPos, damage, -1, 0, force_damage, damageSource, dmgItemID);
		return;
	}
	else if(targetObj->Class->Name == "obj_LightMesh")
	{
		obj_ServerLightMesh* lightM = (obj_ServerLightMesh*)targetObj;
		if(lightM->bLightOn)
		{
			lightM->bLightOn = false;
			lightM->SyncLightStatus(-1);
		}

		return;
	}
	else if(targetObj->isObjType(OBJTYPE_Barricade)) // damage for barricade here!
	{
		obj_ServerBarricade* shield = (obj_ServerBarricade*)targetObj;
		shield->DoDamage(damage);
		return;
	}
	else if(targetObj->Class->Name == "obj_Door")
	{
		obj_Door* Door = (obj_Door*)targetObj;
			Door->DamageDoor(damage);
		return;
	}
	else if(targetObj->Class->Name == "obj_ChaosObject")
	{
		obj_ChaosObject* Chaos = (obj_ChaosObject*)targetObj;
			Chaos->DoDamage(damage);
		return;
	}
	else if (targetObj->isObjType(OBJTYPE_Vehicle)) // damge with grenade for vehicle here!
	{
		if (!fromObj->isObjType(OBJTYPE_Vehicle))
		{
			obj_Vehicle* vehicle = (obj_Vehicle*)targetObj;
			vehicle->ApplyDamage(fromObj, damage, storecat_GRENADE);
		}

		return;
	}

	r3dOutToLog("!!! error !!! was trying to damage object %s [%s]\n", targetObj->Name.c_str(), targetObj->Class->Name.c_str());
}

// return true if hit was registered, false otherwise
// state is grabbed from the dynamics.  [0] is from player in the air, [1] is target player in the air
bool ServerGameLogic::ApplyDamageToPlayer(GameObject* fromObj, obj_ServerPlayer* targetPlr, const r3dPoint3D& dmgPos, float damage, int bodyBone, int bodyPart, bool force_damage, STORE_CATEGORIES damageSource, uint32_t dmgItemID, int airState, bool canApplyBleeding)
{
	r3d_assert(fromObj);
	r3d_assert(targetPlr);

	if(targetPlr->loadout_->Alive == 0)
		return false;

	if (targetPlr->m_DevPlayerHide == true)
		return false;

	if (m_StartGameTime > r3dGetTime() && ginfo_.isBattleRoyaleMap() || m_StartGameTime == 0 && ginfo_.isBattleRoyaleMap()) //battleroyale
		return false;

	if(r3dGetTime() < targetPlr->m_SpawnProtectedUntil)
		return false;

	if(targetPlr->loadout_->isPVE == 1 && fromObj->Class->Name != "obj_Zombie") // for PVE maps
		return false;

	// can't damage players in safe zones (postbox now act like that)
	if((targetPlr->loadout_->GameFlags & wiCharDataFull::GAMEFLAG_NearPostBox) && !force_damage)
		return false;
	// double check that we can't kill player that operating with global inventory
	if(targetPlr->inventoryOpActive_)
		return false;
	// player's in the same group cannot damage each other (except for grenades)
	if(targetPlr->groupID!= 0 && IsServerPlayer(fromObj) && IsServerPlayer(fromObj)->groupID==targetPlr->groupID && damageSource!=storecat_GRENADE
		&& fromObj!=targetPlr // if you are in group and damage is coming from player itself (for example falling damage) then ignore group
		)
		return false;

	damage = targetPlr->ApplyDamage(damage, fromObj, bodyPart, damageSource, dmgItemID, canApplyBleeding);

	if(fromObj->Class->Name == "obj_Zombie")
	{
		PKT_S2C_ZombieAttack_s n;
		n.targetId = toP2pNetId(targetPlr->GetNetworkID());
		p2pBroadcastToActive(fromObj, &n, sizeof(n));

#ifdef VEHICLES_ENABLED
		if (!targetPlr->IsInVehicle())
		{
#endif
			extern float _zai_ChanceOfInfecting;
			extern float _zai_InfectStrength;
			if(u_GetRandom(0.0f, 100.0f) < _zai_ChanceOfInfecting) // chance of infecting
			{
				targetPlr->loadout_->Toxic += _zai_InfectStrength;
				targetPlr->loadout_->Toxic = R3D_CLAMP(targetPlr->loadout_->Toxic, 0.0f, 100.0f);
			}
#ifdef VEHICLES_ENABLED
		}
#endif


	}
	else
	{
		// send damage packet, originating from the firing dude
		PKT_S2C_Damage_s a;
		a.dmgPos = dmgPos;
		a.targetId = toP2pNetId(targetPlr->GetNetworkID());
		a.damage   = R3D_MIN((BYTE)damage, (BYTE)255);
		a.dmgType = damageSource;
		a.bodyBone = bodyBone;
		p2pBroadcastToActive(fromObj, &a, sizeof(a));
	}

	// check if we killed player
	if(targetPlr->loadout_->Health <= 0)
	{
		bool fromPlayerInAir = ((airState & 0x1) != 0);
		bool targetPlayerInAir = ((airState & 0x2) != 0);

		DoKillPlayer(fromObj, targetPlr, damageSource, false, fromPlayerInAir, targetPlayerInAir);
#ifdef MISSIONS
		if( IsServerPlayer(fromObj) && ((obj_ServerPlayer*)fromObj)->m_MissionsProgress )
			((obj_ServerPlayer*)fromObj)->m_MissionsProgress->AddKill( targetPlr, dmgItemID );
#endif
	}

	return true;
}

bool ServerGameLogic::ApplyDamageToZombie(GameObject* fromObj, GameObject* targetZombie, const r3dPoint3D& dmgPos, float damage, int bodyBone, int bodyPart, bool force_damage, STORE_CATEGORIES damageSource, uint32_t dmgItemID)
{
	r3d_assert(fromObj);
	r3d_assert(targetZombie && targetZombie->isObjType(OBJTYPE_Zombie));

	if ( IsServerPlayer(fromObj))
	{
		if (IsServerPlayer(fromObj)->m_DevPlayerHide == true)
			return false;
	}

	// relay to zombie logic
	obj_Zombie* z = (obj_Zombie*)targetZombie;
	bool killed = z->ApplyDamage(fromObj, damage, bodyPart, damageSource);

	if(IsServerPlayer(fromObj) && killed)
	{
		IsServerPlayer(fromObj)->loadout_->Stats.KilledZombies++;

#ifdef MISSIONS
		if( ((obj_ServerPlayer*)fromObj)->m_MissionsProgress )
			IsServerPlayer(fromObj)->m_MissionsProgress->AddKill( z, dmgItemID );
#endif
	}

	return true;
}

#ifdef VEHICLES_ENABLED
bool ServerGameLogic::ApplyDamageToVehicle(GameObject* fromObj, GameObject* targetVehicle, const r3dPoint3D& dmgPos, float damage, bool forceDamage, STORE_CATEGORIES damageSource, uint32_t damageItemId)
{
	r3d_assert(fromObj);
	r3d_assert(targetVehicle && targetVehicle->isObjType(OBJTYPE_Vehicle));

	obj_Vehicle* vehicle = (obj_Vehicle*)targetVehicle;
	bool destroyed = vehicle->ApplyDamage(fromObj, damage, damageSource);
	if (IsServerPlayer(fromObj) && destroyed)
	{
		// todo - add player kill
	}

	return true;
}
#endif

void ServerGameLogic::RelayPacket(DWORD peerId, const GameObject* from, const DefaultPacket* packetData, int packetSize, bool guaranteedAndOrdered)
{
	if(!from)
	{
		r3dError("RelayPacket !from, event: %d", packetData->EventID);
	}
	const INetworkHelper* nh = const_cast<GameObject*>(from)->GetNetworkHelper();

	for(int i=0; i<MAX_PEERS_COUNT; i++)
	{
		if(peers_[i].status_ >= PEER_PLAYING && i != peerId)
		{
			if(!nh->GetVisibility(i))
			{
				continue;
			}

			net_->SendToPeer(packetData, packetSize, i, guaranteedAndOrdered);
			netSentPktSize[packetData->EventID] += packetSize;
		}
	}

	return;
}

void ServerGameLogic::p2pBroadcastToActive(const GameObject* from, DefaultPacket* packetData, int packetSize, bool guaranteedAndOrdered)
{
	if(!from)
	{
		r3dError("p2pBroadcastToActive !from, event: %d", packetData->EventID);
	}
	const INetworkHelper* nh = const_cast<GameObject*>(from)->GetNetworkHelper();

	preparePacket(from, packetData);

	for(int i=0; i<MAX_PEERS_COUNT; i++)
	{
		if(peers_[i].status_ >= PEER_PLAYING)
		{
			if(!nh->GetVisibility(i))
			{
				continue;
			}
			net_->SendToPeer(packetData, packetSize, i, guaranteedAndOrdered);
			netSentPktSize[packetData->EventID] += packetSize;
		}
	}

	return;
}

void ServerGameLogic::p2pBroadcastToAll(DefaultPacket* packetData, int packetSize, bool guaranteedAndOrdered)
{
	preparePacket(NULL, packetData);

	for(int i=0; i<MAX_PEERS_COUNT; i++)
	{
		if(peers_[i].status_ >= PEER_VALIDATED1)
		{
			net_->SendToPeer(packetData, packetSize, i, guaranteedAndOrdered);
			netSentPktSize[packetData->EventID] += packetSize;
		}
	}

	return;
}

obj_ServerPlayer* ServerGameLogic::FindPlayer(DWORD peerId)
{
    obj_ServerPlayer* plr = NULL;

    for(int i = 0; i < MAX_NUM_PLAYERS; i++)
    {
        plr = gServerLogic.GetPlayer(i);
        if(!plr)
            continue;

		if (plr->peerId_ == peerId)
            return plr;
    }

    return NULL;
}

void ServerGameLogic::p2pSendToPeer(DWORD peerId, const GameObject* from, DefaultPacket* packetData, int packetSize, bool guaranteedAndOrdered)
{
	if(!from)
	{
		r3dError("p2pSendToPeer !from, event: %d", packetData->EventID);
	}

	const peerInfo_s& peer = GetPeer(peerId);
	if(peer.status_ >= PEER_PLAYING)
	{
		const INetworkHelper* nh = const_cast<GameObject*>(from)->GetNetworkHelper();
		if(!nh->GetVisibility(peerId))
		{
			return;
		}

		preparePacket(from, packetData);
		net_->SendToPeer(packetData, packetSize, peerId, guaranteedAndOrdered);
		netSentPktSize[packetData->EventID] += packetSize;
	}
}

void ServerGameLogic::p2pSendRawToPeer(DWORD peerId, DefaultPacket* packetData, int packetSize, bool guaranteedAndOrdered)
{
	const peerInfo_s& peer = GetPeer(peerId);
	r3d_assert(peer.status_ != PEER_FREE);

	preparePacket(NULL, packetData);
	net_->SendToPeer(packetData, packetSize, peerId, guaranteedAndOrdered);
	netSentPktSize[packetData->EventID] += packetSize;
}

void ServerGameLogic::InformZombiesAboutSound(const obj_ServerPlayer* plr, const ServerWeapon* wpn)
{
	for(GameObject* obj = GameWorld().GetFirstObject(); obj; obj = GameWorld().GetNextObject(obj))
	{
		if(obj->isObjType(OBJTYPE_Zombie))
			((obj_Zombie*)obj)->SenseWeaponFire(plr, wpn);
	}
}

void ServerGameLogic::InformZombiesAboutSoundItemID(const obj_ServerPlayer* plr, int ItemID)
{
	for(GameObject* obj = GameWorld().GetFirstObject(); obj; obj = GameWorld().GetNextObject(obj))
	{
		if(obj->isObjType(OBJTYPE_Zombie))
			((obj_Zombie*)obj)->SenseItemSound(plr, ItemID);
	}
}

void ServerGameLogic::InformZombiesAboutGrenadeSound(const obj_ServerGrenade* grenade, bool isExplosion)
{
	for(std::list<obj_Zombie*>::iterator it = obj_Zombie::s_ListOfAllActiveZombies.begin(); it != obj_Zombie::s_ListOfAllActiveZombies.end(); ++it)
	{
		(*it)->SenseGrenadeSound(grenade, isExplosion);
	}
}

#ifdef VEHICLES_ENABLED
void ServerGameLogic::InformZombiesAboutVehicleExplosion(const obj_Vehicle* vehicle)
{
	for(std::list<obj_Zombie*>::iterator it = obj_Zombie::s_ListOfAllActiveZombies.begin(); it != obj_Zombie::s_ListOfAllActiveZombies.end(); ++it)
	{
		(*it)->SenseVehicleExplosion(vehicle);
	}
}
#endif

void ServerGameLogic::InformZombiesAboutChaosExplosion(const obj_ChaosObject* chaos)
{
	for(std::list<obj_Zombie*>::iterator it = obj_Zombie::s_ListOfAllActiveZombies.begin(); it != obj_Zombie::s_ListOfAllActiveZombies.end(); ++it)
	{
		(*it)->SenseChaosExplosion(chaos);
	}
}

wiStatsTracking ServerGameLogic::GetRewardData(obj_ServerPlayer* plr, EPlayerRewardID rewardID)
{
	r3d_assert(g_GameRewards);
	const CGameRewards::rwd_s& rwd = g_GameRewards->GetRewardData(rewardID);
	if(!rwd.IsSet)
	{
		LogInfo(plr->peerId_, "GetReward", "%d not set", rewardID);
		return wiStatsTracking();
	}

	wiStatsTracking stat;
	stat.RewardID = (int)rewardID;
	stat.GP       = 0;

	if(plr->profile_.ProfileData.PremiumAcc > 0)
	{
		if(rwd.GD_SOFT>0)
			stat.GD = rwd.GD_SOFT + u_random(38);// random GD, default GD = 40
		else
			stat.GD = rwd.GD_SOFT;

		if(rwd.XP_SOFT>0)
			stat.XP = rwd.XP_SOFT + u_random(38);// random XP, default XP = 40
		else
			stat.XP = rwd.XP_SOFT;
	}
	else if(ginfo_.isBattleRoyaleMap())
	{
		if(rwd.GD_SOFT>0)
			stat.GD = rwd.GD_SOFT + u_random(2);// random GD, default GD = 20
		else
			stat.GD = rwd.GD_SOFT;

		if(rwd.XP_SOFT>0)
			stat.XP = rwd.XP_SOFT + u_random(2);// random XP, default XP = 20
		else
			stat.XP = rwd.XP_SOFT;
	}
	else
	{
		if(rwd.GD_SOFT>0)
			stat.GD = rwd.GD_SOFT + u_random(18);// random GD, default GD = 20
		else
			stat.GD = rwd.GD_SOFT;

		if(rwd.XP_SOFT>0)
			stat.XP = rwd.XP_SOFT + u_random(18);// random XP, default XP = 20
		else
			stat.XP = rwd.XP_SOFT;
	}
	
	return stat;
}

void ServerGameLogic::AddPlayerReward(obj_ServerPlayer* plr, EPlayerRewardID rewardID)
{
	wiStatsTracking stat = GetRewardData(plr, rewardID);
	if(stat.RewardID == 0)
		return;

	const CGameRewards::rwd_s& rwd = g_GameRewards->GetRewardData(rewardID);
	AddDirectPlayerReward(plr, stat, rwd.Name.c_str());
}

void ServerGameLogic::AddDirectPlayerReward(obj_ServerPlayer* plr, const wiStatsTracking& in_rwd, const char* rewardName)
{
	// add reward to player
	wiStatsTracking rwd2 = plr->AddReward(in_rwd);
	int xp = rwd2.XP;
	int gp = rwd2.GP;
	int gd = rwd2.GD;
	if(xp == 0 && gp == 0 && gd == 0)
		return;

	r3dOutToLog("reward: %s got %dxp %dgp %dgd RWD_%s\n", plr->userName, xp, gp, gd, rewardName ? rewardName : "");

	// send it to him
	PKT_S2C_AddScore_s n;
	n.ID = (WORD)in_rwd.RewardID;
	n.XP = R3D_CLAMP(xp, -30000, 30000);
	n.GD = (WORD)gd;
	n.Rep = 0;
	p2pSendToPeer(plr->peerId_, plr, &n, sizeof(n));

	return;
}

IMPL_PACKET_FUNC(ServerGameLogic, PKT_C2S_ValidateConnectingPeer)
{
	// reply back with our version
	PKT_C2S_ValidateConnectingPeer_s n1;
	n1.protocolVersion = P2PNET_VERSION;
	n1.sessionId       = 0;
	p2pSendRawToPeer(peerId, &n1, sizeof(n1));

	if(n.protocolVersion != P2PNET_VERSION)
	{
		DisconnectPeer(peerId, false, "Protocol Version mismatch");
		return;
	}
	extern __int64 cfg_sessionId;
	if(n.sessionId != cfg_sessionId)
	{
		DisconnectPeer(peerId, true, "Wrong sessionId");
		return;
	}

	// switch to Validated state
	peerInfo_s& peer = GetPeer(peerId);
	peer.SetStatus(PEER_VALIDATED1);

	return;
}

#ifdef VEHICLES_ENABLED
IMPL_PACKET_FUNC(ServerGameLogic, PKT_C2S_VehicleEnter)
{
	obj_Vehicle* vehicle = obj_Vehicle::GetVehicleById(n.vehicleId);

	if (!vehicle || !vehicle->isActive())
		return;

	// does the vehicle exist or have room?
	PKT_S2C_VehicleEntered_s packet;

	if (vehicle->GetDurability() <= 0 || vehicle->IsVehicleSeatsFull())
		packet.isSuccess = false;
	else
		packet.isSuccess = true;

	// put the requesting player in the vehicle
	obj_ServerPlayer* player = IsServerPlayer(GetPeer(peerId).player);
	if (!player || player->loadout_->Alive == 0)
		return;

	// make sure player is within 5 meters of vehicle
	if ((player->GetPosition() - vehicle->GetPosition()).Length() > 6.0f)
		 packet.isSuccess = false;

	if (packet.isSuccess)
	{
		player->EnterVehicle(vehicle);

		packet.networkId = toP2pNetId(vehicle->GetNetworkID());
		packet.vehicleId = vehicle->vehicleId;
		packet.seat = (BYTE)player->seatPosition;
		packet.curFuel = vehicle->GetFuel();

		// let all of the clients know, this player has entered a vehicle
		gServerLogic.p2pBroadcastToActive(GetPeer(peerId).player, &packet, sizeof(packet), true);
	}
	else // there is no available vehicle or seat
	{
		p2pSendToPeer(peerId, GetPeer(peerId).player, &packet, sizeof(packet));
	}

	return;
}

IMPL_PACKET_FUNC(ServerGameLogic, PKT_C2S_VehicleExit)
{
	obj_Vehicle* vehicle = obj_Vehicle::GetVehicleById(n.vehicleId);
	if (!vehicle)
		return;

	obj_ServerPlayer* player = IsServerPlayer(GetPeer(peerId).player);
	if (!player)
		return;

	player->ExitVehicle(true);
}

IMPL_PACKET_FUNC(ServerGameLogic, PKT_C2S_VehicleHitTarget)
{
	if (!fromObj || !fromObj->isObjType(OBJTYPE_Vehicle))
		return;

	obj_Vehicle* vehicle = (obj_Vehicle*)fromObj;
	if (!vehicle)
		return;

	obj_ServerPlayer* player = vehicle->playersInVehicle[0];
	if (!player) // we have no driver and we have no last driver.. fail
		return;

	GameObject* gameObj = GameWorld().GetNetworkObject(n.targetId);
	if (!gameObj)
		return;

	float distanceCheck = (vehicle->GetSpeed() + 8.0f) * 2.0f; // temporary solution.
	float distance = (vehicle->GetPosition() - gameObj->GetPosition()).Length();
	if (distance > distanceCheck)
	{
		gServerLogic.LogCheat(peerId, PKT_S2C_CheatWarning_s::CHEAT_VehicleHitTarget, true, "VehicleHitTarget",
			"dist: %f; distCheck: %f", distance, distanceCheck);
		return;
	}

	if (gameObj->isObjType(OBJTYPE_Zombie))
	{
		obj_Zombie* zombie = (obj_Zombie*)gameObj;
		if (zombie->ZombieHealth <= 0)
			return;

		ApplyDamageToZombie(player, gameObj, vehicle->GetPosition(), zombie->ZombieHealth + 1, 1, 1, true, storecat_Vehicle, vehicle->vehicleId);

		vehicle->SetDurability(-10);

		if (zombie->ZombieHealth >= 0.0f)
		{
			PKT_S2C_VehicleHitTargetFail_s n;
			n.targetId = n.targetId;
			p2pBroadcastToActive(vehicle, &n, sizeof(n));
		}
	}
	else if (gameObj->isObjType(OBJTYPE_Human))
	{
		obj_ServerPlayer* hitPlayer = IsServerPlayer(gameObj);
		if (hitPlayer)
		{
			if (hitPlayer->loadout_->GameFlags & wiCharDataFull::GAMEFLAG_NearPostBox || hitPlayer->loadout_->Health <= 0)
				return;

			ApplyDamageToPlayer(player, (obj_ServerPlayer*)gameObj, vehicle->GetPosition(), 200.0f, 1, 2, true, storecat_Vehicle, vehicle->vehicleId);

			// WHAAAT? Zombies and players have the same skeleton config, so why we decrease more for players? another hammerCrap.
			//vehicle->SetDurability(-700, false);
			vehicle->SetDurability(-100, false);
			if (hitPlayer->loadout_->Alive == 1)
			{
				PKT_S2C_VehicleHitTargetFail_s n;
				n.targetId = n.targetId;
				p2pBroadcastToActive(vehicle, &n, sizeof(n));
			}
		}
	}
}

IMPL_PACKET_FUNC(ServerGameLogic, PKT_C2S_VehicleStopZombie)
{
	GameObject* gameObj = GameWorld().GetNetworkObject(n.targetId);
	if (!gameObj || !gameObj->isObjType(OBJTYPE_Zombie))
		return;

	((obj_Zombie*)gameObj)->StopNavAgent();
}
#endif

obj_ServerPlayer* ServerGameLogic::CreateNewPlayer(DWORD peerId, const r3dPoint3D& spawnPos, float spawnDir, bool needsSpawnProtection, bool isFastLoad)
{
	peerInfo_s& peer = GetPeer(peerId);
	const int playerIdx = peer.playerIdx;

	r3d_assert(playerIdx >= 0 && playerIdx < MAX_NUM_PLAYERS);
	r3d_assert(peer.startGameAns == PKT_S2C_StartGameAns_s::RES_Ok);

	// store game session id
	peer.temp_profile.ProfileData.ArmorySlots[0].GameServerId = ginfo_.gameServerId;
	peer.temp_profile.ProfileData.ArmorySlots[0].GameMapId    = ginfo_.mapId;
	peer.temp_profile.ProfileData.ArmorySlots[0].isPVE		  = ginfo_.isPVE; // for PVE maps

	// client server sync point
	bool wipeInventory=false;
	bool giveLoadout = false;
//#ifdef DISABLE_GI_ACCESS_ON_PTE_MAP
//	if(ginfo_.channel == 6) // PTE map, wipe inventory and give one flashlight only
//		wipeInventory=true;
//#endif
//#ifdef DISABLE_GI_ACCESS_ON_PTE_STRONGHOLD_MAP
//	if(ginfo_.channel == 6 && ginfo_.mapId==GBGameInfo::MAPID_AM_Devmap) // PTE map, wipe inventory and give one flashlight only
//		wipeInventory=true;
//#endif
//#ifdef DISABLE_GI_ACCESS_FOR_CALI_SERVER
//	if(gServerLogic.ginfo_.mapId==GBGameInfo::MAPID_WZ_California)
//		wipeInventory=true;
//#endif

	if(wipeInventory)
	{
		for(int i=0; i<wiCharDataFull::CHAR_MAX_BACKPACK_SIZE; ++i)
			peer.temp_profile.ProfileData.ArmorySlots[0].Items[i].Reset();

		if (!giveLoadout) // bambi mode
		{
			wiInventoryItem flashLightItem; //Item 
			flashLightItem.InventoryID = 0; 
			flashLightItem.itemID = 101306; //Item Code
			flashLightItem.quantity = 1; //Piece
			peer.temp_profile.ProfileData.ArmorySlots[0].Items[wiCharDataFull::CHAR_LOADOUT_ITEM2] = flashLightItem; ////Place the Slot
		}
		else // dev event loadout mode
		{
			GiveDevEventLoadout(peerId);
		}
	}
	// end

	// create player
	char name[128];
	//sprintf(name, "player%02d", playerIdx);
	sprintf(name, "%s", peer.temp_profile.ProfileData.ArmorySlots[0].Gamertag);
	obj_ServerPlayer* plr = (obj_ServerPlayer*)srv_CreateGameObject("obj_ServerPlayer", name, spawnPos);

	// add to peer-to-player table (need to do before player creation, because of network objects visibility check)
	r3d_assert(plrToPeer_[playerIdx] != NULL);
	r3d_assert(plrToPeer_[playerIdx]->player == NULL);
	plrToPeer_[playerIdx]->player = plr;
	// mark that we're active
	peer.player = plr;

	// fill player info
	plr->m_PlayerRotation = spawnDir;
	if(needsSpawnProtection)
		plr->m_SpawnProtectedUntil = r3dGetTime() + 99999999999999999999999999.0f; // 30 seconds of spawn protection
	plr->peerId_      = peerId;
	plr->SetNetworkID(playerIdx + NETID_PLAYERS_START);
	plr->NetworkLocal = false;
#ifdef MISSIONS
	peer.temp_profile.ProfileData.ArmorySlots[0].missionsProgress->m_player = plr;
#endif
	plr->SetProfile(peer.temp_profile);
	plr->isFastLoad = isFastLoad;
	plr->GetDBSkins(peer.temp_profile.ProfileData);// get Skins Bought
	plr->OnCreate();

	// from this point we do expect security report packets
	peer.secRepRecvTime = r3dGetTime();
	peer.secRepGameTime = -1;
	peer.secRepRecvAccum = 0;

	plrList_[curPlayers_] = plr;
	curPlayers_++;
	maxLoggedPlayers_ = R3D_MAX(maxLoggedPlayers_, curPlayers_);

	// report player to masterserver
	gMasterServerLogic.AddPlayer(peer.playerIdx, peer.CustomerID, plr->loadout_);

	// report joined player name to all users
	//if(!(plr->profile_.ProfileData.isDevAccount & wiUserProfile::DAA_HIDDEN))
	//if (plr->m_DevPlayerHide==false)
	//{
		PKT_S2C_PlayerNameJoined_s n;
		n.playerIdx = (WORD)playerIdx;
		r3dscpy(n.gamertag, plr->userName);
		n.reputation = plr->loadout_->Stats.Reputation;
		n.ClanID = plr->loadout_->ClanID;
		n.flags = 0;
		if(plr->profile_.ProfileData.AccountType == 0) // legend
			n.flags |= 1;
		if(plr->profile_.ProfileData.isDevAccount & wiUserProfile::DAA_DEV_ICON)
			n.flags |= 2;
		if(plr->profile_.ProfileData.AccountType == 54) // punisher
			n.flags |= 4;
		if(plr->profile_.ProfileData.PremiumAcc > 0)
			n.flags |= 8;
		if (plr->m_DevPlayerHide==true)
			n.m_DevPlayerHide=1;
		else
			n.m_DevPlayerHide=0;

		// send to all, regardless visibility, excluding us
		for(int i=0; i<MAX_PEERS_COUNT; i++) 
		{
			if(peers_[i].status_ >= PEER_PLAYING && peers_[i].player && i != peerId) {
				net_->SendToPeer(&n, sizeof(n), i, true);
			}
		}
	//}

	// report list of current players to user, including us
	{
		for(int i=0; i<MAX_PEERS_COUNT; i++) {
			if(peers_[i].status_ >= PEER_PLAYING && peers_[i].player) {
				//if(!(peers_[i].player->profile_.ProfileData.isDevAccount & wiUserProfile::DAA_HIDDEN))
				//if (peers_[i].player->m_DevPlayerHide==false)
				//{
					PKT_S2C_PlayerNameJoined_s n;
					n.playerIdx = (WORD)peers_[i].playerIdx;
					r3dscpy(n.gamertag, peers_[i].player->userName);
					n.reputation = peers_[i].player->loadout_->Stats.Reputation;
					n.ClanID = peers_[i].player->loadout_->ClanID;
					n.flags = 0;
					if(peers_[i].player->profile_.ProfileData.AccountType == 0) // legend
						n.flags |= 1;
					if(peers_[i].player->profile_.ProfileData.isDevAccount & wiUserProfile::DAA_DEV_ICON)
						n.flags |= 2;
					if(peers_[i].player->profile_.ProfileData.AccountType == 54) // punisher
						n.flags |= 4;
					if(peers_[i].player->profile_.ProfileData.PremiumAcc > 0)
						n.flags |= 8;
					if (peers_[i].player->m_DevPlayerHide==true)
						n.m_DevPlayerHide=1;
					else
						n.m_DevPlayerHide=0;

					net_->SendToPeer(&n, sizeof(n), peerId, true);

					// send call for help if any
					if(r3dGetTime() < (peers_[i].player->lastCallForHelp+600))
					{
						PKT_S2C_CallForHelpEvent_s n3;
						n3.playerIdx = (WORD)peers_[i].playerIdx;
						r3dscpy(n3.distress, peers_[i].player->CallForHelp_distress);
						r3dscpy(n3.reward, peers_[i].player->CallForHelp_reward);
						n3.timeLeft = (peers_[i].player->lastCallForHelp+600) - r3dGetTime();
						n3.locX = peers_[i].player->lastCallForHelpLocation.x;
						n3.locZ = peers_[i].player->lastCallForHelpLocation.z;

						net_->SendToPeer(&n3, sizeof(n3), peerId, true);
					}
				//}
			}
		}
	}

	return plr;
}

void ServerGameLogic::DeletePlayer(int playerIdx, obj_ServerPlayer* plr)
{
	r3d_assert(plr);

	r3d_assert(playerIdx == (plr->GetNetworkID() - NETID_PLAYERS_START));
	r3dOutToLog("DeletePlayer: %s, playerIdx: %d\n", plr->userName, playerIdx);

	// kill UAV of player
	if(plr->uavId_ != invalidGameObjectID)
	{
		obj_ServerUAV* uav = (obj_ServerUAV*)GameWorld().GetObject(plr->uavId_);
		if (uav == NULL) {
			plr->uavId_ = invalidGameObjectID;
			plr->uavRequested_ = false;
		}
		else {
			PKT_S2C_DestroyNetObject_s n2;
			n2.spawnID = gp2pnetid_t(uav->GetNetworkID());
			gServerLogic.p2pBroadcastToActive(plr, &n2, sizeof(n2));

			uav->setActiveFlag(0);
		}	
	}

	ResetNetObjVisData(plr);

	// if player in group
	if(plr->groupID!=0)
	{
		leavePlayerFromGroup(plr);
	}

	// remove player invites from other players
	cleanPlayerGroupInvites(plr);

	// send player destroy
	{
		PKT_S2C_DestroyNetObject_s n;
		n.spawnID = gp2pnetid_t(plr->GetNetworkID());
		p2pBroadcastToActive(plr, &n, sizeof(n));
	}

#if defined(MISSIONS)
	for(int i = 0; i < wiUserProfile::MAX_LOADOUT_SLOTS; ++i)
	{
		if( plr->profile_.ProfileData.ArmorySlots[ i ].missionsProgress )
			delete plr->profile_.ProfileData.ArmorySlots[ i ].missionsProgress;
	}
#endif

	// mark for deletion
	plr->setActiveFlag(0);
	plr->wasDeleted = true;

	// search for that player in flat array and remove it from there.
	for(int i=0; i<curPlayers_; ++i)
	{
		if(plrList_[i] == plr)
		{
			memmove(plrList_ + i, plrList_ + i + 1, (curPlayers_ - i - 1) * sizeof(plrList_[0]));
			break;
		}
	}
	r3d_assert(curPlayers_ > 0);
	curPlayers_--;

	return;
}

// sync with CMasterUserServer::IsAdmin(DWORD CustomerID)
bool ServerGameLogic::IsAdmin(DWORD CustomerID)
{
	switch(CustomerID)
	{
		case 1000021: //Cuenta Admin Local
		case 1000022: //Bombillo
			return true;
	}

	return false;
}

IMPL_PACKET_FUNC(ServerGameLogic, PKT_C2S_JoinGameReq)
{
	DWORD ip = net_->GetPeerIp(peerId);
	r3dOutToLog("peer%02d PKT_C2S_JoinGameReq: CID:%d, ip:%s\n",
		peerId, n.CustomerID, inet_ntoa(*(in_addr*)&ip));
	CLOG_INDENT;

	if(n.CustomerID == 0 || n.SessionID == 0 || n.CharID == 0)
	{
		gServerLogic.LogCheat(peerId, PKT_S2C_CheatWarning_s::CHEAT_Protocol, true, "JoinGame",
			"%d %d %d", n.CustomerID, n.SessionID, n.CharID);
		return;
	}

	// check if that player was kicked before and ban time isn't expired
	{
		TKickedPlayers::iterator it = kickedPlayers_.find(n.CustomerID);
		if(it != kickedPlayers_.end() && r3dGetTime() < it->second)
		{
			PKT_S2C_CustomKickMsg_s n2;
			r3dscpy(n2.msg, "You are temporarily banned from this server");
			p2pSendRawToPeer(peerId, &n2, sizeof(n2));

			PKT_S2C_JoinGameAns_s n;
			n.success   = 0;
			n.playerIdx = 0;
			n.ownerCustomerID = 0;
			p2pSendRawToPeer(peerId, &n, sizeof(n));

			DisconnectPeer(peerId, false, "temporary ban");
			return;
		}
	}

	// GetFreePlayerSlot
	int playerIdx = -1;
	int slotsToScan = IsAdmin(n.CustomerID) ? MAX_NUM_PLAYERS : ginfo_.maxPlayers;
	for(int i=0; i<slotsToScan; i++)
	{
		if(plrToPeer_[i] == NULL)
		{
			playerIdx = i;
			break;
		}
	}

	if(playerIdx == -1)
	{
		PKT_S2C_JoinGameAns_s n;
		n.success   = 0;
		n.playerIdx = 0;
		n.ownerCustomerID = 0;
		p2pSendRawToPeer(peerId, &n, sizeof(n));

		DisconnectPeer(peerId, false, "game is full"); //@ investigate why it's happening
		return;
	}

	peerInfo_s& peer = GetPeer(peerId);
	r3d_assert(peer.player == NULL);
	peer.SetStatus(PEER_LOADING);
	peer.playerIdx      = playerIdx;
	peer.CustomerID     = n.CustomerID;
	peer.SessionID      = n.SessionID;
	peer.CharID         = n.CharID;
	peer.nextPingOnLoad = -1;
	peer.voicePeerId    = (u_random(0xFFFFFFFE) & 0xFFFF0000) | playerIdx;
	peer.voiceClientId  = 0;

	// add to players table
	r3d_assert(plrToPeer_[playerIdx] == NULL);
	plrToPeer_[playerIdx] = &peer;

	// send join answer to peer
	{
		PKT_S2C_JoinGameAns_s n;
		n.success      = 1;
		n.playerIdx    = playerIdx;
		n.ownerCustomerID = creatorID_;
		n.gameInfo     = ginfo_;
		n.gameTime     = GetUtcGameTime();
		n.voiceId      = peer.voicePeerId;
		n.voicePwd     = TSServer_GetPassword();

		p2pSendRawToPeer(peerId, &n, sizeof(n));
	}

	// send game parameters to peer
	{
		PKT_S2C_SetGamePlayParams_s n;
		n.GPP_Data = GPP_Data;
		n.GPP_Seed = GPP_Seed;
		p2pSendRawToPeer(peerId, &n, sizeof(n));
	}

	// start thread for profile loading
	r3d_assert(peer.startGameAns == 0);
	g_AsyncApiMgr->AddJob(new CJobProcessUserJoin(peerId));

	return;
}

IMPL_PACKET_FUNC(ServerGameLogic, PKT_C2S_StartGameReq)
{
	peerInfo_s& peer = GetPeer(peerId);
	r3d_assert(peer.playerIdx != -1);
	r3d_assert(peer.player == NULL);
	r3d_assert(peer.status_ == PEER_LOADING);

	r3dOutToLog("peer%02d PKT_C2S_StartGameReq, playerIdx:%d startGameAns: %d, lastNetID: %d\n", peerId, peer.playerIdx, peer.startGameAns, n.lastNetID); CLOG_INDENT;

	if(n.lastNetID != net_mapLoaded_LastNetID)
	{
		PKT_S2C_StartGameAns_s n2;
		n2.result = PKT_S2C_StartGameAns_s::RES_UNSYNC;
		p2pSendRawToPeer(peerId, &n2, sizeof(n2));
		DisconnectPeer(peerId, true, "netID doesn't match %d vs %d", n.lastNetID, net_mapLoaded_LastNetID);
		return;
	}

	// check for default values, just in case
	r3d_assert(0 == PKT_S2C_StartGameAns_s::RES_Unactive);
	r3d_assert(1 == PKT_S2C_StartGameAns_s::RES_Ok);
	switch(peer.startGameAns)
	{
		// we have profile, process
		case PKT_S2C_StartGameAns_s::RES_Ok:
			break;

		// no profile loaded yet
		case PKT_S2C_StartGameAns_s::RES_Unactive:
		{
			// we give 60sec to finish getting profile per user
			if(r3dGetTime() > (peer.startTime + 60.0f))
			{
				PKT_S2C_StartGameAns_s n;
				n.result = PKT_S2C_StartGameAns_s::RES_Failed;
				p2pSendRawToPeer(peerId, &n, sizeof(n));
				DisconnectPeer(peerId, true, "timeout getting profile data");
			}
			else
			{
				// still pending
				PKT_S2C_StartGameAns_s n;
				n.result = PKT_S2C_StartGameAns_s::RES_Pending;
				p2pSendRawToPeer(peerId, &n, sizeof(n));
			}
			return;
		}

		default:
		{
			PKT_S2C_StartGameAns_s n;
			n.result = (BYTE)peer.startGameAns;
			p2pSendRawToPeer(peerId, &n, sizeof(n));
			DisconnectPeer(peerId, true, "StarGameReq: %d", peer.startGameAns);
			return;
		}
	}

	// we have player profile, put it in game
	r3d_assert(peer.startGameAns == PKT_S2C_StartGameAns_s::RES_Ok);

  	// we must have only one profile with correct charid
	if(peer.temp_profile.ProfileData.NumSlots != 1 ||
	   peer.temp_profile.ProfileData.ArmorySlots[0].LoadoutID != peer.CharID)
	{
		PKT_S2C_StartGameAns_s n;
		n.result = PKT_S2C_StartGameAns_s::RES_Failed;
		p2pSendRawToPeer(peerId, &n, sizeof(n));

		DisconnectPeer(peerId, true, "CharID mismatch %d vs %d", peer.CharID, peer.temp_profile.ProfileData.ArmorySlots[0].LoadoutID);
		return;
	}

	// and it should be alive.
	wiCharDataFull& loadout = peer.temp_profile.ProfileData.ArmorySlots[0];
	if(loadout.Alive == 0)
	{
		PKT_S2C_StartGameAns_s n;
		n.result = PKT_S2C_StartGameAns_s::RES_Failed;
		p2pSendRawToPeer(peerId, &n, sizeof(n));

		DisconnectPeer(peerId, true, "CharID %d is DEAD", peer.CharID);
		return;
	}

	if(peer.temp_profile.ProfileData.isDevAccount==0) // no check for Devs
	{
		if(ginfo_.gameTimeLimit > int(peer.temp_profile.ProfileData.TimePlayed/60/60))
		{
			PKT_S2C_StartGameAns_s n;
			n.result = PKT_S2C_StartGameAns_s::RES_Failed;
			p2pSendRawToPeer(peerId, &n, sizeof(n));

			DisconnectPeer(peerId, true, "Gametime limit on server");
			return;
		}
	}

	if(peer.temp_profile.ProfileData.isDevAccount==0) // only for Devs
	{
		if(ginfo_.mapId == GBGameInfo::MAPID_AM_Devmap)
		{
			PKT_S2C_StartGameAns_s n;
			n.result = PKT_S2C_StartGameAns_s::RES_Failed;
			p2pSendRawToPeer(peerId, &n, sizeof(n));

			DisconnectPeer(peerId, true, "Only Dev users to this map");
			return;
		}
	}

	if(ginfo_.channel==1 && peer.temp_profile.ProfileData.ArmorySlots[0].Hardcore == 0)
	{
		PKT_S2C_StartGameAns_s n;
		n.result = PKT_S2C_StartGameAns_s::RES_Failed;
		p2pSendRawToPeer(peerId, &n, sizeof(n));

		DisconnectPeer(peerId, true, "Hardcore is required");
		return;
	}

	if(ginfo_.channel!=1 && peer.temp_profile.ProfileData.ArmorySlots[0].Hardcore == 1)
	{
		PKT_S2C_StartGameAns_s n;
		n.result = PKT_S2C_StartGameAns_s::RES_Failed;
		p2pSendRawToPeer(peerId, &n, sizeof(n));

		DisconnectPeer(peerId, true, "Normal Player is required");
		return;
	}

	// trial accs allowed to connect only to trial servers
	if(ginfo_.channel!=1 && peer.temp_profile.IsTrialAccount())
	{
		PKT_S2C_StartGameAns_s n;
		n.result = PKT_S2C_StartGameAns_s::RES_Failed;
		p2pSendRawToPeer(peerId, &n, sizeof(n));

		DisconnectPeer(peerId, true, "Trial accs not allowed");
		return;
	}

	if(ginfo_.channel==1 && !peer.temp_profile.IsTrialAccount())
	{
		if(peer.temp_profile.ProfileData.isDevAccount>0)
		{
			// allow devs to connect to trial servers
		}
		else
		{
			PKT_S2C_StartGameAns_s n;
			n.result = PKT_S2C_StartGameAns_s::RES_Failed;
			p2pSendRawToPeer(peerId, &n, sizeof(n));

			DisconnectPeer(peerId, true, "Trial acc is required to join trial server");
			return;
		}
	}

	if(ginfo_.channel==4 && peer.temp_profile.ProfileData.PremiumAcc == 0)
	{
		PKT_S2C_StartGameAns_s n;
		n.result = PKT_S2C_StartGameAns_s::RES_Failed;
		p2pSendRawToPeer(peerId, &n, sizeof(n));

		DisconnectPeer(peerId, true, "Premium required");
		return;
	}

	if(ginfo_.isBattleRoyaleMap() && countTimeFinish)
	{
		PKT_S2C_StartGameAns_s n;
		n.result = PKT_S2C_StartGameAns_s::RES_Failed;
		p2pSendRawToPeer(peerId, &n, sizeof(n));
		r3dOutToLog("peer connected while game has started!\n");
		DisconnectPeer(peerId, true, "game has started!");
		return;
	}

	peer.SetStatus(PEER_PLAYING);

	// if by some fucking unknown method you appeared at 0,0,0 - pretend he was dead, so it'll spawn at point
	/*if(loadout.Alive == 1 && loadout.GameMapId != GBGameInfo::MAPID_ServerTest && loadout.GamePos.Length() < 10)
	{
		LogCheat(peerId, PKT_S2C_CheatWarning_s::CHEAT_Data, false, "ZeroSpawn", "%f %f %f", loadout.GamePos.x, loadout.GamePos.y, loadout.GamePos.z);
		loadout.Alive = 2;
	}*/

	// get spawn position
	r3dPoint3D spawnPos;
	float      spawnDir;
	bool needsSpawnProtection = false;
	//r3dOutToLog("!!! Loading Pos: %.3f %.3f %.3f\n", loadout.GamePos.x, loadout.GamePos.y, loadout.GamePos.z);
	GetStartSpawnPosition(loadout, &spawnPos, &spawnDir, needsSpawnProtection);
	if(peer.isServerHop) // if player server hopped, then he will need spawnProtection no matter what, as he will spawn in random location
		needsSpawnProtection = true;

	// adjust for positions that is under ground because of geometry change
	if(Terrain)
	{
		float y1 = Terrain->GetHeight(spawnPos);
		if(spawnPos.y <= y1)
			spawnPos.y = y1 + 0.1f;
	}
	//r3dOutToLog("!!! Final Pos: %.3f %.3f %.3f\n", spawnPos.x, spawnPos.y, spawnPos.z);

	// create that player
	obj_ServerPlayer* plr = CreateNewPlayer(peerId, spawnPos, spawnDir, needsSpawnProtection, n.FastLoad);

	// send current weapon info to player
	SendWeaponsInfoToPlayer(peerId);

	// send answer to start game
	{
		PKT_S2C_StartGameAns_s n;
		n.result = PKT_S2C_StartGameAns_s::RES_Ok;
		p2pSendRawToPeer(peerId, &n, sizeof(n));
	}
	// send message to client that he was server hopped
	if(peer.isServerHop)
	{
		PKT_C2C_ChatMessage_s n2;
		n2.FromID = toP2pNetId(plr->GetNetworkID());
		n2.msgChannel = 1; // global
		n2.userFlag   = 2; // mark as dev, so color is red
		r3dscpy(n2.gamertag, "[SERVER]");
		r3dscpy(n2.msg, "You were teleported to a safe location because of quick change of game servers");
		p2pSendToPeer(plr->peerId_, plr, &n2, sizeof(n2), 1);
	}

#ifdef MISSIONS
	if( Mission::g_pMissionMgr )
	{
		Mission::g_pMissionMgr->SendActiveMissionsToPlayer( *plr );

		// Sync client mission progress with what's been loaded, and now copied to the player on the server.
		Mission::MissionsProgress* mp = plr->profile_.ProfileData.ArmorySlots[0].missionsProgress;
		if( mp )
		{
			//r3dOutToLog("Syncing Remote Missions Progress for player %s\n", plr->Name);
			mp->SyncRemoteMissionsProgress();
		}
		else
		{
			r3dOutToLog("!!! Missions Progress not set, unable to sync Remote Missions Progress for player %s\n", plr->Name);
		}
	}
#endif

	return;
}

bool ServerGameLogic::CheckForPlayersAround(const r3dPoint3D& pos, float dist)
{
	float distSq = dist * dist;
	for(int i=0; i<curPlayers_; i++)
	{
		const obj_ServerPlayer* plr = plrList_[i];
		if((plr->GetPosition() - pos).LengthSq() < distSq)
			return true;
	}

	return false;
}

void ServerGameLogic::GetStartSpawnPosition(const wiCharDataFull& loadout, r3dPoint3D* pos, float* dir, bool& needsSpawnProtection)
{
	needsSpawnProtection = false;
	bool isBattleRoyale = false;

	// if no map assigned yet, or new map, or newly created character (alive == 3) or joining PTE map
	if(loadout.GameMapId == 0 || loadout.GameMapId != ginfo_.mapId || loadout.Alive == 3)
	{
		needsSpawnProtection = true;
		
		if(ginfo_.isBattleRoyaleMap() && isBattleRoyale == true)
		{
			GetSpawnPositionBattleRoyale(loadout.GamePos, pos, dir);
			// move spawn pos at radius
			pos->x += u_GetRandom(-_glm_SpawnRadius, _glm_SpawnRadius);
			pos->z += u_GetRandom(-_glm_SpawnRadius, _glm_SpawnRadius);
			//r3dOutToLog("revived at position %f %f %f\n", pos->x, pos->y, pos->z);
			//return;
		}
		if(!ginfo_.isBattleRoyaleMap() && isBattleRoyale == false)
		{
			GetSpawnPositionNewPlayer(loadout.GamePos, pos, dir);
			// move spawn pos at radius
			pos->x += u_GetRandom(-_glm_SpawnRadius, _glm_SpawnRadius);
			pos->z += u_GetRandom(-_glm_SpawnRadius, _glm_SpawnRadius);
			//r3dOutToLog("new spawn at position %f %f %f\n", pos->x, pos->y, pos->z);
			return;
		}
	}

	if(ginfo_.isBattleRoyaleMap())
	{
		needsSpawnProtection = true;

		GetSpawnPositionBattleRoyale(loadout.GamePos, pos, dir);
		// move spawn pos at radius
		pos->x += u_GetRandom(-_glm_SpawnRadius, _glm_SpawnRadius);
		pos->z += u_GetRandom(-_glm_SpawnRadius, _glm_SpawnRadius);
		//r3dOutToLog("revived at position %f %f %f\n", pos->x, pos->y, pos->z);
		return;
	}

	// alive at current map
	// revived (alive == 2) - spawn to closest spawn point
	if(loadout.GameMapId && loadout.Alive == 2 || loadout.Alive == 1)
	{
		needsSpawnProtection = true;

		if(ginfo_.isBattleRoyaleMap() && isBattleRoyale == true)
		{
			GetSpawnPositionBattleRoyale(loadout.GamePos, pos, dir);
			// move spawn pos at radius
			pos->x += u_GetRandom(-_glm_SpawnRadius, _glm_SpawnRadius);
			pos->z += u_GetRandom(-_glm_SpawnRadius, _glm_SpawnRadius);
			//r3dOutToLog("revived at position %f %f %f\n", pos->x, pos->y, pos->z);
			//return;
		}
		if(!ginfo_.isBattleRoyaleMap() && isBattleRoyale == false)
		{
			GetSpawnPositionAfterDeath2(loadout.GamePos, pos, dir);
			// move spawn pos at radius
			pos->x += u_GetRandom(-_glm_SpawnRadius, _glm_SpawnRadius);
			pos->z += u_GetRandom(-_glm_SpawnRadius, _glm_SpawnRadius);
			//r3dOutToLog("revived at position %f %f %f\n", pos->x, pos->y, pos->z);
			return;
		}
	}

	r3dOutToLog("%d %d %d\n", loadout.GameMapId, loadout.Alive, ginfo_.mapId);
	r3d_assert(false && "GetStartSpawnPosition");

}

void ServerGameLogic::GetSpawnPositionNewPlayer(const r3dPoint3D& GamePos, r3dPoint3D* pos, float* dir)
{
	if(gCPMgr.numControlPoints_ == 0)
	{
		r3dOutToLog("!!!!!!!!!!!! THERE IS NO CONTROL POINTS !!!!!!!\n");
		*pos = r3dPoint3D(0, 0, 0);
		*dir = 0;
		return;
	}

	int idx1 = u_random(gCPMgr.numControlPoints_);
	r3d_assert(idx1 < gCPMgr.numControlPoints_);
	const BasePlayerSpawnPoint* spawn = gCPMgr.controlPoints_[idx1];
	spawn->getSpawnPoint(*pos, *dir);
	return;
}

void ServerGameLogic::GetSpawnPositionAfterDeath(const r3dPoint3D& GamePos, r3dPoint3D* pos, float* dir)
{
	if(gCPMgr.numControlPoints_ == 0)
	{
		r3dError("!!!!!!!!!!!! THERE IS NO CONTROL POINT !!!!!!!\n");
		*pos = r3dPoint3D(0, 0, 0);
		*dir = 0;
		return;
	}

	// set defaults in case if all spawnpoints is in 100m range (server test map)
	*pos = GamePos;
	*dir = 0;

	// NOTE: this function is used from another thread from CJobProcessUserJoin::DetectServerHop. DO NOT add any statics here
	// spawn to closest point (but don't spawn player at almost exactly same spawn if he died to close to spawn location)
	float minDist = FLT_MAX;
	for(int i=0; i<gCPMgr.numControlPoints_; i++)
	{
		const BasePlayerSpawnPoint* spawn = gCPMgr.controlPoints_[i];
		for(int j=0; j<spawn->m_NumSpawnPoints; j++)
		{
			float dist = (GamePos - spawn->m_SpawnPoints[j].pos).Length();
			if(dist < 100.0f) // to make sure that player will not keep spawning at the same location
				continue;
			if(dist < minDist)
			{
				*pos    = spawn->m_SpawnPoints[j].pos;
				*dir    = spawn->m_SpawnPoints[j].dir;
				minDist = dist;
			}
		}
	}

	return;
}

void ServerGameLogic::GetSpawnPositionAfterDeath2(const r3dPoint3D& GamePos, r3dPoint3D* pos, float* dir)
{
	if(gCPMgr.numControlPoints_ == 0)
	{
		r3dError("!!!!!!!!!!!! THERE IS NO CONTROL POINT !!!!!!!\n");
		*pos = r3dPoint3D(0, 0, 0);
		*dir = 0;
		return;
	}

	// find points in 1km radius and select one with least amount of "danger" in it
	static BasePlayerSpawnPoint::node_s listOfPoints[1000];
	int numPoints = 0;

	for(int i=0; i<gCPMgr.numControlPoints_; i++)
	{
		const BasePlayerSpawnPoint* spawn = gCPMgr.controlPoints_[i];
		for(int j=0; j<spawn->m_NumSpawnPoints; j++)
		{
			float dist = (GamePos - spawn->m_SpawnPoints[j].pos).Length();
			if(dist < 1000.0f) // to make sure that player will not keep spawning at the same location
			{
				listOfPoints[numPoints++] = spawn->m_SpawnPoints[j];
				if(numPoints==1000) // reached maximum
					break;
			}
		}
	}

	if(numPoints == 0) // fallback to old method
	{
		return GetSpawnPositionAfterDeath(GamePos, pos, dir);
	}

	*pos    = listOfPoints[0].pos;
	*dir    = listOfPoints[0].dir;

	// and now find closest point to our location
	int curDanger = 99999999;
	float minDist = FLT_MAX;
	for(int i=0; i<numPoints; ++i)
	{
		if(listOfPoints[i].danger > curDanger)
			continue;

		float dist = (GamePos - listOfPoints[i].pos).Length();
		if(dist < 100.0f) // to make sure that player will not keep spawning at the same location
			continue;
		if(dist < minDist && listOfPoints[i].danger <= curDanger) // select smaller distance and lowest danger
		{
			*pos    = listOfPoints[i].pos;
			*dir    = listOfPoints[i].dir;
			minDist = dist;
			curDanger = listOfPoints[i].danger;
		}
	}

	return;
}

r3dPoint3D ServerGameLogic::AdjustPositionToFloor(const r3dPoint3D& pos)
{
	// do this in a couple of steps. firstly try +0.25, +1, then +5, then +50, then absolute +1000
	PxSweepHit hit;
	PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK, 0, 0, 0), PxSceneQueryFilterFlag::eSTATIC);
	PxSphereGeometry sphere(0.25f);

	if (!g_pPhysicsWorld->PhysXScene->sweepSingle(sphere, PxTransform(PxVec3(pos.x, pos.y+0.5f, pos.z), PxQuat(0,0,0,1)), PxVec3(0, -1, 0), 2000.0f, PxSceneQueryFlag::eIMPACT, hit, filter))
		if (!g_pPhysicsWorld->PhysXScene->sweepSingle(sphere, PxTransform(PxVec3(pos.x, pos.y+1.0f, pos.z), PxQuat(0,0,0,1)), PxVec3(0, -1, 0), 2000.0f, PxSceneQueryFlag::eIMPACT, hit, filter))
			if (!g_pPhysicsWorld->PhysXScene->sweepSingle(sphere, PxTransform(PxVec3(pos.x, pos.y+5.0f, pos.z), PxQuat(0,0,0,1)), PxVec3(0, -1, 0), 2000.0f, PxSceneQueryFlag::eIMPACT, hit, filter))
				if (!g_pPhysicsWorld->PhysXScene->sweepSingle(sphere, PxTransform(PxVec3(pos.x, pos.y+50.0f, pos.z), PxQuat(0,0,0,1)), PxVec3(0, -1, 0), 2000.0f, PxSceneQueryFlag::eIMPACT, hit, filter))
					if (!g_pPhysicsWorld->PhysXScene->sweepSingle(sphere, PxTransform(PxVec3(pos.x, 1000.0f, pos.z), PxQuat(0,0,0,1)), PxVec3(0, -1, 0), 2000.0f, PxSceneQueryFlag::eIMPACT, hit, filter))
					{
						r3dOutToLog("!! there is no floor under %f %f %f\n", pos.x, pos.y, pos.z);
						return pos;
					}

	return r3dPoint3D(hit.impact.x, hit.impact.y + 0.01f, hit.impact.z);
}

//
// every server network object must call this function in their OnCreate() - TODO: think about better way to automatize that
//
void ServerGameLogic::NetRegisterObjectToPeers(GameObject* netObj)
{
	r3d_assert(netObj->GetNetworkID());

	// scan for all peers and see if they within distance of this object
	INetworkHelper* nh = netObj->GetNetworkHelper();
	for(int peerId=0; peerId<MAX_PEERS_COUNT; peerId++)
	{
		const peerInfo_s& peer = peers_[peerId];
		if(peer.player == NULL)
			continue;

		float dist = (peer.player->GetPosition() - netObj->GetPosition()).LengthSq();
		if(dist < nh->distToCreateSq)
		{
#ifdef _DEBUG
r3dOutToLog("NETHELPER: %s: on create - entered visibility of network object %d %s\n", peer.player->userName, netObj->GetNetworkID(), netObj->Name.c_str());
#endif
			r3d_assert(nh->PeerVisStatus[peerId] == 0);
			nh->PeerVisStatus[peerId] = 1;

			int packetSize = 0;
			DefaultPacket* packetData = nh->NetGetCreatePacket(&packetSize);
			if(packetData)
			{
				preparePacket(netObj, packetData);
				net_->SendToPeer(packetData, packetSize, peerId, true);
				netSentPktSize[packetData->EventID] += packetSize;
			}
		}
	}

}

void ServerGameLogic::UpdateNetObjVisData(DWORD peerId, obj_ServerPlayer* pl, GameObject* netObj)
{
	if (netObj->Class->Name != "obj_Building")
	{
		if (!netObj->GetNetworkID()) return;
		r3d_assert(netObj->GetNetworkID());
	}
	r3d_assert(!(netObj->ObjFlags & OBJFLAG_JustCreated)); // object must be fully created at this moment
	
	const peerInfo_s& peer = GetPeer(peerId);
	float dist = (pl->GetPosition() - netObj->GetPosition()).LengthSq();

	INetworkHelper* nh = netObj->GetNetworkHelper();
	if(nh->PeerVisStatus[peerId] == 0)
	{
		if (netObj->Class->Name != "obj_Building")
		{
			if(dist < nh->distToCreateSq)
			{
				if (netObj->Class->Name != "obj_Building")
				{
					nh->PeerVisStatus[peerId] = 1;

					int packetSize = 0;
					DefaultPacket* packetData = nh->NetGetCreatePacket(&packetSize);
					if(packetData)
					{
						if (netObj->Class->Name == "obj_Building")
						{
							PKT_S2C_CreateScenario_s n;
							n.angle = netObj->GetRotationVector();
							n.col = false;
							if(netObj->ObjFlags & OBJFLAG_PlayerCollisionOnly)
								n.col = true;

							n.pos = netObj->GetPosition();
							r3dscpy(n.fname,netObj->FileName.c_str());
							n.spawnID = 0;
							net_->SendToPeer(&n, sizeof(n), peerId, false);
						}
						else
						{
							preparePacket(netObj, packetData);
							net_->SendToPeer(packetData, packetSize, peerId, true);
							netSentPktSize[packetData->EventID] += packetSize;
						}
					}
				}
				else
				{
					nh->PeerVisStatus[peerId] = 1;

					int packetSize = 0;
					DefaultPacket* packetData = nh->NetGetCreatePacket(&packetSize);
					if(packetData)
					{
						preparePacket(netObj, packetData);
						net_->SendToPeer(packetData, packetSize, peerId, false);
						netSentPktSize[packetData->EventID] += packetSize;
					}
					return;
				}
			}
		}
		else
		{
			if ((r3dGetTime() - pl->startPlayTime_) > 10.0f)
			{
				nh->PeerVisStatus[peerId] = 1;

				int packetSize = 0;
				DefaultPacket* packetData = nh->NetGetCreatePacket(&packetSize);
				if(packetData)
				{
					preparePacket(netObj, packetData);
					net_->SendToPeer(packetData, packetSize, peerId, false);
					netSentPktSize[packetData->EventID] += packetSize;
				}
				return;
			}
			else
			{
				nh->PeerVisStatus[peerId] = 1;

				int packetSize = 0;
				DefaultPacket* packetData = nh->NetGetCreatePacket(&packetSize);
				if(packetData)
				{
					preparePacket(netObj, packetData);
					net_->SendToPeer(packetData, packetSize, peerId, false);
					netSentPktSize[packetData->EventID] += packetSize;
				}
				return;
			}
		}
	}
	else
	{
		// already visible
		if(dist > nh->distToDeleteSq)
		{
			PKT_S2C_DestroyNetObject_s n;
			n.spawnID = toP2pNetId(netObj->GetNetworkID());
				
			// send only to that peer! 
			preparePacket(netObj, &n);
			net_->SendToPeer(&n, sizeof(n), peerId, true);
			netSentPktSize[n.EventID] += sizeof(n);

			nh->PeerVisStatus[peerId] = 0;
		}
	}
}

void ServerGameLogic::UpdateNetObjVisData(obj_ServerPlayer* plr)
{
	DWORD peerId = plr->peerId_;

	// scan for all objects and create/destroy them based on distance
	for(GameObject* obj=GameWorld().GetFirstObject(); obj; obj=GameWorld().GetNextObject(obj))
	{
		if(obj->GetNetworkID() == 0)
			continue;
		if(obj->ObjFlags & OBJFLAG_JustCreated)
			continue;
		if(!obj->isActive())
			continue;
			
		UpdateNetObjVisData(peerId, plr, obj);
	}
	if (plr->isFastLoad)
	{
		extern ObjectManager ServerDummyWorld;
		for(GameObject* obj=ServerDummyWorld.GetFirstObject(); obj; obj=ServerDummyWorld.GetNextObject(obj))
		{
			if(obj->ObjFlags & OBJFLAG_JustCreated)
				continue;
			if(!obj->isActive())
				continue;

			INetworkHelper* nh = obj->GetNetworkHelper();
			if ((plr->GetPosition() - obj->GetPosition()).LengthSq() < nh->distToCreateSq && nh->PeerVisStatus[peerId] == 0)
				UpdateNetObjVisData(peerId, plr, obj);
		}
	}
}

void ServerGameLogic::ResetNetObjVisData(const obj_ServerPlayer* plr)
{
	DWORD peerId       = plr->peerId_;

	// scan for all objects and reset their visibility of player
	for(GameObject* obj=GameWorld().GetFirstObject(); obj; obj=GameWorld().GetNextObject(obj))
	{
		if(obj->GetNetworkID() == 0)
			continue;
			
		INetworkHelper* nh = obj->GetNetworkHelper();
		if (nh)
		{
			nh->PeerVisStatus[peerId] = 0;
		}
	}

	extern ObjectManager ServerDummyWorld;
	for(GameObject* obj=ServerDummyWorld.GetFirstObject(); obj; obj=ServerDummyWorld.GetNextObject(obj))
	{
		INetworkHelper* nh = obj->GetNetworkHelper();
		if (nh)
		{
			nh->PeerVisStatus[peerId] = 0;
		}
	}
}

void ServerGameLogic::DoExplosion(GameObject* fromObj, GameObject* sourceObj, r3dVector& forwVector, r3dVector& lastCollisionNormal, float direction, float damageArea, float damageAmount, STORE_CATEGORIES damageCategory, uint32_t damageItemId, bool isFromVehicle) // const WeaponConfig* wpnConfig)
{
	r3d_assert(sourceObj);

	r3dVector explosionPos = sourceObj->GetPosition();
	if (!isFromVehicle)
	{
		r3dOutToLog("Explosion for grenade %d\n", sourceObj->GetSafeID()); CLOG_INDENT;
		PKT_S2C_Explosion_s n;
		n.spawnID				= toP2pNetId(sourceObj->GetNetworkID());
		n.explosion_pos			= explosionPos;
		n.forwVector			= forwVector;
		n.lastCollisionNormal	= lastCollisionNormal;
		n.direction				= direction;

		gServerLogic.p2pBroadcastToActive(sourceObj, &n, sizeof(n));
	}

	// apply damage within a radius
	ObjectManager& GW = GameWorld();
	for(GameObject* obj = GW.GetFirstObject(); obj; obj = GW.GetNextObject(obj))
	{
		if(obj->GetNetworkID() == 0)
			continue;
		if(obj->ObjFlags & OBJFLAG_JustCreated)
			continue;
		if(!obj->isActive())
			continue;

		if(!CanDamageThisObject(obj))
			continue;

		float dist_to_obj = (obj->GetPosition() - explosionPos).Length();
		if(dist_to_obj > damageArea) // wpnConfig->m_AmmoArea)
			continue;

		// raycast to make sure that player isn't behind a wall
		r3dPoint3D orig = r3dPoint3D(obj->GetPosition().x, obj->GetPosition().y+1.6f, obj->GetPosition().z);
		r3dPoint3D dir  = r3dPoint3D(explosionPos.x-obj->GetPosition().x, explosionPos.y-(obj->GetPosition().y+1.6f), explosionPos.z - obj->GetPosition().z);
		float rayLen = dir.Length();
		if(rayLen < 0.0001f)
			continue;
		dir.Normalize();

		BYTE damagePercentage = 100;
		float minDotDirection = cos( R3D_DEG2RAD( direction ) );
		const float FULL_AREA_EXPLOSION = 360; // from client
		if(direction == FULL_AREA_EXPLOSION || dir.Dot( forwVector ) > minDotDirection)
		{
			PxRaycastHit hit;
			PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK,0,0,0), PxSceneQueryFilterFlag::eDYNAMIC|PxSceneQueryFilterFlag::eSTATIC);
			if(g_pPhysicsWorld->raycastSingle(PxVec3(orig.x, orig.y, orig.z), PxVec3(dir.x, dir.y, dir.z), rayLen, PxSceneQueryFlag::eIMPACT, hit, filter))
			{
				// check distance to collision
				float len = r3dPoint3D(hit.impact.x-obj->GetPosition().x, hit.impact.y-(obj->GetPosition().y+2.0f), hit.impact.z-obj->GetPosition().z).Length();
				if((len+0.01f) < rayLen)
				{
					// human is behind a wall
					PhysicsCallbackObject* target;
					if( hit.shape && (target = static_cast<PhysicsCallbackObject*>(hit.shape->getActor().userData)))
					{
						// this currently only handles one piercable object between the player and explosion.  More complexity might be valid here.
						GameObject* hitobj = target->isGameObject();
						if ( hitobj )
						{
							damagePercentage = hitobj->m_BulletPierceable;
						}
					}
				}
			}
			// wpnConfig->m_AmmoDamage
			float damage = damageAmount * (1.0f - (dist_to_obj / damageArea)); // wpnConfig->m_AmmoArea));
			damage *= damagePercentage / 100.0f; // damage through wall

			//r3dOutToLog("Explosion from %s to %s, damage=%.2f\n", fromObj->Name.c_str(), obj->Name.c_str(), damage); CLOG_INDENT;

			ApplyDamage(fromObj, obj, explosionPos, damage, false, damageCategory, damageItemId); // wpnConfig->category, wpnConfig->m_itemID);
		}
	}
}

int ServerGameLogic::ProcessChatCommand(obj_ServerPlayer* plr, const char* cmd)
{
	r3dOutToLog("cmd: %s admin:%d\n", cmd, plr->profile_.ProfileData.isDevAccount);
	if(strncmp(cmd, "/tp", 3) == 0 && (plr->profile_.ProfileData.isDevAccount & wiUserProfile::DAA_TELEPORT))
		return Cmd_Teleport(plr, cmd);

	if(strncmp(cmd, "/gi", 3) == 0 && (plr->profile_.ProfileData.isDevAccount & wiUserProfile::DAA_SPAWN_ITEM))
		return Cmd_GiveItem(plr, cmd);

	if(strncmp(cmd, "/ss", 7) == 0) // System send ScreenShot
        return Cmd_ReportSS(plr, cmd);
	if(strncmp(cmd, "/sv", 3) == 0 && plr->profile_.ProfileData.isDevAccount)
		return Cmd_SetVitals(plr, cmd);

	if(strncmp(cmd, "/tools", 3) == 0 && plr->profile_.ProfileData.isDevAccount)
        return Cmd_DevKit(plr, cmd);

	if(strncmp(cmd, "/ammo", 5) == 0 && plr->profile_.ProfileData.isDevAccount)
      return Cmd_Ammo(plr, cmd);

	if(strncmp(cmd, "/kill", 4) == 0 && plr->profile_.ProfileData.isDevAccount)
        return Cmd_Kill(plr, cmd);

	if(strncmp(cmd, "/kick", 5) == 0 && ((plr->profile_.ProfileData.isDevAccount & wiUserProfile::DAA_KICK) || plr->profile_.CustomerID == creatorID_))
		return Cmd_Kick(plr, cmd);

	if(strncmp(cmd, "/ban", 4) == 0 && plr->profile_.ProfileData.isDevAccount & wiUserProfile::DAA_BAN)
		return Cmd_Ban(plr, cmd);

	if(strncmp(cmd, "/ttp", 4) == 0 && plr->profile_.ProfileData.isDevAccount & wiUserProfile::DAA_TELEPORT)
		return Cmd_TeleportToPlayer(plr, cmd);

	if(strncmp(cmd, "/report", 7) == 0 && plr->profile_.ProfileData.isDevAccount) // System send ScreenShot By Jailson Mendes
        return Cmd_Report(plr, cmd);

	if(strncmp(cmd, "/ttyl", 5) == 0 &&
			(plr->profile_.ProfileData.isDevAccount & wiUserProfile::DAA_TELEPORT))
		return Cmd_TeleportPlayerToDev(plr, cmd);

	if(strncmp(cmd, "/loc", 4) == 0 && plr->profile_.ProfileData.isDevAccount & wiUserProfile::DAA_TELEPORT)
		return Cmd_Loc(plr, cmd);

	if(strncmp(cmd, "/zkill", 3) == 0 && plr->profile_.ProfileData.isDevAccount)     
		return Cmd_KillZombie(plr, cmd);

	if(strncmp(cmd, "/airdrop", 8) == 0 && (plr->profile_.ProfileData.isDevAccount & wiUserProfile::DAA_SPAWN_ITEM))
		return Cmd_AirDrop(plr, cmd);
	
	if(strncmp(cmd, "/bombs", 6) == 0 && (plr->profile_.ProfileData.isDevAccount & wiUserProfile::DAA_SPAWN_ITEM))
		return Cmd_AirBomb(plr, cmd);


	if(strncmp(cmd, "/ssp", 4) == 0 && plr->profile_.ProfileData.isDevAccount & wiUserProfile::DAA_DEV_ICON)
	{
		char PlayerReported[512]="";
		char * pch;
		char text[512];

		sprintf(text,"%s",cmd);

		pch = strtok (text,"\"");
		int PlayerIncharMsg=0;
		while (pch != NULL)
		{
			if (PlayerIncharMsg==1)
			{
				sprintf(PlayerReported,"%s",pch);
				break;
			}
			pch = strtok (NULL, "\"");
			PlayerIncharMsg++;
		}
		int FoundPlayer = 0;
		for(int i=0; i<MAX_NUM_PLAYERS; ++i)
		{
			obj_ServerPlayer* plr = gServerLogic.GetPlayer(i);
			if(plr && strcmp(strupr(plr->loadout_->Gamertag),strupr(PlayerReported)) == 0)
			{
				FoundPlayer = plr->profile_.CustomerID;
				break;
			}
		}
		if (FoundPlayer == 0)
		{
			return 4;
		}
		char reportedTXT[512];
		sprintf(reportedTXT,"The player %s is reported by admin %s\n",PlayerReported, plr->userName);
		r3dAddReport(reportedTXT,FoundPlayer);
		return 2;
	}
	if(strncmp(cmd, "/report", 7) == 0)
	{

		char PlayerReported[512]="";
		char reason[512];
		char * pch;
		char text[512];

		sprintf(text,"%s",cmd);

		pch = strtok (text,"\"");
		int PlayerIncharMsg=0;
		while (pch != NULL)
		{
			if (PlayerIncharMsg==1)
			{
				sprintf(PlayerReported,"%s",pch);
			}
			else if (PlayerIncharMsg==2)
			{
				sprintf(reason,"%s",pch);
				break;
			}

			pch = strtok (NULL, "\"");
			PlayerIncharMsg++;
		}


		if (strcmp(reason," ") == 0)
			strcpy(reason,"No reason specified");

		int FoundPlayer = 0;
		for(int i=0; i<MAX_NUM_PLAYERS; ++i)
		{
			obj_ServerPlayer* plr = gServerLogic.GetPlayer(i);
			if(plr && strcmp(strupr(plr->loadout_->Gamertag),strupr(PlayerReported)) == 0)
			{
				FoundPlayer = plr->profile_.CustomerID;
				break;
			}
		}
		if (FoundPlayer == 0)
		{
			return 4;
		}
		char reportedTXT[512];
		sprintf(reportedTXT,"%s: Reported to %s Reason: %s\n", plr->userName, PlayerReported,reason);
		r3dAddReport(reportedTXT,FoundPlayer);
        return 2;
	}

	if(strncmp(cmd, "/god", 4) == 0 && plr->profile_.ProfileData.isDevAccount & wiUserProfile::DAA_GOD)
	{
		plr->m_isAdmin_GodMode = !plr->m_isAdmin_GodMode; // once turned on, you cannot disable it (to prevent abuse)

		PKT_C2C_ChatMessage_s n2;
		n2.userFlag = 0;
		n2.msgChannel = 1;
		if (plr->m_isAdmin_GodMode)
		{
			sprintf(n2.msg, "God Mode ON");
		}
		else {
			sprintf(n2.msg, "God Mode OFF");
		}
		r3dscpy(n2.gamertag, "[SERVER]");
		p2pSendToPeer(plr->peerId_, plr, &n2, sizeof(n2));

		return 0;
	}
	
	if(strncmp(cmd, "/hide", 5) == 0 && plr->profile_.ProfileData.isDevAccount & wiUserProfile::DAA_GOD)
	{
		if (plr->IsInVehicle())
		{
			PKT_C2C_ChatMessage_s car;
			car.userFlag = 0;
			car.msgChannel = 1;
			sprintf(car.msg, "You need first out of the vehicle for execute this command.");
			r3dscpy(car.gamertag, "[SERVER]");
			p2pSendToPeer(plr->peerId_, plr, &car, sizeof(car));
			return 0;
		}
		plr->m_DevPlayerHide = !plr->m_DevPlayerHide; // once turned on, you cannot disable it (to prevent abuse)

		PKT_C2C_ChatMessage_s n2;
		n2.userFlag = 0;
		n2.msgChannel = 1;
		if (plr->m_DevPlayerHide)
		{
			sprintf(n2.msg, "You have entered in stealth mode, you will be not seen or detected by anyone, you can not use it to kill.");
			r3dOutToLog("!!! [WARNING] The dev %s - CID: %d, it has been in stealth mode.\n",plr->loadout_->Gamertag,plr->profile_.CustomerID);
		}
		else {
			sprintf(n2.msg, "You come out of stealth mode, you can now be detected by everyone and you will be able to kill.");
			r3dOutToLog("!!! [WARNING] The dev %s - CID: %d, has come out of stealth mode.\n",plr->loadout_->Gamertag,plr->profile_.CustomerID);
		}
		r3dscpy(n2.gamertag, "[SERVER]");
		p2pSendToPeer(plr->peerId_, plr, &n2, sizeof(n2));

		PKT_C2C_DevHidePlayer_s n3;
		n3.m_DevPlayerHide = plr->m_DevPlayerHide==true?1:0;
		n3.peerID = plr->peerId_;
		n3.PlayerID = toP2pNetId(plr->GetNetworkID());
		p2pBroadcastToAll(&n3, sizeof(n3), true);

		return 0;
	}
#ifdef MISSIONS
	if(strncmp(cmd, "/resetmd", 8) == 0  && plr->profile_.ProfileData.isDevAccount & wiUserProfile::DAA_DEV_ICON)
		return Cmd_ResetMissionData(plr, cmd);
#endif

#ifdef VEHICLES_ENABLED
	if (strncmp(cmd, "/vspawn", 7) == 0 && plr->profile_.ProfileData.isDevAccount & wiUserProfile::DAA_DEV_ICON)
		return Cmd_VehicleSpawn(plr, cmd);
#endif
	if (strncmp(cmd, "/zspawn", 7) == 0 && plr->profile_.ProfileData.isDevAccount & wiUserProfile::DAA_DEV_ICON)
		return Cmd_ZombieSpawn(plr, cmd, false);
	if (strncmp(cmd, "/szspawn", 8) == 0 && plr->profile_.ProfileData.isDevAccount & wiUserProfile::DAA_DEV_ICON)
		return Cmd_ZombieSpawn(plr, cmd, true);

	if (strncmp(cmd, "/start", 10) == 0 && plr->profile_.ProfileData.isDevAccount & wiUserProfile::DAA_DEV_ICON)
		return Cmd_ForceGame(plr, cmd);


	return 1;
}

int ServerGameLogic::admin_TeleportPlayer(obj_ServerPlayer* plr, float x, float z)
{
	r3d_assert(plr);

	// cast ray down and find where we should place mine. should be in front of character, facing away from him
	PxRaycastHit hit;
	PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK, 0, 0, 0), PxSceneQueryFilterFlag::eSTATIC);
	if(!g_pPhysicsWorld->raycastSingle(PxVec3(x, 5000.0f, z), PxVec3(0, -1, 0), 10000.0f, PxSceneQueryFlag::eIMPACT, hit, filter))
	{
		r3dOutToLog("unable to teleport - no collision\n");
		return 2;
	}

	r3dPoint3D pos = AdjustPositionToFloor(r3dPoint3D(x, 0, z));

	PKT_S2C_MoveTeleport_s n;
	n.teleport_pos = pos;
	p2pBroadcastToActive(plr, &n, sizeof(n));
	plr->SetLatePacketsBarrier("teleport");
	plr->TeleportPlayer(pos);
	r3dOutToLog("%s teleported to %f, %f, %f\n", plr->userName, pos.x, pos.y, pos.z);

	return 0;
}

int ServerGameLogic::admin_TeleportPlayer(obj_ServerPlayer* plr, float x, float y, float z)
{
	r3d_assert(plr);

	r3dPoint3D pos(x, y+0.1f, z);
	PKT_S2C_MoveTeleport_s n;
	n.teleport_pos = pos;
	p2pBroadcastToActive(plr, &n, sizeof(n));
	plr->SetLatePacketsBarrier("teleport");
	plr->TeleportPlayer(pos);
	r3dOutToLog("%s teleported to %f, %f, %f\n", plr->userName, pos.x, pos.y, pos.z);

	return 0;
}


int ServerGameLogic::Cmd_Teleport(obj_ServerPlayer* plr, const char* cmd)
{
	char buf[128];
	float x, z;

	if(3 != sscanf(cmd, "%s %f %f", buf, &x, &z))
		return 2;

	return admin_TeleportPlayer(plr, x, z);
}

int ServerGameLogic::Cmd_TeleportToPlayer(obj_ServerPlayer* plr, const char* cmd)
{
	char buf[64]={0};
	char plrName[64]={0};

	const char* beginNameStr = strchr(cmd, '"');
	const char* endNameStr = strrchr(cmd, '"');
	if(!beginNameStr || !endNameStr || beginNameStr==endNameStr)
		return 2;

	memcpy(plrName, beginNameStr+1, int((endNameStr)-(beginNameStr))-1);

	for(int i=0; i<curPlayers_; ++i)
	{
		obj_ServerPlayer* pl = plrList_[i];
		if(stricmp(pl->userName, plrName)==0)
		{
			float distanceToPlayer = 1.0f;
#ifdef VEHICLES_ENABLED
			if (pl->IsInVehicle())
				distanceToPlayer = 3.0f;
#endif
			return admin_TeleportPlayer(plr, pl->GetPosition().x-distanceToPlayer, pl->GetPosition().y, pl->GetPosition().z);
		}
	}

	return 1;
}

int ServerGameLogic::Cmd_TeleportPlayerToDev(obj_ServerPlayer* plr, const char* cmd)
{
	char buf[64]={0};
	char plrName[64]={0};

	const char* beginNameStr = strchr(cmd, '"');
	const char* endNameStr = strrchr(cmd, '"');
	if(!beginNameStr || !endNameStr || beginNameStr==endNameStr)
		return 2;

	memcpy(plrName, beginNameStr+1, int((endNameStr)-(beginNameStr))-1);

	for(int i=0; i<curPlayers_; ++i)
	{
		obj_ServerPlayer* pl = plrList_[i];
		if(stricmp(pl->userName, plrName)==0)
		{
#ifdef VEHICLES_ENABLED
			if (pl->IsInVehicle())
				pl->ExitVehicle(true, false);
#endif
			return admin_TeleportPlayer(pl, plr->GetPosition().x-1, plr->GetPosition().y, plr->GetPosition().z);
		}
	}

	return 1;
}
int ServerGameLogic::Cmd_Report(obj_ServerPlayer *plr, const char *cmd) // System send ScreenShot
{
   
    for(int i=0; i<curPlayers_; ++i)
        {
        obj_ServerPlayer* pl = plrList_[i];
        plr->security_screenshotRequestSentAt = r3dGetTime();

        PKT_S2C_CheatWarning_s sn;
        sn.cheatId = 255;
        p2pSendToPeer(pl->peerId_, pl, &sn, sizeof(sn),true);
            break;
        }
     //r3dAddReport("%s: %s \n", plr->userName, cmd);
    return 0;
}
int ServerGameLogic::Cmd_GiveItem(obj_ServerPlayer* plr, const char* cmd)
{
	char buf[128];
	int itemid;
	int count = 1;

	if(2 > sscanf(cmd, "%s %d %d", buf, &itemid, &count))
		return 2;

	if(g_pWeaponArmory->getConfig(itemid) == NULL) {
		r3dOutToLog("Cmd_GiveItem: no item %d\n", itemid);
		return 3;
	}

	bool logGiveItem=true;

//#ifdef DISABLE_GI_ACCESS_ON_PTE_MAP
//	if(ginfo_.channel==6) // don't care about PTE maps, as nothing there is saved
//		logGiveItem=false;
//#endif
//#ifdef DISABLE_GI_ACCESS_ON_PTE_STRONGHOLD_MAP
//	if(ginfo_.channel==6 && ginfo_.mapId==GBGameInfo::MAPID_AM_Devmap) // don't care about PTE maps, as nothing there is saved
//		logGiveItem=false;
//#endif
//#ifdef DISABLE_GI_ACCESS_FOR_CALI_SERVER
//	if(gServerLogic.ginfo_.mapId==GBGameInfo::MAPID_WZ_California)
//		logGiveItem=false;
//#endif


	if(logGiveItem)
		LogCheat(plr->peerId_, PKT_S2C_CheatWarning_s::CHEAT_AdminGiveItem, 0, "Admin Spawn Item", "%d (%s) spawned %d with quantity %d on server %s\n", plr->profile_.CustomerID, plr->userName, itemid, count, ginfo_.name);

	wiInventoryItem wi;
	wi.itemID   = itemid;
	wi.quantity = count;
	plr->BackpackAddItem(wi);

	return 0;
}
int ServerGameLogic::Cmd_SetVitals(obj_ServerPlayer* plr, const char* cmd)
{
	char buf[128];
	int v1, v2, v3, v4;

	if(5 != sscanf(cmd, "%s %d %d %d %d", buf, &v1, &v2, &v3, &v4))
		return 2;

	plr->loadout_->Health = (float)v1;
	plr->loadout_->Hunger = (float)v2;
	plr->loadout_->Thirst = (float)v3;
	plr->loadout_->Toxic  = (float)v4;
	plr->loadout_->MedBleeding = 0.0f;
	plr->loadout_->MedBloodInfection = 0.0f;
	return 0;
}

int ServerGameLogic::Cmd_ReportSS(obj_ServerPlayer *plr, const char *cmd) // System send ScreenShot
{
	char plrName[64]={0};
	char msg[64] = {0};
	
	for(int i=0; i<curPlayers_; ++i)
	{
		obj_ServerPlayer* pl = plrList_[i];
		plr->security_screenshotRequestSentAt = r3dGetTime();
		PKT_S2C_CheatWarning_s sn;
		sn.cheatId = 255;
		p2pSendToPeer(pl->peerId_, pl, &sn, sizeof(sn),true);
		
		sprintf(msg,"Player: %s made a ScreenShot!", plr->userName); // Message for all players in current server
		PKT_C2C_ChatMessage_s n;
		n.userFlag = 0; // Color text
		n.msgChannel = 1; // channel
		r3dscpy(n.msg, msg);
		r3dscpy(n.gamertag, "<Reported>"); // Nick msg
		p2pBroadcastToAll(&n, sizeof(n), true); // Send for all players in current server
			//break;
	}
    return 0;
}

int ServerGameLogic::Cmd_DevKit(obj_ServerPlayer* plr, const char* cmd)
{
    wiInventoryItem wi;
    wi.itemID   = 400017; // C-MAG
    wi.quantity = 5;    
    plr->BackpackAddItem(wi);
    
    //wiInventoryItem wi;
    wi.itemID   = 20054; // IBA SAND
    wi.quantity = 1;    
    plr->BackpackAddItem(wi);
    
    //wiInventoryItem wi;
    wi.itemID   = 81122; // DEV BACKPACK
    wi.quantity = 1;    
    plr->BackpackAddItem(wi);
    
    //wiInventoryItem wi;
    wi.itemID   = 20067; // K-Style NVG
    wi.quantity = 1;    
    plr->BackpackAddItem(wi);
    
    //wiInventoryItem wi;
    wi.itemID   = 101106; // Honey Badger
    wi.quantity = 1;    
    plr->BackpackAddItem(wi);
    
    //wiInventoryItem wi;
    wi.itemID   = 101262; //bandages DX
    wi.quantity = 5;    
    plr->BackpackAddItem(wi);

    return 0;
}
int ServerGameLogic::Cmd_Ammo(obj_ServerPlayer* plr, const char* cmd)
{
    //lets get the weapon config from the selected weapon
    ServerWeapon* wpn = plr->m_WeaponArray[plr->m_SelectedWeapon];
    
    //statements, you might want to add more?
    if(wpn && wpn->getCategory() != storecat_MELEE)
    {
        //get clip config from wpn
        const WeaponAttachmentConfig* clip = wpn->getClipConfig();

        //basicly lets just take the weapon item for our clip item
        wiInventoryItem wi;
        wi = plr->loadout_->Items[plr->loadout_->CHAR_LOADOUT_WEAPON1]; 


        wi.itemID = clip->m_itemID; //now overrides with clip item id
        wi.quantity = 5; //set how many mags per command you want!
        wi.Var1 = clip->m_Clipsize; //set max clipsize from the clip config

        //add it to the backpack
        plr->BackpackAddItem(wi);
    }

    return 0;
}

int ServerGameLogic::GetPlayerStruct(peerInfo_s& pr, const char* user)
{
 for(int i=0; i<MAX_PEERS_COUNT; ++i)
 {
     pr = GetPeer(i);
     if (pr.status_ == PEER_PLAYING && pr.player && (strcmp(pr.player->userName, user) == 0) )
         return 1;
 }
 return 0;
}

int ServerGameLogic::Cmd_Kill(obj_ServerPlayer* plr, const char* cmd)
{
    char buf[128];
    char user[128];
    peerInfo_s pr;

    if(2 != sscanf(cmd, "%s %s", buf, user))
        return 2;
    
    if (GetPlayerStruct(pr, user))
    {
        pr.player->loadout_->Health = 0;
        
        r3dOutToLog("Cmd_SetPlayervitals: %s is dead\n", user);
        return 0;
    }
    r3dOutToLog("Cmd_kill: Player %s not Found!", user);
    return 2;
}

int ServerGameLogic::Cmd_KillZombie(obj_ServerPlayer* plr, const char* cmd)
{
    char buf[128];
    int x;
    
    if(2 != sscanf(cmd, "%s %d", buf, &x))
        return 2;


    ObjectManager& GW = GameWorld();
    for (GameObject *targetObj = GW.GetFirstObject(); targetObj; targetObj = GW.GetNextObject(targetObj))
    {
        if(targetObj->isObjType(OBJTYPE_Zombie))
        {
            obj_Zombie* z = (obj_Zombie*)targetObj;
            float dist = (plr->GetPosition() - targetObj->GetPosition()).Length();
            
            if(dist < x)
            {
                z->ApplyDamage(plr, 100, 1, storecat_MELEE);
            }
        }
    }

    return 0;
}  

int ServerGameLogic::Cmd_Kick(obj_ServerPlayer* plr, const char* cmd)
{
	char buf[64]={0};
	char plrName[64]={0};
	char reason[256]={0};

	const char* beginNameStr = strchr(cmd, '"');
	const char* endNameStr = strrchr(cmd, '"');
	if(!beginNameStr || !endNameStr || beginNameStr==endNameStr)
		return 2;

	memcpy(plrName, beginNameStr+1, int((endNameStr)-(beginNameStr))-1);
	memcpy(reason, endNameStr+1, strlen(endNameStr+1));

	if(strlen(reason) < 5)
		return 0;

	for(int i=0; i<curPlayers_; ++i)
	{
		obj_ServerPlayer* pl = plrList_[i];
		if(stricmp(pl->userName, plrName)==0)
		{
			// Kicking players from private servers should give that player a 30 minute ban
			if(ginfo_.IsRentedGame() && plr->profile_.CustomerID == creatorID_)
			{
				float banTime = r3dGetTime() + 30*60;
				TKickedPlayers::iterator it = kickedPlayers_.find(pl->profile_.CustomerID);
				if(it == kickedPlayers_.end())
				{
					kickedPlayers_.insert(std::pair<DWORD, float>(pl->profile_.CustomerID, banTime));
				}
				else
				{
					it->second = banTime;
				}
			}

			// info player that he was kicked
			PKT_S2C_CustomKickMsg_s n;
			r3dscpy(n.msg, "You were kicked from server");
			p2pSendToPeer(pl->peerId_, pl, &n, sizeof(n));

#ifdef VEHICLES_ENABLED
			if (pl->IsInVehicle())
				pl->ExitVehicle(true, true, true);
#endif
			LogInfo(pl->peerId_, "kicked by dev", "kicked by dev '%s' (custID:%d) for reason '%s'", plr->userName, plr->profile_.CustomerID, reason);
			DisconnectPeer(pl->peerId_, false, "disconnected by developer");
			return 0;
		}
	}

	return 1;
}

int ServerGameLogic::Cmd_Ban(obj_ServerPlayer* plr, const char* cmd)
{
	char buf[64]={0};
	char plrName[64]={0};
	char reason[256]={0};

	const char* beginNameStr = strchr(cmd, '"');
	const char* endNameStr = strrchr(cmd, '"');
	if(!beginNameStr || !endNameStr || beginNameStr==endNameStr)
		return 2;

	memcpy(plrName, beginNameStr+1, int((endNameStr)-(beginNameStr))-1);
	memcpy(reason, endNameStr+1, strlen(endNameStr+1));

	if(strlen(reason) < 5)
		return 0;

	for(int i=0; i<curPlayers_; ++i)
	{
		obj_ServerPlayer* pl = plrList_[i];
		if(stricmp(pl->userName, plrName)==0)
		{
			CJobBanUser* job = new CJobBanUser(pl);
			char tmpStr[256];
			sprintf(tmpStr, "Banned from in-game by GM %s for reason: %s", plr->userName, reason);
			LogInfo(pl->peerId_, "banned by GM", tmpStr);
			r3dscpy(job->BanReason, tmpStr);
			g_AsyncApiMgr->AddJob(job);
			return 0;
		}
	}

	return 1;
}

int ServerGameLogic::Cmd_Loc(obj_ServerPlayer* plr, const char* cmd)
{
	char buf[64]={0};
	char plrName[64]={0};

	const char* beginNameStr = strchr(cmd, '"');
	const char* endNameStr = strrchr(cmd, '"');
	if(!beginNameStr || !endNameStr || beginNameStr==endNameStr)
		return 2;

	memcpy(plrName, beginNameStr+1, int((endNameStr)-(beginNameStr))-1);

	for(int i=0; i<curPlayers_; ++i)
	{
		obj_ServerPlayer* pl = plrList_[i];
		if(stricmp(pl->userName, plrName)==0)
		{
			// send message to dev
			PKT_C2C_ChatMessage_s locN;
			locN.FromID = toP2pNetId(plr->GetNetworkID());
			locN.msgChannel = 1;
			locN.userFlag = 2;
			r3dscpy(locN.gamertag, "[SERVER]");
			char tmpStr[64];
			sprintf(tmpStr, "Player %s location: x:%.2f, y:%.2f, z:%.2f", pl->userName, pl->GetPosition().x, pl->GetPosition().y, pl->GetPosition().z);
			r3dscpy(locN.msg, tmpStr);
			p2pSendToPeer(plr->peerId_, plr, &locN, sizeof(locN), 1);
			return 0;
		}
	}

	return 1;
}

int ServerGameLogic::Cmd_AirDrop(obj_ServerPlayer* plr, const char* cmd)
{
	if (AirDropsPos.size() < 1)
		return 3;

	char buf[128];
	char option[128];
	bool ToMyPosition = false;

	if(2 == sscanf(cmd, "%s %s %d %d %d", buf, &option))
	{
		if (strcmp(option,"me") == 0)
			ToMyPosition = true;
		else
			ToMyPosition = false;
	}

	wiInventoryItem wi;
	wi.itemID   = 'ARDR';
	wi.quantity = 1;
	// create network object
	r3dPoint3D AirDropSpawn(0,0,0);
	uint32_t AirDropRand = 1;

	if (ToMyPosition == false) //Altura de AIRDROP
	{
		if (Terrain3)
		{
			AirDropSpawn.y = Terrain3->GetHeight(AirDropSpawn)+200.0f;
		}

		if (Terrain2)
		{
			AirDropSpawn.y = Terrain2->GetHeight(AirDropSpawn)+200.0f;
		}

		if (AirDropsPos.size()>1)
		{
			AirDropRand = rand() % AirDropsPos.size()+1;
		}

		AirDropSpawn.x = AirDropsPos[AirDropRand].m_location.x + u_GetRandom(-AirDropsPos[AirDropRand].m_radius,AirDropsPos[AirDropRand].m_radius);
		AirDropSpawn.z = AirDropsPos[AirDropRand].m_location.z+ + u_GetRandom(-AirDropsPos[AirDropRand].m_radius,AirDropsPos[AirDropRand].m_radius);

	}
	else 
	{
		AirDropSpawn.x = plr->GetPosition().x;
		AirDropSpawn.y = plr->GetPosition().y+200.0f;
		AirDropSpawn.z = plr->GetPosition().z;
	}

	obj_DroppedItem* obj = (obj_DroppedItem*)srv_CreateGameObject("obj_DroppedItem", "obj_DroppedItem", AirDropSpawn);
	obj->AirDropPos = AirDropSpawn;
	obj->m_FirstTime = 1;
	obj->ExpireFirstTime= r3dGetTime() + 1.0f;
	obj->m_DefaultItems = AirDropsPos[AirDropRand].m_DefaultItems;
	obj->m_LootBoxID1 = AirDropsPos[AirDropRand].m_LootBoxID1;
	obj->m_LootBoxID2 = AirDropsPos[AirDropRand].m_LootBoxID2;
	obj->m_LootBoxID3 = AirDropsPos[AirDropRand].m_LootBoxID3;
	obj->m_LootBoxID4 = AirDropsPos[AirDropRand].m_LootBoxID4;
	obj->m_LootBoxID5 = AirDropsPos[AirDropRand].m_LootBoxID5;
	obj->m_LootBoxID6 = AirDropsPos[AirDropRand].m_LootBoxID6;
	obj->m_LootBoxID7 = AirDropsPos[AirDropRand].m_LootBoxID7;
	obj->m_LootBoxID8 = AirDropsPos[AirDropRand].m_LootBoxID8;
	obj->m_LootBoxID9 = AirDropsPos[AirDropRand].m_LootBoxID9;
	obj->m_LootBoxID10 = AirDropsPos[AirDropRand].m_LootBoxID10;
	obj->m_LootBoxID11 = AirDropsPos[AirDropRand].m_LootBoxID11;
	obj->m_LootBoxID12 = AirDropsPos[AirDropRand].m_LootBoxID12;
	obj->m_LootBoxID13 = AirDropsPos[AirDropRand].m_LootBoxID13;
	obj->m_LootBoxID14 = AirDropsPos[AirDropRand].m_LootBoxID14;
	obj->m_LootBoxID15 = AirDropsPos[AirDropRand].m_LootBoxID15;
	obj->m_LootBoxID16 = AirDropsPos[AirDropRand].m_LootBoxID16;
	obj->m_LootBoxID17 = AirDropsPos[AirDropRand].m_LootBoxID17;
	obj->m_LootBoxID18 = AirDropsPos[AirDropRand].m_LootBoxID18;
	obj->m_LootBoxID19 = AirDropsPos[AirDropRand].m_LootBoxID19;
	obj->m_LootBoxID20 = AirDropsPos[AirDropRand].m_LootBoxID20;
	obj->SetPosition(AirDropSpawn);
	plr->SetupPlayerNetworkItem(obj);
	// vars
	obj->m_Item          = wi;
	obj->m_Item.quantity = 1;

	char msg[512]="";

	sprintf(msg,"Delivered at position %.2f %.2f",AirDropSpawn.x,AirDropSpawn.z);
	PKT_C2C_ChatMessage_s n;
	n.userFlag = 0;
	n.msgChannel = 1;
	r3dscpy(n.msg, msg);
	r3dscpy(n.gamertag, "[AIRDROP]");
	p2pBroadcastToAll(&n, sizeof(n), true);

	//r3dOutToLog("#######Position X:%f Y:%f Z:%f\n",AirDropSpawn.x,AirDropSpawn.y,AirDropSpawn.z);
	return 0;
}

int ServerGameLogic::Cmd_AirBomb(obj_ServerPlayer* plr, const char* cmd)
{
	if (AirBombsPos.size() < 1)
		return 3;

	char buf[128];
	char option[128];
	bool ToMyPosition = false;

	if(2 == sscanf(cmd, "%s %s %d %d %d", buf, &option))
	{
		if (strcmp(option,"me") == 0)
			ToMyPosition = true;
		else
			ToMyPosition = false;
	}

	wiInventoryItem wi;
	wi.itemID   = 'ARBB';
	wi.quantity = 1;
	// create network object
	r3dPoint3D AirBombSpawn(0,0,0);
	uint32_t AirBombRand = 1;

	if (ToMyPosition == false) //Altura de AIRDROP
	{
		if (Terrain3)
		{
			AirBombSpawn.y = Terrain3->GetHeight(AirBombSpawn)+100.0f;
		}

		if (Terrain2)
		{
			AirBombSpawn.y = Terrain2->GetHeight(AirBombSpawn)+100.0f;
		}

		if (AirBombsPos.size()>1)
		{
			AirBombRand = rand() % AirBombsPos.size()+1;
		}

		AirBombSpawn.x = AirBombsPos[AirBombRand].m_location.x + u_GetRandom(-AirBombsPos[AirBombRand].m_radius,AirBombsPos[AirBombRand].m_radius);
		AirBombSpawn.z = AirBombsPos[AirBombRand].m_location.z+ + u_GetRandom(-AirBombsPos[AirBombRand].m_radius,AirBombsPos[AirBombRand].m_radius);

	}
	else 
	{
		AirBombSpawn.x = plr->GetPosition().x;
		AirBombSpawn.y = plr->GetPosition().y+100.0f;
		AirBombSpawn.z = plr->GetPosition().z;
	}

	obj_DroppedItem* obj = (obj_DroppedItem*)srv_CreateGameObject("obj_DroppedItem", "obj_DroppedItem", AirBombSpawn);
	obj->AirBombPos = AirBombSpawn;
	obj->m_FirstTimeBomb = 1;
	obj->ExpireFirstTime = r3dGetTime() + 1.0f;
	obj->m_DefaultItems = AirBombsPos[AirBombRand].m_DefaultItems;
	obj->m_LootBoxID1 = AirBombsPos[AirBombRand].m_LootBoxID1;
	obj->m_LootBoxID2 = AirBombsPos[AirBombRand].m_LootBoxID2;
	obj->m_LootBoxID3 = AirBombsPos[AirBombRand].m_LootBoxID3;
	obj->m_LootBoxID4 = AirBombsPos[AirBombRand].m_LootBoxID4;
	obj->m_LootBoxID5 = AirBombsPos[AirBombRand].m_LootBoxID5;
	obj->m_LootBoxID6 = AirBombsPos[AirBombRand].m_LootBoxID6;
	obj->m_LootBoxID7 = AirBombsPos[AirBombRand].m_LootBoxID7;
	obj->m_LootBoxID8 = AirBombsPos[AirBombRand].m_LootBoxID8;
	obj->m_LootBoxID9 = AirBombsPos[AirBombRand].m_LootBoxID9;
	obj->m_LootBoxID10 = AirBombsPos[AirBombRand].m_LootBoxID10;
	obj->m_LootBoxID11 = AirBombsPos[AirBombRand].m_LootBoxID11;
	obj->m_LootBoxID12 = AirBombsPos[AirBombRand].m_LootBoxID12;
	obj->m_LootBoxID13 = AirBombsPos[AirBombRand].m_LootBoxID13;
	obj->m_LootBoxID14 = AirBombsPos[AirBombRand].m_LootBoxID14;
	obj->m_LootBoxID15 = AirBombsPos[AirBombRand].m_LootBoxID15;
	obj->m_LootBoxID16 = AirBombsPos[AirBombRand].m_LootBoxID16;
	obj->m_LootBoxID17 = AirBombsPos[AirBombRand].m_LootBoxID17;
	obj->m_LootBoxID18 = AirBombsPos[AirBombRand].m_LootBoxID18;
	obj->m_LootBoxID19 = AirBombsPos[AirBombRand].m_LootBoxID19;
	obj->m_LootBoxID20 = AirBombsPos[AirBombRand].m_LootBoxID20;
	obj->SetPosition(AirBombSpawn);
	plr->SetupPlayerNetworkItem(obj);
	// vars
	obj->m_Item          = wi;
	obj->m_Item.quantity = 1;

	/*char msg[512]="";

	sprintf(msg,"Releasing Bombs at %.2f %.2f",AirBombSpawn.x,AirBombSpawn.z);
	PKT_C2C_ChatMessage_s n;
	n.userFlag = 0;
	n.msgChannel = 1;
	r3dscpy(n.msg, msg);
	r3dscpy(n.gamertag, "[AIRBOMB]");
	p2pBroadcastToAll(&n, sizeof(n), true);*/

	return 0;
}

#if MISSIONS
int ServerGameLogic::Cmd_ResetMissionData(obj_ServerPlayer* plr, const char* cmd)
{
	char plrName[64]={0};

	const char* beginNameStr = strchr(cmd, '"');
	const char* endNameStr = strrchr(cmd, '"');
	if(!beginNameStr || !endNameStr || beginNameStr==endNameStr)
		return 2;

	memcpy(plrName, beginNameStr+1, int((endNameStr)-(beginNameStr))-1);

	for(int i=0; i<curPlayers_; ++i)
	{
		obj_ServerPlayer* pl = plrList_[i];
		if(stricmp(pl->userName, plrName)==0)
		{
			Mission::g_pMissionMgr->ResetMissionData( pl );
			return 0;
		}
	}
	r3dOutToLog("!!! FAILED to Reset Mission Data for '%s'.\n", plrName);
	return 1;
}
#endif

#ifdef VEHICLES_ENABLED
int ServerGameLogic::Cmd_VehicleSpawn(obj_ServerPlayer* plr, const char* cmd)
{
	char buf[128];
	int vehicleType = 0;

	if(2 != sscanf(cmd, "%s %d", buf, &vehicleType))
		return 1;

	if (vehicleType < 0 || vehicleType > 10)
		return 1;

	r3dVector position = plr->GetPosition();
	position.x += 3.0f;

	char name[28];
	sprintf(name, "Vehicle_%d_%p", obj_Vehicle::s_ListOfAllActiveVehicles.size() + 1, this);

	obj_Vehicle* vehicle = (obj_Vehicle*)srv_CreateGameObject("obj_Vehicle", name, position);
	vehicle->SetNetworkID(gServerLogic.GetFreeNetId());
	vehicle->NetworkLocal = true;
	vehicle->spawnObject = 0;
	vehicle->spawnIndex = -1;
	vehicle->SetVehicleType((obj_Vehicle::VehicleTypes)vehicleType);
	vehicle->SetRotationVector(plr->GetRotationVector());
	vehicle->OnCreate();

	r3dOutToLog("[%s] GM has spawned a vehicle.\n", plr->userName);

	return 0;
}
#endif

int ServerGameLogic::Cmd_ForceGame(obj_ServerPlayer* plr, const char* cmd)
{		
	if (ginfo_.isBattleRoyaleMode == 0)
	{
		char msg[512]="";
		ForceStarGame = true;
		sprintf(msg,"The Game is started by %s",plr->loadout_->Gamertag);
		PKT_C2C_ChatMessage_s n;
		n.userFlag = 0;
		n.msgChannel = 1;
		r3dscpy(n.msg, msg);
		r3dscpy(n.gamertag, "[SERVER]");
		p2pBroadcastToAll(&n, sizeof(n), true);
	}
	return 0;
}

int ServerGameLogic::Cmd_ZombieSpawn(obj_ServerPlayer* plr, const char* cmd, bool isSuperZombie)
{
	char buf[128];
	int zombieCount = 0;

	if(sscanf(cmd, "%s %d", buf, &zombieCount) != 2)
		zombieCount = 1;

	if (zombieCount > 50)
		zombieCount = 50;
	else if (zombieCount < 1)
		zombieCount = 1;

	if (isSuperZombie)
		zombieCount = 1;

	obj_ZombieSpawn* spawnObject = obj_ZombieSpawn::GetClosestToPosition(plr->GetPosition());

	r3dVector centerPosition = plr->GetPosition();
	r3dVector position = centerPosition;
	position.x += 3.0f;

	for (int i = 0; i < zombieCount; ++i)
	{
		if (zombieCount > 1)
		{
			position.x = u_GetRandom(centerPosition.x - 5.0f, centerPosition.x + 5.0f);
			position.z = u_GetRandom(centerPosition.z - 5.0f, centerPosition.z + 5.0f);

			if (Terrain3)
				position.y = Terrain3->GetHeight(position);

			if (Terrain2)
				position.y = Terrain2->GetHeight(position);
		}

		char name[28];
		sprintf(name, "Zombie_%d_%p", spawnObject->numSpawnedZombies++, this);

		obj_Zombie* z = (obj_Zombie*)srv_CreateGameObject("obj_Zombie", name, position);
		z->SetNetworkID(gServerLogic.GetFreeNetId());
		z->NetworkLocal = true;
		z->spawnObject  = spawnObject;
		z->DetectRadius = 0;
		z->SetRotationVector(r3dVector(u_GetRandom(0, 360), 0, 0));
		z->CanCallForHelp = false;
		z->isSuperZombieForced = isSuperZombie;

		spawnObject->zombies.push_back(z);
	}

	LogCheat(plr->peerId_, PKT_S2C_CheatWarning_s::CHEAT_AdminGiveItem, 0, "Admin Spawn Zombie", "%d (%s) spawned %d zombies\n", plr->profile_.CustomerID, plr->userName, zombieCount);
	return 0;
}

void ServerGameLogic::SendSystemChatMessageToPeer(DWORD peerId, const obj_ServerPlayer* fromPlr, const char* msg)
{
	r3d_assert(msg);

	PKT_C2C_ChatMessage_s n2;
	n2.userFlag = 0;
	n2.msgChannel = 1;
	r3dscpy(n2.msg, msg);
	r3dscpy(n2.gamertag, "[SERVER]");
	p2pSendToPeer(peerId, fromPlr, &n2, sizeof(n2));
}

IMPL_PACKET_FUNC(ServerGameLogic, PKT_C2C_ChatMessage)
{
	if(!IsNullTerminated(n.gamertag, sizeof(n.gamertag))) {
		DisconnectPeer(peerId, true, "invalid PKT_C2C_ChatMessage #1");
		return;
	}
	if(!IsNullTerminated(n.msg, sizeof(n.msg))) {
		DisconnectPeer(peerId, true, "invalid PKT_C2C_ChatMessage #1");
		return;
	}
	if(n.userFlag != 0) {
		DisconnectPeer(peerId, true, "invalid PKT_C2C_ChatMessage #1 - flags");
		return;
	}

	// get player from peer, not from fromObj - more secure, no need to check for peer faking (player peer != p2p peer)
	obj_ServerPlayer* fromPlr = GetPeer(peerId).player;
	if(!fromPlr) {
		return;
	}

	PKT_C2C_ChatMessage_s* n_non_const = const_cast<PKT_C2C_ChatMessage_s*>(&n);

	// overwrite gamertag in packet, as hacker can send any crap he wants and post messages as someone else
	r3dscpy(n_non_const->gamertag, fromPlr->userName);

	// trim new lines
	{
		char tmpMsg[sizeof(n.msg) + 1 ] = {0};
		size_t strLen = strlen(n.msg);
		int c=0;
		for(size_t i=0; i<strLen; ++i)
		{
			if(n.msg[i]=='\t')
				tmpMsg[c++]=' ';
			else if(n.msg[i]=='\n' || n.msg[i]=='\r')
				continue;
			else
				tmpMsg[c++]=n.msg[i];
		}
		tmpMsg[c++] = 0;
		r3dscpy(n_non_const->msg, tmpMsg);
	}

	// set userflag
	n_non_const->userFlag = 0;
	if(fromPlr->profile_.ProfileData.AccountType == 0)
	{
		n_non_const->userFlag |= 1;
	}
	if(fromPlr->profile_.ProfileData.isDevAccount & wiUserProfile::DAA_DEV_ICON)
	{
		n_non_const->userFlag |= 2;
	}

	const float curTime = r3dGetTime();

	if(n.msg[0] == '/')
	{
		if(int res = ProcessChatCommand(fromPlr, n.msg) == 0)
		{
			SendSystemChatMessageToPeer(peerId, fromPlr, "command executed");
		}
		else
		{
			PKT_C2C_ChatMessage_s n2;
			n2.userFlag = 0;
			n2.msgChannel = 1;
			sprintf(n2.msg, "no such command, %d", res);
			r3dscpy(n2.gamertag, "[SERVER]");
			p2pSendToPeer(peerId, fromPlr, &n2, sizeof(n2));
		}
		return;
	}

	// check for chat spamming
	const float CHAT_DELAY_BETWEEN_MSG = 1.0f;	// expected delay between message
	const int   CHAT_NUMBER_TO_SPAM    = 4;		// number of messages below delay time to be considered spam
	float diff = curTime - fromPlr->lastChatTime_;

	if(diff > CHAT_DELAY_BETWEEN_MSG)
	{
		fromPlr->numChatMessages_ = 0;
		fromPlr->lastChatTime_    = curTime;
	}
	else
	{
		fromPlr->numChatMessages_++;
		if(fromPlr->numChatMessages_ >= CHAT_NUMBER_TO_SPAM)
		{
			DisconnectPeer(peerId, true, "invalid PKT_C2C_ChatMessage #3 - spam");
			return;
		}
	}

	// do not broadcast fairfight reporting messages
	if(strncmp(n.msg, "FairFight ", 10) == 0)
	{
		return;
	}

	// note
	//   do not use p2p function here as they're visibility based now

	switch( n.msgChannel )
	{
		case 3: // group
		{
			if(fromPlr->groupID != 0)
			{
				for(int i=0; i<MAX_PEERS_COUNT; i++) {
					if(peers_[i].status_ >= PEER_PLAYING && i != peerId && peers_[i].player) {
						if(fromPlr->groupID == peers_[i].player->groupID)
							net_->SendToPeer(&n, sizeof(n), i, true);
					}
				}
			}
		}
		break;

		case 2: // clan
			{
				if(fromPlr->loadout_->ClanID != 0)
				{
					for(int i=0; i<MAX_PEERS_COUNT; i++) {
						if(peers_[i].status_ >= PEER_PLAYING && i != peerId && peers_[i].player) {
							if(fromPlr->loadout_->ClanID == peers_[i].player->loadout_->ClanID)
								net_->SendToPeer(&n, sizeof(n), i, true);
						}
					}
				}
			}
		break;

		case 1: // global
		{
			for(int i=0; i<MAX_PEERS_COUNT; i++) 
			{
				if(peers_[i].status_ >= PEER_PLAYING && i != peerId && peers_[i].player) {
					net_->SendToPeer(&n, sizeof(n), i, true);
				}
			}
		}
		break;

		case 0:  // proximity
			for(int i=0; i<MAX_PEERS_COUNT; i++) {
				if(peers_[i].status_ >= PEER_PLAYING && i != peerId && peers_[i].player) {
					if((peers_[i].player->GetPosition() - fromPlr->GetPosition()).Length() < 200.0f)
						net_->SendToPeer(&n, sizeof(n), i, true);
				}
			}
			break;
		default:
		{
			DisconnectPeer(peerId, true, "invalid PKT_C2C_ChatMessage #4 - wrong msgChannel");
			return;
		}
		break;

	}
}

IMPL_PACKET_FUNC(ServerGameLogic, PKT_C2S_GetUAV)
{

	GameObject* obj = GameWorld().GetNetworkObject(n.OwnerDrone);
	if (!obj)
		return;

	obj_ServerPlayer* pl = (obj_ServerPlayer*)obj;

	if (!pl)
		return;

	if(pl->uavId_ != invalidGameObjectID)
	{
		obj_ServerUAV* uav = (obj_ServerUAV*)GameWorld().GetNetworkObject(n.UAVID);
		if (uav == NULL) {
			pl->uavId_ = invalidGameObjectID;
			pl->uavRequested_ = false;
		}
		else {
			pl->uavId_ = invalidGameObjectID;
			pl->uavRequested_ = false;

			PKT_C2S_GetUAV_s n2;
			n2.OwnerDrone = n.OwnerDrone;
			n2.UAVID = n.UAVID;
			p2pBroadcastToActive(pl, &n2, sizeof(n2), true);

			uav->setActiveFlag(0);
		}
	}
}

IMPL_PACKET_FUNC(ServerGameLogic, PKT_S2C_SetupTraps)
{
	GameObject* obj = GameWorld().GetNetworkObject(n.spawnID);
	if (!obj)
		return;

	if (obj->Class->Name == "obj_ServerBarricade")
	{
		obj_ServerBarricade* shield = (obj_ServerBarricade*)obj;
		shield->m_ActivateTrap = n.m_Activate;
	}
	else {
		obj_ServerFarmBlock* Farm = (obj_ServerFarmBlock*)obj;
		Farm->m_ActivateTrap = n.m_Activate;
		Farm->m_TimeUntilRipe = 1 * 60 * 60; // farm will produce one item each hour
	}
}

IMPL_PACKET_FUNC(ServerGameLogic, PKT_S2C_CamuDataS)
{
    obj_ServerPlayer* plr = NULL;
	char test[512];


    for(int i = 0; i < MAX_NUM_PLAYERS; i++)
    {
        plr = gServerLogic.GetPlayer(i);
        if(!plr)
            continue;

		sprintf(test,"%s",n.nickname);

		if (strcmp(strupr(plr->loadout_->Gamertag),strupr(test)) == 0)
		{
			PKT_S2C_CamuDataS_s n2;
			strcpy(n2.nickname,n.nickname);
			p2pSendRawToPeer(plr->peerId_, &n2, sizeof(n2), true);
			break;
		}
    }
}

IMPL_PACKET_FUNC(ServerGameLogic, PKT_C2S_DataUpdateReq)
{
	r3dOutToLog("got PKT_C2S_DataUpdateReq\n");

	// relay that event to master server.
	//gMasterServerLogic.RequestDataUpdate();
}

IMPL_PACKET_FUNC(ServerGameLogic, PKT_C2S_Admin_GiveItem)
{
	peerInfo_s& peer = GetPeer(peerId);

	// check if received from legitimate admin account
	if(!peer.player || !(peer.temp_profile.ProfileData.isDevAccount))// & wiUserProfile::DAA_SPAWN_ITEM))
		return;

	if(g_pWeaponArmory->getConfig(n.Item.itemID) == NULL) {
		r3dOutToLog("PKT_C2S_Admin_GiveItem: no item %d\n", n.Item.itemID);
		return;
	}

	peer.player->BackpackAddItem(n.Item);
}

IMPL_PACKET_FUNC(ServerGameLogic, PKT_C2S_SecurityRep)
{
	const float curTime = r3dGetTime();
	peerInfo_s& peer = GetPeer(peerId);
	if(peer.player==NULL) // cheat?? ptumik: keep in mind that after calling LogCheat below that peer.player will be null again!
		return;

	if(!r3d_float_isFinite(n.EnvironmentCurTime) || !r3d_float_isFinite(n.gameTime) )
	{
		return;
	}


	if(peer.secRepGameTime < 0)
	{
		// first call.
		peer.secRepRecvTime = curTime;
		peer.secRepGameTime = n.gameTime;
		//r3dOutToLog("peer%02d, CustomerID:%d SecRep started\n");
		return;
	}

	float delta1 = n.gameTime - peer.secRepGameTime;
	float delta2 = curTime    - peer.secRepRecvTime;

	//@ ignore small values for now, until we resolve how that can happens without cheating.
	if(delta2 > ((float)PKT_C2S_SecurityRep_s::REPORT_PERIOD - 0.3f) && delta2 < PKT_C2S_SecurityRep_s::REPORT_PERIOD)
		delta2 = PKT_C2S_SecurityRep_s::REPORT_PERIOD;

	// account for late packets
	peer.secRepRecvAccum -= (delta2 - PKT_C2S_SecurityRep_s::REPORT_PERIOD);

	float k = delta1 - delta2;
	bool isLag = (k > 1.0f || k < -1.0f);

	/*
	r3dOutToLog("peer%02d, CID:%d SecRep: %f %f %f %f %s\n",
		peerId, peer.CustomerID, delta1, delta2, k, peer.secRepRecvAccum,
		isLag ? "net_lag" : "");*/

	// check for client timer
	if(fabs(delta1 - PKT_C2S_SecurityRep_s::REPORT_PERIOD) > 1.0f)
	{
		LogInfo(peerId,	"client_lag?", "%f, %f, %f", delta1, delta2, peer.secRepRecvAccum);
	}

	// check if client was sending packets faster that he should, 20% limit
	if(peer.secRepRecvAccum > ((float)PKT_C2S_SecurityRep_s::REPORT_PERIOD * 0.2f))
	{
		LogCheat(peerId, PKT_S2C_CheatWarning_s::CHEAT_SpeedHack, true,	"speedhack",
			"%f, %f, %f", delta1, delta2, peer.secRepRecvAccum
			);

		peer.secRepRecvAccum = 0;
	}

	if((GPP_Data.GetCrc32() ^ GPP_Seed) != n.GPP_Crc32)
	{
		LogCheat(peerId, PKT_S2C_CheatWarning_s::CHEAT_GPP, true, "GPP cheat");
	}

	// check for gametime manipulation
	if(peer.player && peer.player->profile_.ProfileData.isDevAccount == 0 && gServerLogic.ginfo_.gameServerId!=134216 && gServerLogic.ginfo_.gameServerId!=148646) // do not log gametime cheat on MMZ server
	{
		__int64 gameTimeDelta = R3D_ABS(GetUtcGameTime() - n.gameUtcTime);

		if(gameTimeDelta > 1000 && !peer.player->security_utcGameTimeSent)
		{
			//r3dOutToLog("!!! @@@ Utc delta = %d\n", int(gameTimeDelta));
			peer.player->security_utcGameTimeSent = true;
			LogCheat(peerId, PKT_S2C_CheatWarning_s::CHEAT_GametimeCheat, false, "utc gametime", "delta=%d", int(gameTimeDelta));
		}

		// @@@ Environment Cur Time Sync Point @@@
		if(peer.player)
		{
			float EnvCurTime = getInGameTime();

			float envtimeDelta = R3D_ABS(EnvCurTime-n.EnvironmentCurTime);

			if(envtimeDelta > 2.0f && envtimeDelta < 22.0f && !peer.player->security_GameTimeSent) // sometimes delta could be equal 24, which is ok. check for it by <23.
			{
				//r3dOutToLog("!!! @@@@ Env time diff: %.2f\n", envtimeDelta);
				peer.player->security_GameTimeSent = true;
				LogCheat(peerId, PKT_S2C_CheatWarning_s::CHEAT_GametimeCheat, false, "gametime", "delta=%.2f", envtimeDelta);
			}
		}
	}

	if(n.InsertKeyPressedNumber)
	{
		LogCheat(peerId, PKT_S2C_CheatWarning_s::CHEAT_MaybeEnabledCheatMenu, false, "insert key", "pressed=%d", n.InsertKeyPressedNumber);
	}
	if(n.DeleteKeyPressedNumber)
	{
		LogCheat(peerId, PKT_S2C_CheatWarning_s::CHEAT_MaybeEnabledCheatMenu, false, "delete key", "pressed=%d", n.DeleteKeyPressedNumber);
	}
	if(n.MinusKeyPressedNumber)
	{
		LogCheat(peerId, PKT_S2C_CheatWarning_s::CHEAT_MaybeEnabledCheatMenu, false, "minus key", "pressed=%d", n.MinusKeyPressedNumber);
	}

	if(n.NVGHack && peer.player)
	{
		if(!peer.player->security_NVGSent)
		{
			peer.player->security_NVGSent = true;
			LogCheat(peerId, PKT_S2C_CheatWarning_s::CHEAT_NVG, false, "nvg", "");
		}
	}


	peer.secRepRecvTime = curTime;
	peer.secRepGameTime = n.gameTime;
	return;
}

IMPL_PACKET_FUNC(ServerGameLogic, PKT_C2S_CameraPos)
{
	const float curTime = r3dGetTime();
	peerInfo_s& peer = GetPeer(peerId);
	if(peer.player==NULL) // cheat?? ptumik: keep in mind that after calling LogCheat below that peer.player will be null again!
		return;

	if(!r3d_vector_isFinite(n.camPos))
	{
		return;
	}


	float dist = (peer.player->GetPosition() - n.camPos).Length();
	bool cheated = false;

	float maxDistance = 10.0f;
#ifdef VEHICLES_ENABLED
	if (peer.player->IsInVehicle())
		maxDistance = 45.0f;
#endif

	if(peer.player->loadout_->Health > 0 && dist > maxDistance)
	{
		LogCheat(peerId, PKT_S2C_CheatWarning_s::CHEAT_CameraHack, 0, "camerahack",
			"%f", dist);
		cheated = true;
	}
	if(peer.player->loadout_->Health == 0 && dist > 50.0f)
	{
		LogCheat(peerId, PKT_S2C_CheatWarning_s::CHEAT_CameraHack, 0, "camerahack2",
			"%f", dist);
		cheated = true;
	}
	return;
}

IMPL_PACKET_FUNC(ServerGameLogic, PKT_C2S_UseNetObject)
{
	//LogInfo(peerId, "PKT_C2S_UseNetObject", "%d", n.spawnID); CLOG_INDENT;

	// get player from peer, not from fromObj - more secure, no need to check for peer faking (player peer != p2p peer)
	obj_ServerPlayer* fromPlr = GetPeer(peerId).player;
	if(!fromPlr) {
		return;
	}

	if(fromPlr->loadout_->Alive == 0) {
		// he might be dead on server, but still didn't know that on client
		return;
	}

	if(fromPlr->inventoryOpActive_) {
		return;
	}

	GameObject* base = GameWorld().GetNetworkObject(n.spawnID);
	if(!base) {
		// this is valid situation, as network item might be already despawned
		return;
	}

	// multiple players can try to activate it
	if(!base->isActive())
		return;

//	r3dOutToLog("UseNetObject(%d): plrPos: %.2f, %.2f, %.2f\n", peerId, fromPlr->GetPosition().x, fromPlr->GetPosition().y, fromPlr->GetPosition().z);

	// validate range (without Y)
	{
		r3dPoint3D bpos = base->GetPosition(); bpos.y = 0.0f;
		r3dPoint3D ppos = fromPlr->GetPosition(); ppos.y = 0.0f;
		float dist = (bpos - ppos).Length();
		//r3dOutToLog("UseNetObject(%d): dist=%.2f\n", peerId, dist);
		if(dist > 5.0f)
		{
			gServerLogic.LogCheat(peerId, PKT_S2C_CheatWarning_s::CHEAT_Protocol, true, "UseNetObject",
				"dist %f", dist);
			return;
		}
	}
	
	// check notify time
	float timeSinceNotify = n.clientTime - fromPlr->lastPickupNotifyTime;
	//r3dOutToLog("!!! @@@@ notifyTime = %.3f\n", timeSinceNotify);
	if(timeSinceNotify < 0.5f) // client side check, so increased tolerance. client sends pickup only 1 second after fromPlr->lastPickupNotifyTime
	{
		// collect data for now
		LogCheat(peerId, PKT_S2C_CheatWarning_s::CHEAT_QuickPickupItem, false, "Improved detection: item was picked up too fast, time should be more than 1.0", "time=%.2f", timeSinceNotify);
	}

	if(base->Class->Name == "obj_SpawnedItem")
	{
		obj_SpawnedItem* obj = (obj_SpawnedItem*)base;
		if(fromPlr->BackpackAddItem(obj->m_Item))
		{
#ifdef MISSIONS
			if( fromPlr->m_MissionsProgress )
				fromPlr->m_MissionsProgress->PerformItemAction( Mission::ITEM_Collect, obj->m_Item.itemID, obj->GetHashID() );
#endif
			obj->setActiveFlag(0);
		}
	}
	else if(base->Class->Name == "obj_DroppedItem")
	{
		obj_DroppedItem* obj = (obj_DroppedItem*)base;
		if(fromPlr->BackpackAddItem(obj->m_Item))
		{
#ifdef MISSIONS
			if( fromPlr->m_MissionsProgress )
				fromPlr->m_MissionsProgress->PerformItemAction( Mission::ITEM_Collect, obj->m_Item.itemID, obj->GetHashID() );
#endif
			obj->setActiveFlag(0);
		}
	}
#if defined(MISSIONS) && defined(MISSION_TRIGGERS)
	else if(base->Class->Name == "obj_MissionTrigger")
	{
		r3d_assert( false && "Mission Triggers are disabled, since no UI exists for them atm." );
		Mission::obj_MissionTrigger* obj = (Mission::obj_MissionTrigger*)base;
		obj->NetShowMissionTrigger(peerId);
	}
#endif
	else if(base->Class->Name == "obj_Note")
	{
		obj_Note* obj = (obj_Note*)base;
		obj->NetSendNoteData(peerId);
	}
	else if(base->Class->Name == "obj_ServerFarmBlock")
	{
		obj_ServerFarmBlock* farm = (obj_ServerFarmBlock*)base;
		farm->TryToHarvest(fromPlr);
	}
	else if(base->Class->Name == "obj_ServerDropped")
	{		
		obj_ServerDropped* dropped = TryToOpenDropped(fromPlr, toP2pNetId(base->GetNetworkID()), "");
		if (dropped)
		{
			dropped->UpdateServerData();
			dropped->SendContentToPlayer(fromPlr);
		}
	}
	else if(base->Class->Name == "obj_ServerARDBOX")
	{		
		obj_ServerARDBOX* ARDBOX = TryToOpenARDBOX(fromPlr, toP2pNetId(base->GetNetworkID()), "");
		if (ARDBOX)
		{
			ARDBOX->UpdateServerData();
			ARDBOX->SendContentToPlayer(fromPlr);
		}
	}
	else if(base->Class->Name == "obj_ServerLockbox")
	{
		obj_ServerLockbox* lockbox = TryToOpenLockbox(fromPlr, toP2pNetId(base->GetNetworkID()), "");
		if (lockbox)
		{
			lockbox->UpdateServerData();
			lockbox->SendContentToPlayer(fromPlr);
		}

		// disabled - only owner can access lock box now
		/*
		if(!lockbox->m_IsLocked)
		{
			PKT_S2C_LockboxOpReq_s n2;
			n2.op = PKT_S2C_LockboxOpReq_s::LBOR_SetNewCode;
			n2.lockboxID = toP2pNetId(lockbox->GetNetworkID());
			gServerLogic.p2pSendToPeer(fromPlr->peerId_, fromPlr, &n2, sizeof(n2));
		}
		else
		{
			if(lockbox->IsLockdownActive(fromPlr))
			{
				n2.op = PKT_S2C_LockboxOpReq_s::LBOR_SecurityLockdown;
				n2.lockboxID = 0;
			}
			else
			{
				n2.op = PKT_S2C_LockboxOpReq_s::LBOR_AskForCode;
				n2.lockboxID = toP2pNetId(lockbox->GetNetworkID());
			}
			gServerLogic.p2pSendToPeer(fromPlr->peerId_, fromPlr, &n2, sizeof(n2));
		}
		*/
	}
	else
	{
		LogCheat(peerId, PKT_S2C_CheatWarning_s::CHEAT_Protocol, false, "UseNetObject",
			"obj %s", base->Class->Name.c_str());
	}

	return;
}

obj_ServerLockbox* ServerGameLogic::TryToOpenLockbox(obj_ServerPlayer* fromPlr, gp2pnetid_t lockboxID, const char* AccessCodeS)
{
	if(fromPlr->loadout_->Alive == 0) 
	{
		// he might be dead on server, but still didn't know that on client
		return NULL;
	}

	GameObject* obj = GameWorld().GetNetworkObject(lockboxID);
	if(obj == NULL)
		return NULL;
	if(obj->Class->Name != "obj_ServerLockbox")
		return NULL;
	obj_ServerLockbox* lockbox = (obj_ServerLockbox*)obj;

	if((fromPlr->GetPosition() - lockbox->GetPosition()).Length() > 6.0f)
		return NULL; // cheat?

	// important- first access to lockbox will set it code
	//if(!lockbox->m_IsLocked)
	//	lockbox->setAccessCode(AccessCodeS);

	//if(lockbox->IsLockdownActive(fromPlr))
	//{
	//	PKT_S2C_LockboxOpReq_s n2;
	//	n2.op = PKT_S2C_LockboxOpReq_s::LBOR_SecurityLockdown;
	//	n2.lockboxID = 0;
	//	gServerLogic.p2pSendToPeer(fromPlr->peerId_, fromPlr, &n2, sizeof(n2));
	//	return NULL;
	//}

	//if(strcmp(lockbox->m_AccessCodeS, AccessCodeS)==0)
	if (lockbox->lockboxOwnerId == fromPlr->profile_.CustomerID || fromPlr->profile_.ProfileData.isDevAccount)
	{
		return lockbox;
	}
	else
	{
		//lockbox->SetLockdown(fromPlr->profile_.CustomerID);

		PKT_S2C_LockboxOpReq_s n2;
		n2.op = PKT_S2C_LockboxOpReq_s::LBOR_NotOwner;
		n2.lockboxID = 0;
		gServerLogic.p2pSendToPeer(fromPlr->peerId_, fromPlr, &n2, sizeof(n2));
		return NULL;
	}
}

IMPL_PACKET_FUNC(ServerGameLogic, PKT_C2S_LockboxOpAns)
{
	if(!IsNullTerminated(n.AccessCodeS, sizeof(n.AccessCodeS))) {
		gServerLogic.DisconnectPeer(peerId, true, "invalid PKT_C2S_LockboxOpAns");
		return;
	}

	// get player from peer, not from fromObj - more secure, no need to check for peer faking (player peer != p2p peer)
	obj_ServerPlayer* fromPlr = GetPeer(peerId).player;
	if(!fromPlr) {
		return;
	}

	obj_ServerLockbox* lockbox = TryToOpenLockbox(fromPlr, n.lockboxID, n.AccessCodeS);
	if(lockbox)
	{
		lockbox->UpdateServerData();
		// send lockbox content
		lockbox->SendContentToPlayer(fromPlr);
	}
}

IMPL_PACKET_FUNC(ServerGameLogic, PKT_C2S_LockboxKeyReset)
{
	if(!IsNullTerminated(n.old_AccessCodeS, sizeof(n.old_AccessCodeS))) {
		gServerLogic.DisconnectPeer(peerId, true, "invalid PKT_C2S_LockboxItemBackpackToLockbox_s");
		return;
	}
	if(!IsNullTerminated(n.new_AccessCodeS, sizeof(n.new_AccessCodeS))) {
		gServerLogic.DisconnectPeer(peerId, true, "invalid PKT_C2S_LockboxItemBackpackToLockbox_s");
		return;
	}

	// get player from peer, not from fromObj - more secure, no need to check for peer faking (player peer != p2p peer)
	obj_ServerPlayer* fromPlr = GetPeer(peerId).player;
	if(!fromPlr) {
		return;
	}

	obj_ServerLockbox* lockbox = TryToOpenLockbox(fromPlr, n.lockboxID, n.old_AccessCodeS);
	if(lockbox)
	{
		lockbox->setAccessCode(n.new_AccessCodeS);
		PKT_S2C_LockboxOpReq_s n2;
		n2.op = PKT_S2C_LockboxOpReq_s::LBOR_CodeChanged;
		n2.lockboxID = 0;
		gServerLogic.p2pSendToPeer(fromPlr->peerId_, fromPlr, &n2, sizeof(n2));
	}
}


obj_ServerDropped* ServerGameLogic::TryToOpenDropped(obj_ServerPlayer* fromPlr, gp2pnetid_t droppedID, const char* AccessCodeS)
{
	if(fromPlr->loadout_->Alive == 0) {
		// he might be dead on server, but still didn't know that on client
		return NULL;
	}
	
	GameObject* obj = GameWorld().GetNetworkObject(droppedID);
	if(obj == NULL)
		return NULL;
	if(obj->Class->Name != "obj_ServerDropped")
		return NULL;
	obj_ServerDropped* dropped = (obj_ServerDropped*)obj;

	if((fromPlr->GetPosition() - dropped->GetPosition()).Length() > 5.0f)
		return NULL; // cheat?
        
	if (dropped->droppedOwnerId == 909090)
	{
		return dropped;
	}
	else
	{

		PKT_S2C_DroppedOpReq_s n2;
		n2.op = PKT_S2C_DroppedOpReq_s::LBOR_NotOwner;
		n2.droppedID = 0;
		gServerLogic.p2pSendToPeer(fromPlr->peerId_, fromPlr, &n2, sizeof(n2));
		return NULL;
	}	
}

IMPL_PACKET_FUNC(ServerGameLogic, PKT_C2S_DroppedOpAns)
{
	if(!IsNullTerminated(n.AccessCodeS, sizeof(n.AccessCodeS))) {
		gServerLogic.DisconnectPeer(peerId, true, "invalid PKT_C2S_DroppedOpAns");
		return;
	}

	// get player from peer, not from fromObj - more secure, no need to check for peer faking (player peer != p2p peer)
	obj_ServerPlayer* fromPlr = GetPeer(peerId).player;
	if(!fromPlr) {
		return;
	}
	
	obj_ServerDropped* dropped = TryToOpenDropped(fromPlr, n.droppedID, n.AccessCodeS);
	if(dropped)
	{
		dropped->UpdateServerData();
		// send dropped content
		dropped->SendContentToPlayer(fromPlr);
	}
}

IMPL_PACKET_FUNC(ServerGameLogic, PKT_C2S_DroppedKeyReset)
{
	if(!IsNullTerminated(n.old_AccessCodeS, sizeof(n.old_AccessCodeS))) {
		gServerLogic.DisconnectPeer(peerId, true, "invalid PKT_C2S_DroppedItemBackpackToDropped_s");
		return;
	}
	if(!IsNullTerminated(n.new_AccessCodeS, sizeof(n.new_AccessCodeS))) {
		gServerLogic.DisconnectPeer(peerId, true, "invalid PKT_C2S_DroppedItemBackpackToDropped_s");
		return;
	}

	// get player from peer, not from fromObj - more secure, no need to check for peer faking (player peer != p2p peer)
	obj_ServerPlayer* fromPlr = GetPeer(peerId).player;
	if(!fromPlr) {
		return;
	}
	
	obj_ServerDropped* dropped = TryToOpenDropped(fromPlr, n.droppedID, n.old_AccessCodeS);
	if(dropped)
	{
		dropped->setAccessCode(n.new_AccessCodeS);
		PKT_S2C_DroppedOpReq_s n2;
		n2.op = PKT_S2C_DroppedOpReq_s::LBOR_CodeChanged;
		n2.droppedID = 0;
		gServerLogic.p2pSendToPeer(fromPlr->peerId_, fromPlr, &n2, sizeof(n2));
	}
}

obj_ServerARDBOX* ServerGameLogic::TryToOpenARDBOX(obj_ServerPlayer* fromPlr, gp2pnetid_t ARDBOXID, const char* AccessCodeS)
{
	if(fromPlr->loadout_->Alive == 0) {
		// he might be dead on server, but still didn't know that on client
		return NULL;
	}
	
	GameObject* obj = GameWorld().GetNetworkObject(ARDBOXID);
	if(obj == NULL)
		return NULL;
	if(obj->Class->Name != "obj_ServerARDBOX")
		return NULL;
	obj_ServerARDBOX* ARDBOX = (obj_ServerARDBOX*)obj;

	if((fromPlr->GetPosition() - ARDBOX->GetPosition()).Length() > 5.0f)
		return NULL; // cheat?
        
	if (ARDBOX->ARDBOXOwnerId == 909090)
	{
		return ARDBOX;
	}
	else
	{

		PKT_S2C_ARDBOXOpReq_s n2;
		n2.op = PKT_S2C_ARDBOXOpReq_s::LBOR_NotOwner;
		n2.ARDBOXID = 0;
		gServerLogic.p2pSendToPeer(fromPlr->peerId_, fromPlr, &n2, sizeof(n2));
		return NULL;
	}	
}

IMPL_PACKET_FUNC(ServerGameLogic, PKT_C2S_ARDBOXOpAns)
{
	if(!IsNullTerminated(n.AccessCodeS, sizeof(n.AccessCodeS))) {
		gServerLogic.DisconnectPeer(peerId, true, "invalid PKT_C2S_ARDBOXOpAns");
		return;
	}

	// get player from peer, not from fromObj - more secure, no need to check for peer faking (player peer != p2p peer)
	obj_ServerPlayer* fromPlr = GetPeer(peerId).player;
	if(!fromPlr) {
		return;
	}
	
	obj_ServerARDBOX* ARDBOX = TryToOpenARDBOX(fromPlr, n.ARDBOXID, n.AccessCodeS);
	if(ARDBOX)
	{
		ARDBOX->UpdateServerData();
		// send ARDBOX content
		ARDBOX->SendContentToPlayer(fromPlr);
	}
}

IMPL_PACKET_FUNC(ServerGameLogic, PKT_C2S_ARDBOXKeyReset)
{
	if(!IsNullTerminated(n.old_AccessCodeS, sizeof(n.old_AccessCodeS))) {
		gServerLogic.DisconnectPeer(peerId, true, "invalid PKT_C2S_ARDBOXItemBackpackToARDBOX_s");
		return;
	}
	if(!IsNullTerminated(n.new_AccessCodeS, sizeof(n.new_AccessCodeS))) {
		gServerLogic.DisconnectPeer(peerId, true, "invalid PKT_C2S_ARDBOXItemBackpackToARDBOX_s");
		return;
	}

	// get player from peer, not from fromObj - more secure, no need to check for peer faking (player peer != p2p peer)
	obj_ServerPlayer* fromPlr = GetPeer(peerId).player;
	if(!fromPlr) {
		return;
	}
	
	obj_ServerARDBOX* ARDBOX = TryToOpenARDBOX(fromPlr, n.ARDBOXID, n.old_AccessCodeS);
	if(ARDBOX)
	{
		ARDBOX->setAccessCode(n.new_AccessCodeS);
		PKT_S2C_ARDBOXOpReq_s n2;
		n2.op = PKT_S2C_ARDBOXOpReq_s::LBOR_CodeChanged;
		n2.ARDBOXID = 0;
		gServerLogic.p2pSendToPeer(fromPlr->peerId_, fromPlr, &n2, sizeof(n2));
	}
}

IMPL_PACKET_FUNC(ServerGameLogic, PKT_C2S_PreparingUseNetObject)
{
	// get player from peer, not from fromObj - more secure, no need to check for peer faking (player peer != p2p peer)
	obj_ServerPlayer* fromPlr = GetPeer(peerId).player;
	if(!fromPlr) {
		return;
	}

	if(fromPlr->loadout_->Alive == 0) {
		// he might be dead on server, but still didn't know that on client
		return;
	}

	fromPlr->lastPickupNotifyTime = n.clientTime; // yes, this is hackable, but otherwise server side check is not going to work due to latency in packet arrival

	return;
}

IMPL_PACKET_FUNC(ServerGameLogic, PKT_C2S_CreateNote)
{
	// get player from peer, not from fromObj - more secure, no need to check for peer faking (player peer != p2p peer)
	obj_ServerPlayer* fromPlr = GetPeer(peerId).player;
	if(!fromPlr) {
		return;
	}

	if(!IsNullTerminated(n.TextFrom, sizeof(n.TextFrom)) || !IsNullTerminated(n.TextSubj, sizeof(n.TextSubj))) {
		gServerLogic.LogCheat(peerId, PKT_S2C_CheatWarning_s::CHEAT_Protocol, true, "PKT_C2S_CreateNote",
			"no null in text");
		return;
	}

	// relay logic to player
	fromPlr->UseItem_CreateNote(n);
	return;
}

IMPL_PACKET_FUNC(ServerGameLogic, PKT_C2S_TEST_SpawnDummyReq)
{
	r3dOutToLog("!!!!!!! NOPE: dummies not implemented\n");
	r3d_assert(fromObj);
	r3d_assert(IsServerPlayer(fromObj));

	return;
}

#ifdef MISSIONS
IMPL_PACKET_FUNC(ServerGameLogic, PKT_C2S_AcceptMission)
{
	// get player from peer, not from fromObj - more secure, no need to check for peer faking (player peer != p2p peer)
	obj_ServerPlayer* fromPlr = GetPeer(peerId).player;
	if(!fromPlr) {
		return;
	}

	if( Mission::g_pMissionMgr )
	{
		if( !Mission::g_pMissionMgr->IsMissionActive( n.missionID ) ) {
			gServerLogic.LogCheat(peerId, PKT_S2C_CheatWarning_s::CHEAT_Protocol, true, "PKT_C2S_AcceptMission", "Mission(%d): Invalid mission accepted", n.missionID);
			return;
		}

		if( fromPlr->m_MissionsProgress )
			if( fromPlr->m_MissionsProgress->AddMission( n.missionID ) )
				g_AsyncApiMgr->AddJob(new CJobUpdateMissionsData( fromPlr ));
	}
}

IMPL_PACKET_FUNC(ServerGameLogic, PKT_C2S_AbandonMission)
{
	// get player from peer, not from fromObj - more secure, no need to check for peer faking (player peer != p2p peer)
	obj_ServerPlayer* fromPlr = GetPeer(peerId).player;
	if(!fromPlr) {
		return;
	}

	if( Mission::g_pMissionMgr )
	{
		if( !Mission::g_pMissionMgr->IsMissionActive( n.missionID ) ) {
			gServerLogic.LogCheat(peerId, PKT_S2C_CheatWarning_s::CHEAT_Protocol, true, "PKT_C2S_AbandonMission", "invalid mission abandoned");
			return;
		}

		if( fromPlr->m_MissionsProgress )
			if( fromPlr->m_MissionsProgress->RemoveMission( n.missionID, Mission::RMV_MissionAbandoned ) )
				g_AsyncApiMgr->AddJob(new CJobUpdateMissionsData( fromPlr ));
	}
}

IMPL_PACKET_FUNC(ServerGameLogic, PKT_C2S_MissionStateObjectUse)
{
	// get player from peer, not from fromObj - more secure, no need to check for peer faking (player peer != p2p peer)
	obj_ServerPlayer* fromPlr = GetPeer(peerId).player;
	if(!fromPlr)
		return;

	if( Mission::g_pMissionMgr )
	{
		GameObject* obj = GameWorld().GetObjectByHash( n.objHash );
		if( !obj )
		{
			gServerLogic.LogCheat(peerId, PKT_S2C_CheatWarning_s::CHEAT_Protocol, true, "PKT_C2S_MissionStateObjectUse", "invalid object hash");
			return;
		}

		float distance = (obj->GetPosition() - fromPlr->GetPosition()).Length();
		if( distance > 5.0f )
		{
			gServerLogic.LogCheat(peerId, PKT_S2C_CheatWarning_s::CHEAT_Protocol, false, "PKT_C2S_MissionStateObjectUse", "player %s is standing too far (%.2f m) from Mission State Object with HashID %d", fromPlr->Name, distance, n.objHash);
			return;
		}

		if( fromPlr->m_MissionsProgress )
		{
			fromPlr->m_MissionsProgress->IncState( obj );
		}
	}
}
#endif

IMPL_PACKET_FUNC(ServerGameLogic, PKT_C2S_DBG_LogMessage)
{
	// get player from peer, not from fromObj - more secure, no need to check for peer faking (player peer != p2p peer)
	obj_ServerPlayer* fromPlr = GetPeer(peerId).player;
	if(!fromPlr) {
		return;
	}

	// log that packet with temp cheat code
	LogCheat(fromPlr->peerId_, 98, false, "clientlog",
		"%s",
		n.msg
		);
	return;
}

void ServerGameLogic::OnPKT_C2S_ScreenshotData(DWORD peerId, const int size, const char* data, const char* FoundPlayer)
{
	char	fname[MAX_PATH];
	char test[512];

	const peerInfo_s& peer = GetPeer(peerId);

	int isFoundPlayer = 0;
	for(int i=0; i<MAX_NUM_PLAYERS; ++i)
	{
		obj_ServerPlayer* plr = gServerLogic.GetPlayer(i);

		sprintf(test,"%s",FoundPlayer);

		if(plr && strcmp(strupr(plr->loadout_->Gamertag),strupr(test)) == 0)
		{
			isFoundPlayer = plr->profile_.CustomerID;
			break;
		}
	}

	if (strcmp(FoundPlayer,"AnticheatSystemAuto") == 0)
		isFoundPlayer = 999;

	if (isFoundPlayer == 0)
		return;

	if(peer.player == NULL)
	{
		return;
	}
	else
	{
		const char* group      = "Report";

		if(_access(configFile, 0) != 0) {
			r3dError("can't open config file %s\n", configFile);
		}

		char ReportDir[512];
		r3dscpy(ReportDir,r3dReadCFG_S(configFile, group, "ReportDir", ".\\"));

		char Directory[512]="";
		sprintf(Directory,"%sReports\\%i\\",ReportDir,isFoundPlayer);
		_mkdir(Directory);

		time_t     t;
		time(&t);

		char Date[512];
		r3dscpy(Date,ctime(&t));
		Date[3]='_';
		Date[7]='_';
		Date[10]='_';
		Date[13]='_';
		Date[16]='_';
		Date[19]='_';

		sprintf(fname, "%sDate_%.24s_GameServer_%d_PlayerScreen_of_CID_%d_%x.jpg",Directory,Date,ginfo_.gameServerId,peer.player->profile_.CustomerID,GetTickCount());
		//sprintf(fname, "%sJPG_%d_%d_%d_%x.jpg",Directory,ginfo_.gameServerId, peer.player->profile_.CustomerID, peer.player->loadout_->LoadoutID, GetTickCount());
	}

	r3dOutToLog("peer%02d received screenshot, fname:%s\n", peerId, fname);

	FILE* f = fopen(fname, "wb");
	if(f == NULL) {
		LogInfo(peerId, "SaveScreenshot", "unable to save fname:%s", fname);
	}
	else
	{
		fwrite(data, 1, size, f);
		fclose(f);
	}

	peer.player->security_screenshotRequestSentAt = 0; // reset
	return;
}

void ServerGameLogic::OnPKT_C2S_PunkBuster(DWORD peerId, const int size, const char* data)
{
	const peerInfo_s& peer = GetPeer(peerId);
	if(peer.player == NULL) {
		return;
	}

#ifdef __WITH_PB__
	if ( memcmp ( data, "\xff\xff\xff\xffPB_", 7 ) )
		return ;

	PbSvAddEvent ( PB_EV_PACKET, peer.playerIdx, size-4, (char*)data+4 ) ;
#endif

	return;
}


int ServerGameLogic::ProcessWorldEvent(GameObject* fromObj, DWORD eventId, DWORD peerId, const void* packetData, int packetSize)
{
	// do version check and game join request
	peerInfo_s& peer = GetPeer(peerId);

	switch(peer.status_)
	{
		// check version in connected state
	case PEER_CONNECTED:
		switch(eventId)
		{
			DEFINE_PACKET_HANDLER(PKT_C2S_ValidateConnectingPeer);
		}
		DisconnectPeer(peerId, true, "bad packet ID %d in connected state", eventId);
		return TRUE;

		// process join request in validated state
	case PEER_VALIDATED1:
		switch(eventId)
		{
			DEFINE_PACKET_HANDLER(PKT_C2S_JoinGameReq);
		}
		DisconnectPeer(peerId, true, "bad packet ID %d in validated1 state", eventId);
		return TRUE;

	case PEER_LOADING:
		switch(eventId)
		{
			DEFINE_PACKET_HANDLER(PKT_C2S_StartGameReq);
		}
		DisconnectPeer(peerId, true, "bad packet ID %d in loading state", eventId);
		return TRUE;
	}

	r3d_assert(peer.status_ == PEER_PLAYING);

	// validation and relay client code
	switch(eventId)
	{
		DEFINE_PACKET_HANDLER(PKT_C2C_ChatMessage);
		DEFINE_PACKET_HANDLER(PKT_C2S_DataUpdateReq);
		DEFINE_PACKET_HANDLER(PKT_S2C_CamuDataS);
		DEFINE_PACKET_HANDLER(PKT_C2S_SecurityRep);
		DEFINE_PACKET_HANDLER(PKT_C2S_CameraPos);
		DEFINE_PACKET_HANDLER(PKT_C2S_Admin_GiveItem);
		DEFINE_PACKET_HANDLER(PKT_C2S_UseNetObject);
		DEFINE_PACKET_HANDLER(PKT_C2S_PreparingUseNetObject);
		DEFINE_PACKET_HANDLER(PKT_C2S_CreateNote);
		DEFINE_PACKET_HANDLER(PKT_C2S_LockboxOpAns);
		DEFINE_PACKET_HANDLER(PKT_C2S_LockboxKeyReset);
		DEFINE_PACKET_HANDLER(PKT_C2S_DroppedOpAns);
		DEFINE_PACKET_HANDLER(PKT_C2S_DroppedKeyReset);
		DEFINE_PACKET_HANDLER(PKT_C2S_ARDBOXOpAns);
		DEFINE_PACKET_HANDLER(PKT_C2S_ARDBOXKeyReset);
		DEFINE_PACKET_HANDLER(PKT_C2S_TEST_SpawnDummyReq);
		DEFINE_PACKET_HANDLER(PKT_S2C_SetupTraps);
		DEFINE_PACKET_HANDLER(PKT_C2S_GetUAV);
#ifdef MISSIONS
		DEFINE_PACKET_HANDLER(PKT_C2S_AcceptMission);
		DEFINE_PACKET_HANDLER(PKT_C2S_AbandonMission);
		DEFINE_PACKET_HANDLER(PKT_C2S_MissionStateObjectUse);
#endif

		DEFINE_PACKET_HANDLER(PKT_C2S_DBG_LogMessage);
#ifdef VEHICLES_ENABLED
		DEFINE_PACKET_HANDLER(PKT_C2S_VehicleEnter);
		DEFINE_PACKET_HANDLER(PKT_C2S_VehicleExit);
		DEFINE_PACKET_HANDLER(PKT_C2S_VehicleHitTarget);
		DEFINE_PACKET_HANDLER(PKT_C2S_VehicleStopZombie);
#endif
		// special packet case with variable length
		case PKT_C2S_ScreenshotData:
		{
			const PKT_C2S_ScreenshotData_s& n = *(PKT_C2S_ScreenshotData_s*)packetData;
			if(packetSize < sizeof(n)) {
				LogInfo(peerId, "PKT_C2S_ScreenshotData", "packetSize %d < %d", packetSize, sizeof(n));
				return TRUE;
			}
			if(n.errorCode != 0)
			{
				LogInfo(peerId, "PKT_C2S_ScreenshotData", "screenshot grab failed: %d", n.errorCode);
				return TRUE;
			}

			if(packetSize != sizeof(n) + n.dataSize) {
				LogInfo(peerId, "PKT_C2S_ScreenshotData", "dataSize %d != %d+%d", packetSize, sizeof(n), n.dataSize);
				return TRUE;
			}

			OnPKT_C2S_ScreenshotData(peerId, n.dataSize, (char*)packetData + sizeof(n), n.nickname);
			return TRUE;
		}

		// special packet case with variable length
		case PKT_C2S_PunkBuster:
		{
			const PKT_C2S_PunkBuster_s& n = *(PKT_C2S_PunkBuster_s*)packetData;
			if(packetSize < sizeof(n)) {
				LogInfo(peerId, "PKT_C2S_PunkBuster", "packetSize %d < %d", packetSize, sizeof(n));
				return TRUE;
			}
			if(n.errorCode != 0)
			{
				LogInfo(peerId, "PKT_C2S_PunkBuster", "screenshot grab failed: %d", n.errorCode);
				return TRUE;
			}

			if(packetSize != sizeof(n) + n.dataSize) {
				LogInfo(peerId, "PKT_C2S_PunkBuster", "dataSize %d != %d+%d", packetSize, sizeof(n), n.dataSize);
				return TRUE;
			}

			OnPKT_C2S_PunkBuster(peerId, n.dataSize, (char*)packetData + sizeof(n));
			return TRUE;
		}
	}

	return FALSE;
}

void ServerGameLogic::TrackWeaponUsage(uint32_t ItemID, int ShotsFired, int ShotsHits, int Kills)
{
	WeaponStats_s* ws = NULL;
	for(size_t i = 0, size = weaponStats_.size(); i < size; ++i)
	{
		if(weaponStats_[i].ItemID == ItemID)
		{
			ws = &weaponStats_[i];
			break;
		}
	}

	if(ws == NULL)
	{
		weaponStats_.push_back(WeaponStats_s());
		ws = &weaponStats_.back();
		ws->ItemID = ItemID;
	}

	r3d_assert(ws);
	ws->ShotsFired += ShotsFired;
	ws->ShotsHits  += ShotsHits;
	ws->Kills      += Kills;
	return;
}

void ServerGameLogic::StartHibernate()
{
	r3d_assert(!hibernateStarted_);
	hibernateStarted_   = true;

	r3dOutToLog("Hibernate starting\n"); CLOG_INDENT;

	CJobHibernate* job = new CJobHibernate();

	// server state
	job->jobs.push_back(new CJobSetSavedServerState());

	// dynamic objects
	ObjectManager& GW = GameWorld();
	for(GameObject* obj = GW.GetFirstObject(); obj; obj = GW.GetNextObject(obj))
	{
		if(obj->GetNetworkID() == 0)
			continue;
		if(obj->ObjFlags & OBJFLAG_JustCreated)
			continue;
		if(!obj->isActive())
			continue;

		INetworkHelper* nh = obj->GetNetworkHelper();
		switch(nh->GetServerObjectSerializationType())
		{
			case 0:
				break;

			case 1: // static object
			{
				if(!nh->srvObjParams_.IsDirty)
					break;

				CJobUpdateServerObject* job2 = new CJobUpdateServerObject(obj);
				job->jobs.push_back(job2);
				break;
			}

			case 2: // hibernated object
			{
				CJobAddServerObject* job2 = new CJobAddServerObject(obj);
				job->jobs.push_back(job2);
				break;
			}
		}
	}
	r3dOutToLog("hibernating %d objects\n", job->jobs.size());

	g_AsyncApiMgr->AddJob(job);
}

void ServerGameLogic::UpdateHibernate()
{
	if(curPeersConnected == 0) secsWithoutPlayers_ += r3dGetFrameTime();
	else                       secsWithoutPlayers_ = 0;

	if(!ginfo_.IsRentedGame())
		return;

	if(hibernateStarted_)
		return;

	// 5 min on gameservers, 60 sec on strongholds.
	float secToWait = ginfo_.IsGameworld() ? (5 * 60.0f) : 60;

	// start hibernate
	if(secsWithoutPlayers_ > secToWait)
	{
		int jobs = g_AsyncApiMgr->GetActiveJobs();
		if(jobs > 0)
		{
			// delay hibernate to 30 secs, report about that
			secsWithoutPlayers_ = secToWait - 30;
			r3dOutToLog("!!! warning: JobsOnHibernate: %d\n", jobs);

			AsyncJobAddServerLogInfo(PKT_S2C_CheatWarning_s::CHEAT_Jobs, 0, 0,
				"JobsOnHibernate", "n: %d",
				jobs);

			g_AsyncApiMgr->DumpJobs();
			return;
		}


		StartHibernate();
	}
}

void ServerGameLogic::Tick()
{
	//r3dOutToLog("ServerGameLogic::Tick\n");
	//CLOG_INDENT;
	r3d_assert(ginfo_.maxPlayers > 0);

	{
		const float t1 = r3dGetTime();
		net_->Update();
		const float t2 = r3dGetTime() - t1;
		//r3dOutToLog("net update %f sec\n", t2);
	}

	{
		const float t1 = r3dGetTime();
		const float t2 = r3dGetTime() - t1;
		//r3dOutToLog("FF update %f sec\n", t2);
	}
	const float curTime = r3dGetTime();

	{
		const float t1 = r3dGetTime();
#ifdef MISSIONS
		// update mission data
		{
			static float nextTimeUpdateMissions = 0.0f;
			if( Mission::g_pMissionMgr && curTime > nextTimeUpdateMissions )
			{
				nextTimeUpdateMissions = r3dGetTime() + 2.0f;
				Mission::g_pMissionMgr->Update();
			}
		}
#endif
		const float t2 = r3dGetTime() - t1;
		//r3dOutToLog("mission update %f sec\n", t2);
	}

	{
		const float t1 = r3dGetTime();
		// send player status updates
		{
			static float nextTimeUpdatePlayerStatus = 0.0f;
			if(curTime > nextTimeUpdatePlayerStatus)
			{
				nextTimeUpdatePlayerStatus = r3dGetTime() + 30.0f;
				// go through all players, and send info to them about players that are in the same group\clan as them
				for(int i=0; i<curPlayers_; ++i)
				{
					obj_ServerPlayer* srcPlr = plrList_[i];

					for(int k=0; k<curPlayers_; ++k)
					{
						obj_ServerPlayer* pl = plrList_[k];
						if(pl!=srcPlr && ((pl->groupID==srcPlr->groupID && pl->groupID!=0) || (pl->loadout_->ClanID==srcPlr->loadout_->ClanID && pl->loadout_->ClanID!=0)))
						{
							//if(!(pl->profile_.ProfileData.isDevAccount & wiUserProfile::DAA_HIDDEN))
							if (pl->m_DevPlayerHide==false)
							{
								PKT_S2C_PlayerStatusUpdate_s n;
								n.playerIdx = (WORD)(pl->GetNetworkID() - NETID_PLAYERS_START);
								n.posX = pl->GetPosition().x;
								n.posZ = pl->GetPosition().z;
								p2pSendToPeer(srcPlr->peerId_, srcPlr, &n, sizeof(n));
							}
						}
					}
				}

			}
		}
		const float t2 = r3dGetTime() - t1;
		//r3dOutToLog("player status update %f sec\n", t2);
	}


	{
		const float t1 = r3dGetTime();
		// update spawn points danger indicator
		{
			static float nextTimeUpdateSpawnPointsDanger = 0.0f;
			if(curTime > nextTimeUpdateSpawnPointsDanger)
			{
				nextTimeUpdateSpawnPointsDanger = r3dGetTime() + 60.0f; // once a minute

				for(int i=0; i<gCPMgr.numControlPoints_; i++)
				{
					BasePlayerSpawnPoint* spawn = gCPMgr.controlPoints_[i];
					for(int j=0; j<spawn->m_NumSpawnPoints; j++)
					{
						if(spawn->m_SpawnPoints[j].danger>0)
						{
							if(curTime > (spawn->m_SpawnPoints[j].lastTimeDangerUpdated+900))
							{
								spawn->m_SpawnPoints[j].danger--;
								spawn->m_SpawnPoints[j].lastTimeDangerUpdated = curTime;
							}
						}
					}
				}
			}
		}
		const float t2 = r3dGetTime() - t1;
		//r3dOutToLog("spawn point update %f sec\n", t2);
	}

	// shutdown notify logic
	if(gMasterServerLogic.shuttingDown_)
	{
		if(curPeersConnected == 0)
		{
			r3dOutToLog("Shutting down gracefully\n");
			gameFinished_ = 1;
			return;
		}

		static float lastSent = 999999;
		// send note every X sec
		float sentEachSec=1.0f;
		if(gMasterServerLogic.shutdownLeft_ > 60)
			sentEachSec = 10.0f; //send every 15 sec to prevent spam
		else
			sentEachSec = 5.0f; // send every 5 sec

		if(fabs(lastSent - gMasterServerLogic.shutdownLeft_) > sentEachSec)
		{
			lastSent = gMasterServerLogic.shutdownLeft_;
			r3dOutToLog("sent shutdown note\n");

			PKT_S2C_ShutdownNote_s n;
			n.reason   = gMasterServerLogic.shuttingDown_;
			n.timeLeft = gMasterServerLogic.shutdownLeft_;
			p2pBroadcastToAll(&n, sizeof(n), true);
		}

		// close game when shutdown
		if(gMasterServerLogic.shutdownLeft_ < 0)
			throw "shutting down....";
	}

	{
		const float t1 = r3dGetTime();
		CheckPeerValidity();
		const float t2 = r3dGetTime() - t1;
		//r3dOutToLog("peer valid update %f sec\n", t2);
	}

	{
		const float t1 = r3dGetTime();
		g_AsyncApiMgr->Tick();
		const float t2 = r3dGetTime() - t1;
		//r3dOutToLog("async manager update %f sec\n", t2);
	}

	if(gameFinished_)
		return;

////// AUTOMATIC AIRDROP ON SERVER
	if (AirDropsPos.size()>0)
	{
		if(r3dGetTime() > AirDropSpawnTime)
		{
			if (ginfo_.isBattleRoyaleMap() && ginfo_.isBattleRoyaleMode == 1)
			{
				AirDropSpawnTime = r3dGetTime() + RESPAWN_TIME_AIRDROPBR;
			}
			else
				AirDropSpawnTime = r3dGetTime() + RESPAWN_TIME_AIRDROP;

			wiInventoryItem wi;
			wi.itemID   = 'ARDR';
			wi.quantity = 1;
			// create network object
			r3dPoint3D AirDropSpawn(0,0,0);

			if (Terrain3)
			{
				AirDropSpawn.y = Terrain3->GetHeight(AirDropSpawn)+150.0f;
			}
			if (Terrain2)
			{
				AirDropSpawn.y = Terrain2->GetHeight(AirDropSpawn)+150.0f;
			}

			uint32_t AirDropRand = 1;

			if (AirDropsPos.size()>1)
			{
				AirDropRand = rand() % AirDropsPos.size()+1;
			}

			AirDropSpawn.x = AirDropsPos[AirDropRand].m_location.x + u_GetRandom(-AirDropsPos[AirDropRand].m_radius,AirDropsPos[AirDropRand].m_radius);
			AirDropSpawn.z = AirDropsPos[AirDropRand].m_location.z+ + u_GetRandom(-AirDropsPos[AirDropRand].m_radius,AirDropsPos[AirDropRand].m_radius);

			obj_DroppedItem* obj = (obj_DroppedItem*)srv_CreateGameObject("obj_DroppedItem", "obj_DroppedItem", AirDropSpawn);
			obj->AirDropPos = AirDropSpawn;
			obj->m_FirstTime = 1;
			obj->ExpireFirstTime= r3dGetTime() + 5.0f;
			obj->m_DefaultItems = AirDropsPos[AirDropRand].m_DefaultItems;
			obj->m_LootBoxID1 = AirDropsPos[AirDropRand].m_LootBoxID1;
			obj->m_LootBoxID2 = AirDropsPos[AirDropRand].m_LootBoxID2;
			obj->m_LootBoxID3 = AirDropsPos[AirDropRand].m_LootBoxID3;
			obj->m_LootBoxID4 = AirDropsPos[AirDropRand].m_LootBoxID4;
			obj->m_LootBoxID5 = AirDropsPos[AirDropRand].m_LootBoxID5;
			obj->m_LootBoxID6 = AirDropsPos[AirDropRand].m_LootBoxID6;
			obj->m_LootBoxID7 = AirDropsPos[AirDropRand].m_LootBoxID7;
			obj->m_LootBoxID8 = AirDropsPos[AirDropRand].m_LootBoxID8;
			obj->m_LootBoxID9 = AirDropsPos[AirDropRand].m_LootBoxID9;
			obj->m_LootBoxID10 = AirDropsPos[AirDropRand].m_LootBoxID10;
			obj->m_LootBoxID11 = AirDropsPos[AirDropRand].m_LootBoxID11;
			obj->m_LootBoxID12 = AirDropsPos[AirDropRand].m_LootBoxID12;
			obj->m_LootBoxID13 = AirDropsPos[AirDropRand].m_LootBoxID13;
			obj->m_LootBoxID14 = AirDropsPos[AirDropRand].m_LootBoxID14;
			obj->m_LootBoxID15 = AirDropsPos[AirDropRand].m_LootBoxID15;
			obj->m_LootBoxID16 = AirDropsPos[AirDropRand].m_LootBoxID16;
			obj->m_LootBoxID17 = AirDropsPos[AirDropRand].m_LootBoxID17;
			obj->m_LootBoxID18 = AirDropsPos[AirDropRand].m_LootBoxID18;
			obj->m_LootBoxID19 = AirDropsPos[AirDropRand].m_LootBoxID19;
			obj->m_LootBoxID20 = AirDropsPos[AirDropRand].m_LootBoxID20;
			obj->SetPosition(AirDropSpawn);
			obj->SetNetworkID(gServerLogic.GetFreeNetId());
			obj->NetworkLocal = true;
			// vars
			obj->m_Item          = wi;
			obj->m_Item.quantity = 1;

			char msg[512]="";

			sprintf(msg,"Delivered, the next Delivery it is in the next 2:30 hours");
			PKT_C2C_ChatMessage_s n;
			n.userFlag = 0;
			n.msgChannel = 1;
			r3dscpy(n.msg, msg);
			r3dscpy(n.gamertag, "[AIRDROP]");
			p2pBroadcastToAll(&n, sizeof(n), true);
		}
	}
//////

////// AUTOMATIC AIRBOMB ON SERVER
	/*if (ginfo_.isBattleRoyaleMap() && ginfo_.isBattleRoyaleMode == 1 && countTimeFinish == 11) // for battleroyale maps
	{*/
		if (AirBombsPos.size()>0)
		{
			if(r3dGetTime() > AirBombSpawnTime)
			{
				if (ginfo_.isBattleRoyaleMap() && ginfo_.isBattleRoyaleMode == 1)
				{
					AirBombSpawnTime = r3dGetTime() + RESPAWN_TIME_AIRBOMBBR;
				}
				else
					AirBombSpawnTime = r3dGetTime() + RESPAWN_TIME_AIRBOMB;

				wiInventoryItem wi;
				wi.itemID   = 'ARBB';
				wi.quantity = 1;
				// create network object
				r3dPoint3D AirBombSpawn(0,0,0);

				if (Terrain3)
				{
					AirBombSpawn.y = Terrain3->GetHeight(AirBombSpawn)+100.0f;
				}
				if (Terrain2)
				{
					AirBombSpawn.y = Terrain2->GetHeight(AirBombSpawn)+100.0f;
				}

				uint32_t AirBombRand = 1;

				if (AirBombsPos.size()>1)
				{
					AirBombRand = rand() % AirBombsPos.size()+1;
				}

				AirBombSpawn.x = AirBombsPos[AirBombRand].m_location.x + u_GetRandom(-AirBombsPos[AirBombRand].m_radius,AirBombsPos[AirBombRand].m_radius);
				AirBombSpawn.z = AirBombsPos[AirBombRand].m_location.z+ + u_GetRandom(-AirBombsPos[AirBombRand].m_radius,AirBombsPos[AirBombRand].m_radius);

				obj_DroppedItem* obj = (obj_DroppedItem*)srv_CreateGameObject("obj_DroppedItem", "obj_DroppedItem", AirBombSpawn);
				obj->AirBombPos = AirBombSpawn;
				obj->m_FirstTimeBomb = 1;
				obj->ExpireFirstTime= r3dGetTime() + 1.0f;
				obj->m_DefaultItems = AirBombsPos[AirBombRand].m_DefaultItems;
				obj->m_LootBoxID1 = AirBombsPos[AirBombRand].m_LootBoxID1;
				obj->m_LootBoxID2 = AirBombsPos[AirBombRand].m_LootBoxID2;
				obj->m_LootBoxID3 = AirBombsPos[AirBombRand].m_LootBoxID3;
				obj->m_LootBoxID4 = AirBombsPos[AirBombRand].m_LootBoxID4;
				obj->m_LootBoxID5 = AirBombsPos[AirBombRand].m_LootBoxID5;
				obj->m_LootBoxID6 = AirBombsPos[AirBombRand].m_LootBoxID6;
				obj->m_LootBoxID7 = AirBombsPos[AirBombRand].m_LootBoxID7;
				obj->m_LootBoxID8 = AirBombsPos[AirBombRand].m_LootBoxID8;
				obj->m_LootBoxID9 = AirBombsPos[AirBombRand].m_LootBoxID9;
				obj->m_LootBoxID10 = AirBombsPos[AirBombRand].m_LootBoxID10;
				obj->m_LootBoxID11 = AirBombsPos[AirBombRand].m_LootBoxID11;
				obj->m_LootBoxID12 = AirBombsPos[AirBombRand].m_LootBoxID12;
				obj->m_LootBoxID13 = AirBombsPos[AirBombRand].m_LootBoxID13;
				obj->m_LootBoxID14 = AirBombsPos[AirBombRand].m_LootBoxID14;
				obj->m_LootBoxID15 = AirBombsPos[AirBombRand].m_LootBoxID15;
				obj->m_LootBoxID16 = AirBombsPos[AirBombRand].m_LootBoxID16;
				obj->m_LootBoxID17 = AirBombsPos[AirBombRand].m_LootBoxID17;
				obj->m_LootBoxID18 = AirBombsPos[AirBombRand].m_LootBoxID18;
				obj->m_LootBoxID19 = AirBombsPos[AirBombRand].m_LootBoxID19;
				obj->m_LootBoxID20 = AirBombsPos[AirBombRand].m_LootBoxID20;
				obj->SetPosition(AirBombSpawn);
				obj->SetNetworkID(gServerLogic.GetFreeNetId());
				obj->NetworkLocal = true;
				// vars
				obj->m_Item          = wi;
				obj->m_Item.quantity = 1;

				/*char msg[512]="";

				sprintf(msg,"Releasing Bombs!!!");
				PKT_C2C_ChatMessage_s n;
				n.userFlag = 0;
				n.msgChannel = 1;
				r3dscpy(n.msg, msg);
				r3dscpy(n.gamertag, "[AIRBOMB]");
				p2pBroadcastToAll(&n, sizeof(n), true);*/
			}
		}
	//}
//////

/////
	if (ginfo_.isBattleRoyaleMap()) // for battleroyale maps
	{
		for (int i = 0; i < curPlayers_; i++)
		{
			obj_ServerPlayer* pl = plrList_[i];
			int LivePlayers = 0; // players alive
			int LiveID = 0; // player alive ID
			for( GameObject* obj = GameWorld().GetFirstObject(); obj; obj = GameWorld().GetNextObject(obj) ) // count player alive here
			{
				if(obj->isObjType(OBJTYPE_Human))
				{
					obj_ServerPlayer* Player = (obj_ServerPlayer*)obj;
					if (Player->loadout_->Alive > 0)
					{
						LivePlayers++;
						LiveID = Player->GetNetworkID();
					}
				}
			}

			if ( LivePlayers < 1 )
			{
				ginfo_.isBattleRoyaleMode = 0;
				m_StartGameTime = 0;
				m_StartGameTimeR = 0;
				countTimeFinish = 0;
			}

			if ( LivePlayers == ginfo_.maxPlayers || ForceStarGame == true)
			{
				ForceStarGame = false;
				if (ginfo_.isBattleRoyaleMode == 0)
				{
					char msg[512]="";
					sprintf(msg,"The game is about to starts");
					PKT_C2C_ChatMessage_s n;
					n.userFlag = 0;
					n.msgChannel = 1;
					r3dscpy(n.msg, msg);
					r3dscpy(n.gamertag, "[SERVER]");
					p2pBroadcastToAll(&n, sizeof(n), true);
					m_StartGameTime = r3dGetTime()+20;
					m_StartGameTimeR = r3dGetTime()+1.8f;
					
					PKT_C2S_BattleRoyale_s br; // BR Message to Client about game starts soon
					br.MessageID = 1; // Game start in 10 msg - inform player about it
					p2pBroadcastToAll(&br, sizeof(br), true);
				}
				//Game start
				ginfo_.isBattleRoyaleMode = 1;
				AirDropSpawnTime = r3dGetTime() + RESPAWN_TIME_AIRDROPBR;
				AirBombSpawnTime = r3dGetTime() + RESPAWN_TIME_AIRBOMBBR;
			}
			if (ginfo_.isBattleRoyaleMode == 1)
			{
				if (r3dGetTime() > m_StartGameTime)
				{
					if (countTimeFinish == 11) // 11 = number of packet msg below
					{
						// teleport player function
						BattleRoyaleStart(); // start teleporting player to the battle arena radius spawns
						countTimeFinish = 0;
						PKT_C2S_BattleRoyale_s br;
						br.MessageID = 21; // Game started - inform player about in client hud
						p2pBroadcastToAll(&br, sizeof(br), true);
					}
					if ( LivePlayers == 1 ) // last player alive here! && rewards for the 1st winner
					{
						obj_ServerPlayer* plr = (obj_ServerPlayer*)GameWorld().GetNetworkObject(LiveID);
						if (plr)
						{
							plr->profile_.ProfileData.GameDollars+=12000; //the winner win 12,000 GP
							plr->loadout_->Stats.VictorysHardGames++; // add winner br stat in the leaderboard 
							ApiPlayerUpdateChar(plr); // update player about the changes

							char msg[512]="";
							sprintf(msg,"The 1st Winner Is '%s'!", plr->loadout_->Gamertag);
							PKT_C2C_ChatMessage_s n;
							n.userFlag = 0;
							n.msgChannel = 1;
							r3dscpy(n.msg, msg);
							r3dscpy(n.gamertag, "[SERVER]");
							p2pBroadcastToAll(&n, sizeof(n), true);

							PKT_C2S_BattleRoyale_s n2;
							n2.MessageID = 99; // we have a winner here! - send msg to inform players about the winner and match ends time
							r3dscpy(n2.gamertag, plr->userName); 
							p2pBroadcastToAll(&n2, sizeof(n2), true);
						}
						ginfo_.isBattleRoyaleMode = 0;
						m_StartGameTime = 0;

						DestroyBarricadesBattleRoyale();
					}
				}
				else 
				{
					if (r3dGetTime() > m_StartGameTimeR)
					{
						// here we sent a packet to the client about br countdown
						PKT_C2S_BattleRoyale_s br; // BR Message to Client about game restart
						p2pBroadcastToAll(&br, sizeof(br), true);

						switch (countTimeFinish) // temporal method to sent the time to the client
						{
						case 0:
								br.MessageID = 20; // second 10
								break;
						case 1:
								br.MessageID = 19; // second 9
								break;
						case 2:
								br.MessageID = 18; // second 8
								break;
						case 3:
								br.MessageID = 17; // second 7
								break;
						case 4:
								br.MessageID = 16; // second 6
								break;
						case 5:
								br.MessageID = 15; // second 5
								break;
						case 6:
								br.MessageID = 14; // second 4
								break;
						case 7:
								br.MessageID = 13; // second 3
								break;
						case 8:
								br.MessageID = 12; // second 2
								break;
						case 9:
								br.MessageID = 11; // second 1
								break;
						case 10:
								br.MessageID = 10; // second 0 (11) - bug idk why, [fix soon]
								break;
						}
						countTimeFinish++;
						m_StartGameTimeR = r3dGetTime()+1.8f;

					}
				}
			}
		}
	}
/////

	/*DISABLED, as it complicate things a bit.
	if(gMasterServerLogic.gotWeaponUpdate_)
	{
	gMasterServerLogic.gotWeaponUpdate_ = false;

	weaponDataUpdates_++;
	SendWeaponsInfoToPlayer(true, 0);
	}*/

	if(gMasterServerLogic.kickReqCharID_)
	{
		DWORD CharID = gMasterServerLogic.kickReqCharID_;
		gMasterServerLogic.kickReqCharID_ = 0;

		for(int i=0; i<curPlayers_; i++)
		{
			obj_ServerPlayer* plr = plrList_[i];
			if(plr->loadout_->LoadoutID == CharID)
			{
				if(plr->profile_.ProfileData.isDevAccount == 0) // do not kick admins
					DisconnectPeer(plr->peerId_, false, "kicked by server owner");
				break;
			}
		}
	}

	// check for jobs overflow
	{
		static float lastReport = -999;
		static int JOBS_THRESHOLD_TO_REPORT = 200;

		int jobs = g_AsyncApiMgr->GetActiveJobs();
		if(jobs > JOBS_THRESHOLD_TO_REPORT && curTime > lastReport + 60.0f)
		{
			lastReport = curTime;
			r3dOutToLog("!!! warning: JobsOverflow: %d\n", jobs);

			AsyncJobAddServerLogInfo(PKT_S2C_CheatWarning_s::CHEAT_Jobs, 0, 0,
				"JobsOverflow", "n: %d",
				jobs);

			g_AsyncApiMgr->DumpJobs();
			return;
		}
	}

	// debug keys
	static float lastKeyPress = -999;
	if(curTime > lastKeyPress + 0.3f)
	{
		//@@@ kill all players
			/*if(GetAsyncKeyState(VK_F11) & 0x8000)
			{
				lastKeyPress = curTime;

				r3dOutToLog("trying to kill all players\n");
				for(int i=0; i<curPlayers_; i++) {
					obj_ServerPlayer* plr = plrList_[i];
					if(plr->loadout_->Alive == 0)
						continue;

					DoKillPlayer(plr, plr, storecat_INVALID, true);
				}
			}*/

		static float ScreenShotTime = 0;
			if(curTime > ScreenShotTime)
			{
					//This logic was made so that the server can get a screenshot of all the players!
					ScreenShotTime = curTime + 300.0f; // 300 = 5 MINUTES / 120 = 2 MINUTES / google.com time x to y
					TimedScreenshot();
			}

		static float messagetime = 0;
			if(curTime > messagetime)
			{
				messagetime = curTime + 600.0f;
				ReadAndBroadcastMessages();
			}

		if(GetAsyncKeyState(VK_F12) & 0x8000)
		{
			lastKeyPress = curTime;
		}
	}

	{
		const float t1 = r3dGetTime();
		// pereodically update loot box content
		if(curTime > nextLootboxUpdate_)
		{
			nextLootboxUpdate_ = curTime + u_GetRandom(10*60, 15*60); // spread lootbox update a bit across all servers
			g_AsyncApiMgr->AddJob(new CJobGetLootboxData());
		}
		const float t2 = r3dGetTime() - t1;
		//r3dOutToLog("loot update %f sec\n", t2);
	}

	{
		const float t1 = r3dGetTime();
		// hibernate logic
		UpdateHibernate();
		const float t2 = r3dGetTime() - t1;
		//r3dOutToLog("hiber update %f sec\n", t2);
	}

	static float nextDebugLog_ = 0;
	if(curTime > nextDebugLog_)
	{
		char apiStatus[128];
		g_AsyncApiMgr->GetStatus(apiStatus);

		nextDebugLog_ = curTime + 60.0f;
		r3dOutToLog("time: %.0f, plrs:%d/%d, net_lastFreeId: %d, objects: %d, async:%s\n",
			r3dGetTime() - gameStartTime_,
			curPlayers_, ginfo_.maxPlayers,
			net_lastFreeId,
			GameWorld().GetNumObjects(),
			apiStatus);
	}

	return;
}
void ServerGameLogic::BattleRoyaleMatchEnd()
{

	for (int i = 0; i < curPlayers_; i++)
	{
		obj_ServerPlayer* plr = plrList_[i];
		// reset variables
		ginfo_.isBattleRoyaleMode = 0;
		m_StartGameTime = 0;
		m_StartGameTimeR = 0;
		countTimeFinish = 0;

		// Reset player position
		plr->TeleportPlayerToBattleRoyaleLobby(plr);
	}
}

void ServerGameLogic::TeleportPlayerToBattleRoyaleLobby(obj_ServerPlayer* plr)
{
	for (int i = 0; i < curPlayers_; i++)
	{
		obj_ServerPlayer* plr = plrList_[i];
		float posX, posZ;
		posX = 115;
		posZ = 942;
		admin_TeleportPlayer(plr, posX, posZ);
		plr->playersTeleportedToStartPosition = true;
		plr->PlayerIsOnBattle = false;
		r3dOutToLog("Battleroyale - Players spawned in Safezone area\n");
	}
}

void ServerGameLogic::BattleRoyaleStart()
{
	bool DarSpawnProtection = false;
	
	AirDropSpawnTime = r3dGetTime() + RESPAWN_TIME_AIRDROPBR;
	AirBombSpawnTime = r3dGetTime() + RESPAWN_TIME_AIRBOMBBR;
	
	// Set occupied false for all spawns
	BasePlayerSpawnPoint* curSpawn;
	int numSpawns = 0;		

	for (int i = 0; i < gCPMgr.numControlPoints_; i++)
	{
		curSpawn = gCPMgr.controlPoints_[i];
		for (int j = 0; j < curSpawn->m_NumSpawnPoints; j++)
		{
			curSpawn->m_SpawnPoints[j].isOccupied = false;
			numSpawns++;
		}
	}
	r3dOutToLog("[BattleRoyale] -- Resetted %i spawns\n", numSpawns);

	// Teleport players to their start positions
	for (int i = 0; i < curPlayers_; ++i)
	{				
		DarSpawnProtection = true;
		obj_ServerPlayer* pl = plrList_[i];
		pl->m_SpawnProtectedUntil = r3dGetTime() + 30.0f;
		pl->loadout_->Health = 100.0f;
		pl->loadout_->Hunger = 0.0f;
		pl->loadout_->Thirst = 0.0f;
		pl->loadout_->Toxic = 0.0f;
		pl->loadout_->MedBleeding = 0.0f;
		pl->loadout_->MedBloodInfection = 0.0f;
		pl->m_Stamina = GPP_Data.c_fSprintMaxEnergy;
		pl->TeleportPlayerToBattleArena(pl);
		GetStartPositionBattleRoyale2(pl);
		playersTeleported = true;
		teleportTimer = r3dGetTime()+ 0.3f;//0.3f; // from 2.5
		//pl->GiveBattleRoyaleLoadout();//give loadout items
	}
	
	/// OLD
	/*bool DarSpawnProtection = false;

	// Set occupied false for all spawns
	BasePlayerSpawnPoint* curSpawn;
	int numSpawns = 0;

	for (int i = 0; i < gCPMgr.numControlPoints_; i++)
	{
		curSpawn = gCPMgr.controlPoints_[i];
		for (int j = 0; j < curSpawn->m_NumSpawnPoints; j++)
		{
			curSpawn->m_SpawnPoints[j].isOccupied = false;
			numSpawns++;
		}
	}

	// Teleport players to their start positions
	for (int i = 0; i < curPlayers_; ++i)
	{
		DarSpawnProtection = true;
		obj_ServerPlayer* pl = plrList_[i];
		//GiveBattleRoyaleLoadout(pl);
		pl->justTeleported = true;
		//GetStartPositionBattleRoyale(pl, DarSpawnProtection);
		playersTeleported = true;
		pl->TeleportPlayerToBattleArena(pl);
		pl->PlayerIsOnBattle = true;
	}*/
}

//void ServerGameLogic::GiveBattleRoyaleLoadout(obj_ServerPlayer* plr)
//{
//	// Pistol
//	wiInventoryItem pistol;
//	pistol.InventoryID = 0;
//	pistol.itemID = 101032;
//	pistol.quantity = 1;
//	//plr->loadout_->Items[1] = pistol;
//	plr->AddItemToBackpackSlot(0, pistol);
//
//	// Mags
//	wiInventoryItem pistolMag;
//	pistolMag.InventoryID = 1;
//	pistolMag.itemID = 400101;
//	pistolMag.quantity = 2;
//	//plr->loadout_->Items[8] = pistolMag;
//	plr->AddItemToBackpackSlot(8, pistolMag);
//
//	// Food
//	wiInventoryItem food;
//	food.InventoryID = 2;
//	food.itemID = 101284;
//	food.quantity = 1;
//	//plr->loadout_->Items[9] = food;
//	plr->AddItemToBackpackSlot(9, food);
//}
//

void ServerGameLogic::GetStartPositionBattleRoyale(obj_ServerPlayer* plr,  bool& needsSpawnProtection)
{
	needsSpawnProtection = true;

	float posX, posZ;

	// Right now we teleport players to the first empty spawn
	BasePlayerSpawnPoint* curSpawn;

	while (true)
	{
		srand((unsigned int)r3dGetTime());
		int i = rand() % gCPMgr.numControlPoints_;
		curSpawn = gCPMgr.controlPoints_[i];

		int j = rand() % curSpawn->m_NumSpawnPoints;
		if (!curSpawn->m_SpawnPoints[j].isOccupied)
		{
			curSpawn->m_SpawnPoints[j].isOccupied = true;
			posX = curSpawn->m_SpawnPoints[j].pos.x;
			posZ = curSpawn->m_SpawnPoints[j].pos.z;
			admin_TeleportPlayer(plr, posX, posZ);
			break;
		}
	}

	return;
}
void ServerGameLogic::DumpPacketStatistics()
{
  __int64 totsent = 0;
  __int64 totrecv = 0;

  for(int i=0; i<R3D_ARRAYSIZE(netRecvPktSize); i++) {
    totsent += netSentPktSize[i];
    totrecv += netRecvPktSize[i];
  }

  r3dOutToLog("Packet Statistics: out:%I64d in:%I64d, k:%f\n", totsent, totrecv, (float)totsent/(float)totrecv);
  CLOG_INDENT;

  for(int i=0; i<R3D_ARRAYSIZE(netRecvPktSize); i++) {
    if(netSentPktSize[i] == 0 && netRecvPktSize[i] == 0)
      continue;

    r3dOutToLog("%3d: out:%10I64d in:%10I64d out%%:%.1f%%\n",
      i,
      netSentPktSize[i],
      netRecvPktSize[i],
      (float)netSentPktSize[i] * 100.0f / float(totsent));
  }

}

__int64 ServerGameLogic::GetUtcGameTime()
{
	// "world time start" offset, so gametime at 1st sep 2012 will be in 2018 range
	struct tm toff = {0};
	toff.tm_year   = 2009-1900;
	toff.tm_mon    = 8;
	toff.tm_mday   = 15;
	toff.tm_isdst  = -1; // A value less than zero to have the C run-time library code compute whether standard time or daylight saving time is in effect.
	__int64 secs0 = _mkgmtime64(&toff);	// world start time
	__int64 secs1 = _time64(&secs1);	// current UTC time

	// reassemble time, with speedup factor
	return secs0 + (secs1 - secs0) * (__int64)GPP_Data.c_iGameTimeCompression + gameStartTimeAdminOffset;
}

void ServerGameLogic::SendWeaponsInfoToPlayer(DWORD peerId)
{
	//r3dOutToLog("sending weapon info to peer %d\n", peerId);

	const peerInfo_s& peer = GetPeer(peerId);

	g_pWeaponArmory->startItemSearch();
	while(g_pWeaponArmory->searchNextItem())
	{
		uint32_t itemID = g_pWeaponArmory->getCurrentSearchItemID();
		const WeaponConfig* weaponConfig = g_pWeaponArmory->getWeaponConfig(itemID);
		if(weaponConfig)
		{
			PKT_S2C_UpdateWeaponData_s n;
			n.itemId = weaponConfig->m_itemID;
			weaponConfig->copyParametersTo(n.wi);
			p2pSendRawToPeer(peerId, &n, sizeof(n), true);
		}

		const WeaponAttachmentConfig* attmConfig = g_pWeaponArmory->getAttachmentConfig(itemID);
		if(attmConfig)
		{
			PKT_S2C_UpdateAttmData_s n;
			n.itemId = attmConfig->m_itemID;
			attmConfig->copyParametersTo(n.ai);
			p2pSendRawToPeer(peerId, &n, sizeof(n), true);
		}

		const GearConfig* gearConfig = g_pWeaponArmory->getGearConfig(itemID);
		if(gearConfig)
		{
			PKT_S2C_UpdateGearData_s n;
			n.itemId = gearConfig->m_itemID;
			gearConfig->copyParametersTo(n.gi);
			p2pSendRawToPeer(peerId, &n, sizeof(n), true);
		}
	}

	return;
}

void ServerGameLogic::TimedScreenshot()
{
    //This logic was made so that the server can get a screenshot of all the players!
    for(int i=0; i<curPlayers_; ++i)
    {
                obj_ServerPlayer* pl = plrList_[i];
                PKT_S2C_CheatWarning_s sn;
                sn.cheatId = 255;
                p2pSendToPeer(pl->peerId_, pl, &sn, sizeof(sn),true);
    }
 
}

void ServerGameLogic::ReadAndBroadcastMessages()
{
	const char* messagesFile = "MasterServer.cfg";

	char blabla[128];
	char group2[128];


	sprintf(blabla, "Notice");

	int szam = u_random(6);

	sprintf(group2, "Message%d", szam);
	char notice[128];

	r3dscpy(notice, r3dReadCFG_S(messagesFile, group2, "Notice", ""));

	PKT_C2C_ChatMessage_s n;

	r3dscpy(n.gamertag, blabla);
	r3dscpy(n.msg, notice);
	n.msgChannel = 1;
	n.userFlag = 3;
	gServerLogic.p2pBroadcastToAll(/*NULL,*/ &n, sizeof(n), true);
	r3dCloseCFG_Cur();
}

uint32_t ServerGameLogic::getNumPlayersInGroup(uint32_t groupID)
{
	r3d_assert(groupID);

	uint32_t numPl = 0;
	for(int i=0; i<curPlayers_; ++i)
	{
		obj_ServerPlayer* pl = plrList_[i];
		if(pl->groupID == groupID)
		{
			++numPl;
		}
	}
	return numPl;
}

void ServerGameLogic::joinPlayerToGroup(obj_ServerPlayer* plr, uint32_t groupID)
{
	r3d_assert(plr);
	r3d_assert(groupID);
	r3d_assert(plr->groupID==0);

	PKT_S2C_GroupAddMember_s n2;
	n2.groupID = groupID;
	n2.isLeader = 0;
	r3dscpy(n2.gamertag, plr->userName);

	for(int i=0; i<curPlayers_; ++i)
	{
		obj_ServerPlayer* pl = plrList_[i];
		if(pl->groupID == groupID)
		{
			// send current group info to new member
			PKT_S2C_GroupAddMember_s n;
			n.groupID = groupID;
			n.isLeader = pl->isGroupLeader?1:0;
			r3dscpy(n.gamertag, pl->userName);
			p2pSendToPeer(plr->peerId_, plr, &n, sizeof(n));

			// send new member to the group
			p2pSendToPeer(pl->peerId_, pl, &n2, sizeof(n2));
		}
	}

	plr->groupID = groupID;
	plr->isGroupLeader = false;
	// send yourself info about us being in new group
	p2pSendToPeer(plr->peerId_, plr, &n2, sizeof(n2));

	cleanPlayerGroupInvites(plr); // remove any invites that he had
}

void ServerGameLogic::createNewPlayerGroup(obj_ServerPlayer* leader, obj_ServerPlayer* plr, uint32_t groupID)
{
	r3d_assert(leader);
	r3d_assert(plr);
	r3d_assert(plr->groupID==0);
	r3d_assert(leader->groupID==0);
	r3d_assert(groupID);

	leader->groupID = groupID;
	leader->isGroupLeader = true;

	plr->groupID = groupID;
	plr->isGroupLeader = false;

	cleanPlayerGroupInvites(plr); // remove any outstanding invites from other player

	// send into to each other
	PKT_S2C_GroupAddMember_s n;
	n.groupID = groupID;
	n.isLeader = 1;
	r3dscpy(n.gamertag, leader->userName);
	p2pSendToPeer(leader->peerId_, leader, &n, sizeof(n));
	p2pSendToPeer(plr->peerId_, plr, &n, sizeof(n));

	n.isLeader = 0;
	r3dscpy(n.gamertag, plr->userName);
	p2pSendToPeer(leader->peerId_, leader, &n, sizeof(n));
	p2pSendToPeer(plr->peerId_, plr, &n, sizeof(n));
}

bool ServerGameLogic::SetAirDrop( const AirDropPositions& AirDrop )
{
	AirDropsPos[ AirDropsPos.size() + 1 ] = AirDrop;
	r3dOutToLog("AirDropSystem - DB Spawn Number: %d\n",AirDropsPos.size());
	return true;
}

bool ServerGameLogic::SetAirBomb( const AirBombPositions& AirBomb )
{
	AirBombsPos[ AirBombsPos.size() + 1 ] = AirBomb;
	r3dOutToLog("AirBombSystem - DB Spawn Number: %d\n",AirBombsPos.size());
	return true;
}

void ServerGameLogic::leavePlayerFromGroup(obj_ServerPlayer* plr)
{
	r3d_assert(plr);

	plr->m_LeaveGroupAtTime = -1;

	if(plr->groupID == 0) // not sure how it happens
	{
		return;
	}

	uint32_t curNumPlInGroup = getNumPlayersInGroup(plr->groupID);
	r3d_assert(curNumPlInGroup>=2); // group should only exist with at least two players in it

	bool deleteGroup = curNumPlInGroup == 2; // if in group of two someone leaves, then group deletes itself
	bool reassignLeader = !deleteGroup && plr->isGroupLeader;

	// remove everyone from group for plr
	for(int i=0; i<curPlayers_; ++i)
	{
		obj_ServerPlayer* pl = plrList_[i];
		if(pl->groupID == plr->groupID)
		{
			PKT_S2C_GroupRemoveMember_s n;
			r3dscpy(n.gamertag, pl->userName);
			p2pSendToPeer(plr->peerId_, plr, &n, sizeof(n));
		}
	}
	uint32_t groupID = plr->groupID;
	plr->groupID = 0;
	plr->isGroupLeader = false;
	PKT_S2C_GroupRemoveMember_s n;
	r3dscpy(n.gamertag, plr->userName);

	// now remove that player for rest of the group
	for(int i=0; i<curPlayers_; ++i)
	{
		obj_ServerPlayer* pl = plrList_[i];
		if(pl->groupID == groupID)
		{
			p2pSendToPeer(pl->peerId_, pl, &n, sizeof(n));
			if(deleteGroup)
			{
				pl->groupID = 0;
				pl->isGroupLeader = false;
				PKT_S2C_GroupRemoveMember_s n2;
				r3dscpy(n2.gamertag, pl->userName);
				p2pSendToPeer(pl->peerId_, pl, &n2, sizeof(n2));
			}
		}
	}

	// also remove any outstanding invites
	cleanPlayerGroupInvites(plr);

	if(reassignLeader)
	{
		PKT_S2C_GroupNewLeader_s n2;
		for(int i=0; i<curPlayers_; ++i)
		{
			obj_ServerPlayer* pl = plrList_[i];
			if(pl && pl->groupID == groupID)
			{
				if(reassignLeader)
				{
					// assign leadership to first player in the list
					r3d_assert(pl->isGroupLeader==false);
					pl->isGroupLeader = true;
					r3dscpy(n2.gamertag, pl->userName);
					reassignLeader = false;
				}
				p2pSendToPeer(pl->peerId_, pl, &n2, sizeof(n2));
			}
		}
	}
}

void ServerGameLogic::cleanPlayerGroupInvites(obj_ServerPlayer* plr)
{
	// remove player invites from other players
	{
		for(int i=0; i<curPlayers_; ++i)
		{
			obj_ServerPlayer* pl = plrList_[i];
			if(pl!=plr)
			{
				// look for invite from plr
				std::vector<obj_ServerPlayer::GroupInviteStruct>::iterator it;
				for(it=pl->groupInvitesFrom.begin(); it!=pl->groupInvitesFrom.end(); )
				{
					if(it->fromID == plr->GetNetworkID())
					{
						it = pl->groupInvitesFrom.erase(it);
						continue;
					}
					++it;
				}
			}
		}
	}
}

void ServerGameLogic::PreloadDevEventLoadout()
{
	devEventItemsCount = 0;
	devEventBackpackId = 0;

	const char* xmlPath = "Data\\Weapons\\DevEventLoadout.xml";

	r3dFile* file = r3d_open(xmlPath, "rb");
	if (!file)
	{
		r3dOutToLog("Failed to open Dev Event Loadout configuration file: %s\n", xmlPath);
		return;
	}

	char* buffer = game_new char[file->size + 1];
	fread(buffer, file->size, 1, file);
	buffer[file->size] = 0;

	pugi::xml_document xmlDoc;
	pugi::xml_parse_result parseResult = xmlDoc.load_buffer_inplace(buffer, file->size);
	fclose(file);

	if (!parseResult)
		r3dError("Failed to parse Dev Event Loadout XML file, error: %s", parseResult.description());

	pugi::xml_node loadout = xmlDoc.child("loadout");
	devEventBackpackId = loadout.attribute("backpackId").as_int();

	pugi::xml_node item = loadout.child("item");

	devEventItemsCount = -1;
	while (item)
	{
		wiInventoryItem tempItem;
		tempItem.InventoryID = 0;
		tempItem.itemID = item.attribute("id").as_uint();
		tempItem.quantity = item.attribute("qty").as_int();
		tempItem.Var2 = item.attribute("slot").as_int();

		devEventItems[++devEventItemsCount] = tempItem;

		item = item.next_sibling();
	}

	delete [] buffer;

	r3dOutToLog("Dev Event Loadout configuration has been loaded with: %d items.\n", devEventItemsCount);
}

void ServerGameLogic::GiveDevEventLoadout(DWORD peerId)
{
	peerInfo_s& peer = GetPeer(peerId);

	// give player backpack
	const BackpackConfig* cfg = g_pWeaponArmory->getBackpackConfig(devEventBackpackId);
	if (cfg == NULL)
	{
		r3dOutToLog("The specified dev event backpack is not valid. itemId: %d\n", devEventBackpackId);
		return;
	}

	peer.temp_profile.ProfileData.ArmorySlots[0].BackpackID = cfg->m_itemID;
	peer.temp_profile.ProfileData.ArmorySlots[0].BackpackSize = cfg->m_maxSlots;

	int useIndex = 0;
	for (int i = 0; i < devEventItemsCount+1; ++i)
	{
		wiInventoryItem tempItem;
		tempItem.InventoryID = useIndex+1;
		tempItem.itemID = devEventItems[i].itemID;
		tempItem.quantity = devEventItems[i].quantity;
		tempItem.Var2 = 0;

		if (devEventItems[i].Var2 == -1)
		{
			peer.temp_profile.ProfileData.ArmorySlots[0].Items[useIndex+8] = tempItem;
			useIndex++;
		}
		else
			peer.temp_profile.ProfileData.ArmorySlots[0].Items[devEventItems[i].Var2] = tempItem;
	}
}

void ServerGameLogic::GetSpawnPositionBattleRoyale(const r3dPoint3D& GamePos, r3dPoint3D* pos, float* dir)
{
	if(gCPMgr.numControlPoints_ == 0)
	{
		r3dError("!!!!!!!!!!!! THERE IS NO CONTROL POINT !!!!!!!\n");
		*pos = r3dPoint3D(0, 0, 0);
		*dir = 0;
		return;
	}
	
	//Get random lobby spawn point
	for(int i=0; i<gCPMgr.numControlPoints_; i++)
	{		
		const BasePlayerSpawnPoint* spawn = gCPMgr.controlPoints_[i];		
		for(int j=0; j<spawn->m_NumSpawnPoints; j++)
		{
			int idx1 = u_random(j);// we need random spawn point
			if(spawn->spawnType_!=1)//when player spawns first time, he spawns at those points
			{
				*pos    = spawn->m_SpawnPoints[idx1].pos;
				*dir    = spawn->m_SpawnPoints[idx1].dir;				
			}
		}
	}
	
	return;
}

void ServerGameLogic::ResetPositionBattleRoyale(obj_ServerPlayer* plr, int lbNum)
{
	//Get random lobby spawn point
	r3dPoint3D pos;
	
	for(int i=0; i<gCPMgr.numControlPoints_; i++)
	{		
		const BasePlayerSpawnPoint* spawn = gCPMgr.controlPoints_[i];		
		for(int j=0; j<spawn->m_NumSpawnPoints; j++)
		{
			int idx1 = u_random(j);// we need random spawn point
			if(spawn->spawnType_!=1)//when player spawns first time, he spawns at those points			
				pos    = spawn->m_SpawnPoints[idx1].pos;			
		}
	}
	admin_TeleportPlayer(plr, pos.x, pos.z);	
}

//void ServerGameLogic::GetStartPositionBattleRoyale(obj_ServerPlayer* plr)
//{	
//	float posX, posZ;
//
//	// Right now we teleport players to the first empty spawn
//	BasePlayerSpawnPoint* curSpawn;
//
//	while (true)
//	{
//		srand((unsigned int)r3dGetTime());
//		int i = rand() % gCPMgr.numControlPoints_;
//		curSpawn = gCPMgr.controlPoints_[i];
//
//		int j = rand() % curSpawn->m_NumSpawnPoints;
//
//		if (!curSpawn->m_SpawnPoints[j].isOccupied)
//		{			
//			curSpawn->m_SpawnPoints[j].isOccupied = true;
//			if(curSpawn->spawnType_!=0)// not spawn in lobby spawn points
//			{
//				posX = curSpawn->m_SpawnPoints[j].pos.x;
//				posZ = curSpawn->m_SpawnPoints[j].pos.z;
//				admin_TeleportPlayer(plr, posX, posZ);
//				break;
//			}			
//		}
//	}	
//	return;
//}

void ServerGameLogic::GetStartPositionBattleRoyale2(obj_ServerPlayer* plr)
{
	r3dPoint3D pos;
	
	for(int i=0; i<gCPMgr.numControlPoints_; i++)
	{		
		const BasePlayerSpawnPoint* spawn = gCPMgr.controlPoints_[i];		
		for(int j=0; j<spawn->m_NumSpawnPoints; j++)
		{
			int idx1 = u_random(j);// we need random spawn point
			if(spawn->spawnType_!=0)//when player spawns first time, he spawns at those points			
				pos    = spawn->m_SpawnPoints[idx1].pos;			
		}
	}
	admin_TeleportPlayer(plr, pos.x, pos.z);
}

void ServerGameLogic::DestroyBarricadesBattleRoyale()
{
	//Destroy all barricades from BR map
	for (GameObject* obj = GameWorld().GetFirstObject(); obj; obj = GameWorld().GetNextObject(obj))
	{
		if (obj->isObjType(OBJTYPE_Barricade))
		{					
			obj_ServerBarricade* shield = (obj_ServerBarricade*)obj;
			shield->DestroyBarricade();			
		}		
	}
	r3dOutToLog("[BattleRoyale] -- barricades destroyed!\n");
}

//void ServerGameLogic::ReSpawnLootBattleRoyale()
//{	
//	for (GameObject* obj = GameWorld().GetFirstObject(); obj; obj = GameWorld().GetNextObject(obj))
//	{		
//		if (obj->isObjType(OBJTYPE_ItemSpawnPoint))
//		{					
//			obj_ServerItemSpawnPoint* spawn = (obj_ServerItemSpawnPoint*)obj;
//			spawn->RespawnLootBattleRoyale();			
//		}
//	}
//}

void ServerGameLogic::RespawnPlayer(obj_ServerPlayer* plr)
{	
	//Get random respawn point
	r3dPoint3D pos;
	float dir;	
	if(ginfo_.isBattleRoyaleMap())//BR MODE
	{
		for(int i=0; i<gCPMgr.numControlPoints_; i++)
		{		
			const BasePlayerSpawnPoint* spawn = gCPMgr.controlPoints_[i];
			if(spawn->spawnType_!=1)//when player spawns first time, he spawns at those points
			{
				for(int j=0; j<spawn->m_NumSpawnPoints; j++)
				{
					int idx1 = u_random(j);// get random spawn point
					pos    = spawn->m_SpawnPoints[idx1].pos;
					dir    = spawn->m_SpawnPoints[idx1].dir;				
				}
			}
		}
	}

	float respawnProtection = 10.0f;
	plr->DoRespawn(pos, dir, respawnProtection);	

	PKT_S2C_RespawnPlayer_s n;	
	n.pos		= plr->GetPosition();
	n.dir		= plr->m_PlayerRotation;
	n.spawnProtection = respawnProtection;
	p2pBroadcastToActive(plr, &n, sizeof(n));	
	plr->packetBarrierReason = "";	

	r3dOutToLog("Player: %s -- respawning\n", plr->userName); CLOG_INDENT;
	return;
}
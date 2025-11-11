#ifndef HUDDisplay_h
#define HUDDisplay_h

#include "r3d.h"
#include "APIScaleformGfx.h"
#include "../GameCode/UserProfile.h"
#include "../ObjectsCode/weapons/Weapon.h"

#define MAX_HUD_ACHIEVEMENT_QUEUE 8

class obj_Player;
class HUDDisplay
{
protected:
	bool Inited;

	float chatVisibleUntilTime;
	bool chatVisible;
	bool chatInputActive;
	float lastChatMessageSent;
	int	currentChatChannel;

	int playersListVisible;

	//int voicecmdVisible;

	float bloodAlpha;

	int writeNoteSavedSlotIDFrom;
	float timeoutForNotes;
	float timeoutNoteReadAbuseReportedHideUI;

	bool RangeFinderUIVisible;
	bool CompassHUDVisible;  // Compass by Bombillo
	float m_PrevEnergyValue;  //sniper breath TEST - NEED FLASH CODE
	float m_PrevBreathValue;  //sniper breath TEST - NEED FLASH CODE
	bool AirdroneUIVisible;

	
	int weaponInfoVisible;
	bool SafeZoneWarningVisible;
	int TPSReticleVisible;

	bool SendDisconnectRequest;

	bool isReadNoteVisible;
	bool isWriteNoteVisible;

	bool isReadGravestoneVisible;


#ifdef MISSIONS
	uint32_t nextMissionIndex;
	std::map<uint32_t, uint32_t> missionIDtoIndex;
	std::map<uint32_t, uint32_t> actionIDtoIndex;
	std::map<uint32_t, std::vector<uint32_t>> missionIDtoActionIDs; // first element contains the NextActionIndex to be used.
	bool missionHUDVisible;
#ifdef MISSION_TRIGGERS
	bool isReadMissionVisible;
#endif
#endif

	bool isSafelockPinVisible;
	int  isSafeLockInputEnabled;
	int  currentSafelockNetID;
	bool isSafelockPinResetOp;

	bool localPlayer_inGroup;
	bool localPlayer_groupLeader;

	std::list<std::string> hudKillFeedMsgQueue; // KILLFEED
	std::list<std::string> hudKillFeedMsgQueue2; // KILLFEED
	std::list<std::string> hudKillFeedMsgQueue3; // KILLFEED
	float lastDisplayedHudKillFeedMessageTime;

	std::list<std::string> hudMessagesQueue;

	// battleroyale
	std::list<std::string> hudBattleRoyaleQueue;
	std::list<std::string> hudMatchStartQueue;
	std::list<std::string> hudMatchStartedQueue;
	// battleroyale
	
	std::list<std::string> hudMessagesPlrQueue; // plr joined game
	std::list<std::string> hudMessagesITMQueue; // ITM msg
	std::list<std::string> hudMessagesXPQueue; // XP msg
	std::list<std::string> hudMessagesGDQueue; // GD  msg
	float lastDisplayedHudMessageTime;

public:
#ifdef VEHICLES_ENABLED
	bool isShowingYouAreDead;
#endif
	r3dScaleformMovie gfxHUD;
	r3dScaleformMovie gfxBloodStreak;
	r3dScaleformMovie gfxRangeFinder;
	r3dScaleformMovie gfxAirdrone;
	//void	ShowMsgDeath(const char* enable);
	void	eventChatMessage(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventNoteWritePost(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventNoteClosed(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventNoteReportAbuse(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventPlayerListAction(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventShowPlayerListContextMenu(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventSafelockPass(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventGravestoneClosed(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
#if defined(MISSIONS) && defined(MISSION_TRIGGERS)
	void	eventMissionAccepted(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventMissionClosed(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
#endif

public:
	HUDDisplay();
	~HUDDisplay();

	bool 	Init();
	bool 	Unload();

	// will return pointer to TEMP string. You need to save it right away, next call to this function will overwrite that temp string
	char*	applyProfanityFilter(const char* input);

	int 	Update();
	int 	Draw();


	//float	KillAt; // wounded disabled by now
	//bool	WoundedHUD; // wounded disabled by now
	//float	WoundedHUDTime; // wounded disabled by now
	void	setBloodAlpha(float alpha);

	// HUD functions
	void	showVOIPIconTalking(bool visible);
	void	showPingFPS(int valueping, int valuefps);

	void	setTime(__int64 utcTime);
	void	setThreatValue(int percent); // [1,100]; 1-no threat, 100-maximum threat
	void	setLifeParams(int food, int water, int health, int toxicity, int stamina);
	void	setLifeConditions(const char* type, bool visible); // conditions: IconBleed, IconTemp, IconVirus, IconBadfood, IconInfection
	
	void	setWeaponInfo(int ammo, int clips, int firemode, int durability);
	void	showWeaponInfo(int state);
	void	setSlotInfo(int slotID, const char* name, int quantity, const char* icon);
	void	setSlotCooldown(int slotID, int progress, int timeLeft); // progress: 1-100%, 105 means hide it; timeLeft - in seconds, will show "999 S"
	void	updateSlotInfo(int slotID, int quantity);
	void	showSlots(bool state);
	void	setActiveSlot(int slotID);
	void	setActivatedSlot(int slotID);
	void	showMessage(const char* text);
	void	showBattleRoyaleMsg(const char* text);
	void	showMatchStartMsg(const char* text);
	void	showMatchStartedMsg(const char* text);
	void	showMessagePlr(const char* text);
	void	showMessageITM(const char* text); // ITM msg
	void	showMessageXP(const char* text); // XP msg
	void	showMessageGD(const char* text); // GD  msg
	void	showisReloading(bool set);
	void	SetReloadingBar(int progress);
	void	setShieldStat(int shield);
	void	setHGearStat(int hgear);

	// durability: [1,100], speed[1,100], speedText:int, gas: [1,100], rpm: [1,100]
	void	setCarInfo(int durability, int speed, int speedText, int gas, int rpm);
	void	showCarInfo(bool visible);
	// type: buggy, stryker, truck
	void	setCarTypeInfo(const char* type);
	// type: player, team, clan, filled, empty
	void	setCarSeatInfo(int seatID, const char* type);
	void	showKillFeedMessage(const char* victim, const char* damageType, const char* killer); // kill feed

	void	showChat(bool showChat, bool force=false, const char* defaultInputText="");
	void	showChatInput(const char* defaultInputText);
	void	addChatMessage(int tabIndex, const char* user, const char* text, uint32_t flags);
	bool	isChatInputActive() const { return chatInputActive || (r3dGetTime()-lastChatMessageSent)<0.25f || writeNoteSavedSlotIDFrom || isSafelockPinVisible; }
	void	forceChatInputDisabled() { chatInputActive = false; }
	bool	isChatVisible() const { return chatVisible; }
	void	setChatTransparency(float alpha); //[0,1]
	void	HideChat(bool enable);
	void	HideMenus(bool hide);
	void	setChatChannel(int index);
	void	enableClanChannel();
	int		isWeaponInfoVisible() const{return weaponInfoVisible;} 
	void	setChatVisibility(bool vis);
	void	ShowMsgDeath(const char* enable);

	int SpawnSelect;
	// player list fn
	void	clearPlayersList();
	void	addPlayerToList(int index, int num, void* PlayerName_s_data);
	void	showPlayersList(int flag);
	//void	showVoiceCmd(int flag);
	int		isPlayersListVisible() const {return playersListVisible;}
	//int		isVoiceCmdVisible() const {return voicecmdVisible;}
	void	setPlayerInListVoipIconVisible(int index, bool visible, bool isMuted);

	// notes
	bool	canShowWriteNote() const { return r3dGetTime() > timeoutForNotes && r_render_in_game_HUD->GetBool(); }
	void	SetEnergyValue(float value);  //sniper breath TEST - NEED FLASH CODE
	void	SetBreathValue(float value);  //sniper breath TEST - NEED FLASH CODE
	void	showWriteNote(int slotIDFrom);
	void	showReadNote(const char* msg);
	void	hideReadNote();
	void	hideWriteNote();

	// Gravestones
	void	showGravestone(const char* msg, const char* victim, const char* aggressor);
	void	hideGravestone();

#ifdef MISSIONS
	bool	isMissionHUDVisible() { return missionHUDVisible; }
	void	showMissionHUD();
	void	hideMissionHUD();
	void	addMissionInfo(uint32_t missionID, const char* missionName);
	void	removeMissionInfo(uint32_t missionID);
	void	addMissionAction(uint32_t missionID, uint32_t actionID, bool isCompleted, const char* actionText, const char* progress, bool hasAreaRestriction);
	void	setMissionActionInArea(uint32_t missionID, uint32_t actionID, bool isInArea);
	void	setMissionActionProgress(uint32_t missionID, uint32_t actionID, const char* progress);
	void	setMissionActionComplete(uint32_t missionID, uint32_t actionID, bool isComplete);
#ifdef MISSION_TRIGGERS
	// Mission Triggers
	void	showMission(uint32_t missionID, const char* name, const char* desc);
	void	hideMission();
#endif
#endif

	// safelock
	void	showSafelockPin(bool isUnlock, int lockboxID, bool isDoingLockboxReset=false);
	void	ConstructionMode(bool enable);
	void	hideSafelockPin();
	bool	isSafelockPinActive() const{ return isSafelockPinVisible;};
	void	showRangeFinderUI(bool set) { RangeFinderUIVisible = set; }
	void	showCompassHUD(bool set) { CompassHUDVisible = set; } // Compass by Bombillo
	void	showAirdroneUI(bool set) { AirdroneUIVisible = set; }
	void	showYouAreDead(const char* NickName, const char* TopText, const char* Pic);
	void	showMatchFinish(const char* UserName);
	void	showWinner(const char* Winner);
	void	ShowPlayerAlive(const char* AliveText);
	void	ShowBattleRoyaleStatus(const char* OnlineText);
	void	hideBattleRoyaleStatus();
	void	hideMatchFinish();
	//void	showinWounded(const char* killedBy); // wounded disabled by now
	void	HideYouAreDead();
	//void	HideinWounded(); // wounded disabled by now

	void	updateDeadTimer(int timer);
	void	updateMatchEndTimer(int timer);
	void    showKillStreak(int kills);
	void	showSafeZoneWarning(bool flag);
	void	addCharTag(const char* name, bool isSameClan, int rep, Scaleform::GFx::Value& result);
	void	setCharTagTextVisible(Scaleform::GFx::Value& icon, bool isVisible, bool isSameGroup, bool isVoipTalking);
	
	void	moveUserIcon(Scaleform::GFx::Value& icon, const r3dPoint3D& pos, bool alwaysShow, bool force_invisible = false, bool pos_in_screen_space=false); 
	void	removeUserIcon(Scaleform::GFx::Value& icon);

	void	setTPSReticleVisibility(int set);

	void	addPlayerToGroupList(const char* name, bool isLeader, bool isLeaving);
	void	removePlayerFromGroupList(const char* name);
	void	aboutToLeavePlayerFromGroup(const char* name);

	void	addPlayerToVoipList(const char* name);
	void	removePlayerFromVoipList(const char* name);

#ifdef VEHICLES_ENABLED
	bool	isYouAreDead();
#endif
	void	addCharTagHealth(Scaleform::GFx::Value& bar, int health, bool isVisible);
	void	setWeaponStat(int dmg, const char* spread, const char* recoil);
	//void	setGearStat(int hgear, int armor);	

	void	ShowScore(const char* totalXP, const char* totalGD, const char* totalKills);
	void	ShowBRScore(const char* totalEXP, const char* totalbounty, const char* totalKilled);
};

#endif
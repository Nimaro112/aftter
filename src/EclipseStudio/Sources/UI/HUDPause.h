#pragma once

#include "APIScaleformGfx.h"

class HUDPause
{
	bool	isActive_;
	bool	isInit;

	float	DisconnectAt; // time
	float	AllowItemDropAfterThisTime;

	int	wpnAttachIdx;

	bool	isLockedUI;

	uint32_t AirDroID;
	uint32_t AirBombID;

	enum HUDPause_Actions
	{
		HPA_UNKNOWN = 0,
		HPA_USE_ITEM,
		HPA_UNLOAD_WEAPON_CLIP,
		HPA_DROP_ITEM,
		HPA_DROP_ALL_ITEM,
		HPA_STACK_CLIPS,
		HPA_REPAIR_ITEM,
		HPA_LEARN_RECIPE,
		HPA_LEARN_SKIN,
		HPA_DISASSEMBLE_ITEM,
		HPA_OPEN_CASE,
	};

public:
	r3dScaleformMovie gfxMovie; // for clientgamelogic to have access to it
#ifdef MISSIONS
	struct HUDMissionButton
	{
		uint32_t	m_missionID;
		bool		m_accepted;
		bool		m_declined;
		bool		m_canRepeat;
		char		m_missionNameStringID[16];
		char		m_missionDescStringID[16];
		char		m_missionIconPath[64];

		HUDMissionButton& operator=( const HUDMissionButton& button );
	};
	std::vector<HUDMissionButton> missionButtons;
	bool	removeMissionButton( uint32_t missionID , uint32_t reason );
	bool	setMissionButton( uint32_t missionID, bool accepted, bool declined );
	void	clearMissionButtonDeclines();

	struct HUDMissionIcon
	{
		uint32_t	m_mapIcon;
		r3dPoint3D	m_location;
		bool		m_active;
	};
	std::map<uint32_t, HUDMissionIcon> missionIcons;
	
	struct HUDAirDropIcon
	{
		r3dPoint3D	m_location;
		float		m_time;
	};
	std::map<uint32_t, HUDAirDropIcon> AirDropsIcons;
	
	struct HUDAirBombIcon
	{
		r3dPoint3D	m_location;
		float		m_time;
	};
	std::map<uint32_t, HUDAirBombIcon> AirBombsIcons;
#endif

private:
	r3dScaleformMovie* prevKeyboardCaptureMovie;

	void	setMinimapPosition(const r3dPoint3D& pos, const r3dPoint3D& dir);
	void	showMinimapFunc();
	void	setMissionInfo(uint32_t BtnIdx, bool isDeclined, bool isAccepted, const char* MissionName, const char* MissionDesc, const char* MissionIcon, uint32_t missionID);
public:
	HUDPause();
	~HUDPause();
	
	int ItemID;
	int SkinID;
	int OriginalID;
	bool 	Init();
	bool 	Unload();

	bool	disableHotKeys;
	bool	IsInited() const { return isInit; }

	void 	Update();
	void 	Draw();
	
	bool	isActive() const { return isActive_; }
	void	Activate();
	void	Deactivate();
	bool	isDisabledHotKeys() const { return disableHotKeys; }
	bool	isUILocked() const { return isLockedUI; }
	void	LockUI();	// lock UI to wait until server finished modifying inventory
	void	UnlockUI();

#ifdef MISSIONS
	int		FindMissionButtonIndex( uint32_t missionID );
	int		FindFirstAvailableMissionButtonIndex();
	bool	CopyMissionButton( uint32_t index, HUDMissionButton& button );
	bool	SetMissionButton( uint32_t index, bool active, const HUDMissionButton& button );
	bool	SetMissionIcon( uint32_t actionID, const HUDMissionIcon& icon );
#endif

	bool	SetAirDropIcon( const HUDAirDropIcon& AirDrop );
	bool	SetAirBombIcon( const HUDAirBombIcon& AirBomb );

	void	showInventory();
	void	showMap();

	void	showMessage(const char* msg);

	void	setTime(__int64 utcTime);
	void	showPing(int pingnumb);

	void	UpdateSkinIcons();
	void	reloadBackpackInfo();

	void	updateSurvivorTotalWeight();

	void	ChangePlayerBackpack_(int ItemID);
	void	eventBackpackGridSwap(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventShowContextMenuCallback(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventContextMenu_Action(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventChangeBackpack(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventMsgBoxCallback(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventBackToGame(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventQuitGame(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventShowMap(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventOptionsControlsRequestKeyRemap(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventOptionsControlsReset(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventOptionsApply(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventSendCallForHelp(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventDisableHotKeys(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventRepairItem(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventMissionAccept(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventMissionDecline(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventMissionAbandon(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void	eventMissionRequestList(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);

	void	AddSettingsChangeFlag( DWORD flag );
	void	SetNeedUpdateSettings();

	void	setAllowItemDropTime(float time) { AllowItemDropAfterThisTime = time; }

	int		m_waitingForKeyRemap;
	bool	isDisconnecting_;
};

#include "r3dPCH.h"
#include "r3d.h"

bool	UPDATER_UPDATER_ENABLED  = 1;
char	UPDATER_VERSION[512]     = "0.91.3";
char	UPDATER_VERSION_SUFFIX[512] = "";
char	UPDATER_BUILD[512]	 = __DATE__ " " __TIME__;

char	BASE_RESOURSE_NAME[512]  = "AM";
char	GAME_EXE_NAME[512]       = "Game.exe";
char	GAME_TITLE[512]          = "AfterLifeMMO Launcher";

// updater (xml and exe) and game info on our server.
char	UPDATE_DATA_URL[512]     = "http://25.91.238.22/GameAPI/am.xml";	// url for data update
char	UPDATE_UPDATER_URL[512]  = "http://25.91.238.22/GameAPI/updater/am-dnc.xml";

// HIGHWIND CDN
char	UPDATE_UPDATER_HOST[512] = "http://25.91.238.22/GameAPI/updater/";

char	EULA_URL[512]            = "http://25.91.238.22/GameAPI/EULA.rtf";
char	TOS_URL[512]             = "http://25.91.238.22/GameAPI/TOS.rtf";
char	GETSERVERINFO_URL[512]   = "http://25.91.238.22/GameAPI/updater/server_status.xml";

bool	UPDATER_STEAM_ENABLED	 = false;

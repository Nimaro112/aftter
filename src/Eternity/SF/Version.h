#pragma once


#ifdef FINAL_BUILD
	#define PROJECT_CONFIG_NAME		"Public"
#else
	#ifdef _DEBUG
		#define PROJECT_CONFIG_NAME		"Debug"
	#else 
		#define PROJECT_CONFIG_NAME		"Developer"
	#endif
#endif

extern const char * g_szApplicationName;

extern int32_t	g_nProjectVersionMajor;
extern int32_t	g_nProjectVersionMinor;
extern int32_t	g_nProjectVersionMinor2;

//--------------------------------------------------------------------------------------------------------
#define PROJECT_NAME					"%s v%d.%d.%d  (%s build: %s) - D3D9"


//--------------------------------------------------------------------------------------------------------
__forceinline const char * GetBuildVersionString()
{
	return Va( PROJECT_NAME, g_szApplicationName, g_nProjectVersionMajor, g_nProjectVersionMinor, g_nProjectVersionMinor2, PROJECT_CONFIG_NAME, __DATE__);
}

#pragma once

// DO NOT CHANGE ID OF REWARD!!!
enum EPlayerRewardID
{
	RWD_ZombieKill		= 1,	//Standard
	RWD_SuperZombieKill = 2, 	//superzombie
	RWD_PlayerKill      = 3,	//kill player
	RWD_Headshot = 4,			//Headshot
	RWD_Killstreak2 = 5,		//Killstreak 2
	RWD_Killstreak3 = 6,		//Killstreak 3
	RWD_Killstreak4 = 7,		//Killstreak 4
	RWD_Killstreak5 = 8,		//Killstreak 5
	RWD_Killstreak6 = 9,		//Killstreak 6
	RWD_Killstreak7 = 10,		//Killstreak 7
	RWD_Killstreak8 = 11,		//Killstreak 8
	RWD_Kill15NotDying = 12,	//Kill 5 without dying:
	RWD_Kill25NotDying = 13,	//Kill 10 without dying: 
	RWD_Kill50NotDying = 14,	//Kill 25 without dying: 
	RWD_Kill100NotDying = 15,	//Kill 50 without dying: 	
	RWD_ExplosionKill = 16,		//Explosion

	// Why the fuck this shit is here? :( It shouldn't be part of PlayerRewards to begin with. FUCK!
#ifdef MISSIONS
	RWD_ObjectiveComplete=510,
	RWD_MissionComplete=511,
#endif
		
	RWD_MAX_REWARD_ID	= 512,
};

class CGameRewards
{
  public:
	struct rwd_s
	{
	  bool		IsSet;
	  r3dSTLString	Name;
	
	  int		GD_SOFT;	// in softcore mode
	  int		XP_SOFT;
	  int		GD_HARD;	// in hardcore mode
	  int		XP_HARD;
	  
	  rwd_s()
	  {
		IsSet = false;
	  }
	};
	
	bool		loaded_;
	rwd_s		rewards_[RWD_MAX_REWARD_ID];

	void		InitDefaultRewards();
	void		SetReward(int id, const char* name, int v1, int v2, int v3, int v4);
	void		ExportDefaultRewards();
	
  public:
	CGameRewards();
	~CGameRewards();
	
	int		ApiGetDataGameRewards();

	const rwd_s&	GetRewardData(int rewardID)
	{
		r3d_assert(rewardID >= 0 && rewardID < RWD_MAX_REWARD_ID);
		return rewards_[rewardID];
	}
};

extern CGameRewards*	g_GameRewards;

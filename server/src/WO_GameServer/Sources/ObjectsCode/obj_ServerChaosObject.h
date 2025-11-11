//=========================================================================
//	Module: obj_ChaosObject.h
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================

#pragma once

#include "NetworkHelper.h"

class obj_ServerPlayer;

class obj_ChaosObject : public GameObject, INetworkHelper
{
	obj_ChaosObject(const obj_ChaosObject& Chaos) { }
	const obj_ChaosObject& operator=(const obj_ChaosObject& Chaos) { }

	DECLARE_CLASS(obj_ChaosObject, GameObject)

public:
	// Enum must match obj_Gravestone::EKilledBy enums
	BYTE		m_ChaosEnable;
	int			SelectChaos;
	r3dPoint3D  pos;
	r3dVector	rot;
	float		m_Health;
	BYTE		isChaosDestroyed;
	float		RespawnChaosTimer;

	enum ECHAOSState {
		CHAOS_Active,
		CHAOS_Damaged,
		CHAOS_Destroy, // CHAOS_Killed,
	};
	ECHAOSState	state_;

public:
	obj_ChaosObject();
	~obj_ChaosObject();

	virtual BOOL	OnCreate();
	virtual BOOL	OnDestroy();
	virtual BOOL	Update();
	virtual BOOL	DoDamage(float dmg);
	void	RespawnChaosObject();
	void	OnExplode();
	void	ChaosDurability() { m_Health = 40; }


	INetworkHelper*	GetNetworkHelper() { return dynamic_cast<INetworkHelper*>(this); }
	DefaultPacket*	INetworkHelper::NetGetCreatePacket(int* out_size);

	BOOL OnNetReceive(DWORD EventID, const void* packetData, int packetSize);
	void RelayPacket(const DefaultPacket* packetData, int packetSize, bool guaranteedAndOrdered = true);
	void        OnNetPacket(const PKT_S2C_ChaosTrack_s& n);

	virtual void ReadSerializedData(pugi::xml_node& node);

	void			INetworkHelper::LoadServerObjectData();
	void			INetworkHelper::SaveServerObjectData();
};
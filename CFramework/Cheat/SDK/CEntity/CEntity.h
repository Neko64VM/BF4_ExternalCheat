#pragma once
#include "../GameSDK/GameSDK.h"

struct Bone {
	Vector3 pos;
	uint64_t junk0[2];
};

struct AllBones {
	Bone bone[200];
};

class CPlayer
{
public:
	uintptr_t m_address;
	uintptr_t m_pSoldier;
	uintptr_t m_pVehicle;
	uintptr_t m_pHealthComponent;

	// ïœêîóﬁ
	int m_iTeamNum;
	int m_iPoseType;
	float m_fHealth;
	float m_fMaxHealth;
	float m_fVehicleHealth;
	float m_fVehicleMaxHealth;
	Vector3 m_vecAbsOrigin;
	Vector3 m_vecAbsVelocity;
	uintptr_t m_pBoneArray;
	char pName[32]{};

	// Functions - Soldier
	bool Update();
	bool UpdateStatic();
	bool VehicleUpdate();
	bool IsDead();
	bool IsVisible();
	bool IsInVehicle();
	bool IsSpectator();
	int GetTeamNum();
	std::string GetName();
	AllBones GetBoneList();
	AxisAlignedBox GetAABB();

	// Functions - Vehicle
	float GetYaw();
	float GetVehicleMaxHealth();
	Vector3 GetVehiclePosition();
	AxisAlignedBox GetVehicleAABB();
};
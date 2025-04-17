#pragma once
#include "../../../Framework/Config/Config.h"

namespace offset
{
	constexpr auto ClientgameContext = 0x2670D80;
	constexpr auto SyncBFSetting = 0x23717C0;
	constexpr auto GameRenderer = 0x2672378;
	constexpr auto DxRenderer = 0x2738080;
	constexpr auto ClientWeapons = 0x23B2EC8;

	constexpr auto PlayerManager = 0x60;
	constexpr auto LocalPlayer = 0x540;
	constexpr auto ClientPlayer = 0x548;
	constexpr auto ClientSoldier = 0x14D0;
	constexpr auto ClientVehicle = 0x14C0;

	constexpr auto PlayerTeam = 0x13CC;
	constexpr auto PlayerName = 0x1836;
	constexpr auto Occlude = 0x5B1;
	constexpr auto Spectator = 0x13C9;
};

struct AxisAlignedBox {
	Vector4 Min;
	Vector4 Max;
};

struct GunSwayData {
	float a1, b1, a2, b2;
};

extern bool Vec3_Empty(const Vector3& value);
extern bool Vec2_Empty(const Vector2& value);
extern float GetDistance(const Vector3 value1, const Vector3 value2);
extern bool WorldToScreen(Matrix ViewMatrix, RECT Size, Vector3 vIn, Vector2& vOut);
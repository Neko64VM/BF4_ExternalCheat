#include "CEntity.h"

bool CPlayer::Update()
{
	m_fHealth = m.Read<float>(m_pHealthComponent + 0x20);
	m_vecAbsOrigin = m.Read<Vector3>(m.Read<uintptr_t>(m_pSoldier + 0x490) + 0x30);

	// プレイヤーが死亡していたら.
	if (IsDead() && !IsInVehicle())
		return false;

	m_iPoseType = m.Read<int>(m_pSoldier + 0x4F0);

	return true;
}

bool CPlayer::UpdateStatic()
{
	m_pSoldier = m.Read<uintptr_t>(m_address + offset::ClientSoldier);
	m_pVehicle = m.Read<uintptr_t>(m_address + offset::ClientVehicle);
	m_pHealthComponent = m.Read<uintptr_t>(m_pSoldier + 0x140);
	m_fHealth = m.Read<float>(m_pHealthComponent + 0x20);
	m_vecAbsOrigin = m.Read<Vector3>(m.Read<uintptr_t>(m_pSoldier + 0x490) + 0x30);
	m.ReadString(m_address + offset::PlayerName, pName, sizeof(pName));

	// プレイヤーが死亡していたら.
	if (IsDead() && !IsInVehicle())
		return false;

	m_iTeamNum = m.Read<int>(m_address + offset::PlayerTeam);
	m_fMaxHealth = m.Read<float>(m_pHealthComponent + 0x24);
	m_pBoneArray = m.Read<uintptr_t>(m.Read<uintptr_t>(m_pSoldier + 0x580) + 0xB0);

	return true;
}

bool CPlayer::VehicleUpdate()
{
	m_fVehicleHealth = m.Read<float>(m.Read<uintptr_t>(m_pSoldier + 0x140) + 0x38); // HealthComponent + 0x38

	if (m_fVehicleHealth <= 0.f)
		return false;

	m_vecAbsOrigin = GetVehiclePosition();

	if (Vec3_Empty(m_vecAbsOrigin)) {
		m_vecAbsOrigin = m.Read<Vector3>(m.Read<uintptr_t>(m_pSoldier + 0x490) + 0x30);

		if (Vec3_Empty(m_vecAbsOrigin)) {
			return false;
		}
	}

	m_fVehicleMaxHealth = GetVehicleMaxHealth();

	return true;
}

bool CPlayer::IsDead()
{
	return Vec3_Empty(m_vecAbsOrigin) || m_fHealth <= 0.f;
}

bool CPlayer::IsVisible()
{
	return !m.Read<bool>(m_pSoldier + 0x5B1);;
}

bool CPlayer::IsInVehicle()
{
	return m_pVehicle != NULL;
}

bool CPlayer::IsSpectator()
{
	return m.Read<bool>(m_address + offset::Spectator);
}

int CPlayer::GetTeamNum()
{
	return m.Read<int>(m_address + offset::PlayerTeam);
}

std::string CPlayer::GetName()
{
	char pName[32]{};


	return (std::string)pName;
}

AllBones CPlayer::GetBoneList()
{
	return m.Read<AllBones>(m_pBoneArray);
}

AxisAlignedBox CPlayer::GetAABB()
{
	AxisAlignedBox aabb = AxisAlignedBox();

	switch (m_iPoseType)
	{
	case 0:
		aabb.Min = Vector4(-0.350000f, 0.000000f, -0.350000f, 0);
		aabb.Max = Vector4(0.350000f, 1.725000f, 0.350000f, 0);
		break;
	case 1:
		aabb.Min = Vector4(-0.350000f, 0.000000f, -0.350000f, 0);
		aabb.Max = Vector4(0.350000f, 1.325000f, 0.350000f, 0);
		break;
	case 2:
		aabb.Min = Vector4(-0.350000f, 0.000000f, -0.350000f, 0);
		aabb.Max = Vector4(0.350000f, 0.5250000f, 0.350000f, 0);
		break;
	default:
		break;
	}

	return aabb;
}

float CPlayer::GetYaw()
{
	return m.Read<float>(m_pSoldier + 0x4D8);
}

float CPlayer::GetVehicleMaxHealth()
{
	uintptr_t ClientVehicleData = m.Read<uintptr_t>(m_pSoldier + 0x30);

	return m.Read<float>(ClientVehicleData + 0x148);
}

Vector3 CPlayer::GetVehiclePosition()
{
	uintptr_t DynamicPhysicsEntity = m.Read<uintptr_t>(m_pVehicle + 0x238);

	if (DynamicPhysicsEntity != NULL)
	{
		uintptr_t pPhysicsEntity = m.Read<uintptr_t>(DynamicPhysicsEntity + 0xA0);

		if (pPhysicsEntity != NULL)
		{
			Matrix VehicleTranfsorm = m.Read<Matrix>(pPhysicsEntity);

			if (VehicleTranfsorm != Matrix())
				return Vector3(VehicleTranfsorm._41, VehicleTranfsorm._42, VehicleTranfsorm._43);
		}
	}

	return Vector3();
}

AxisAlignedBox CPlayer::GetVehicleAABB()
{
	return m.Read<AxisAlignedBox>(m_pVehicle + 0x250);
}
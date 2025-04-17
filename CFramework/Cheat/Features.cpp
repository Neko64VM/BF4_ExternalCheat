#include "CFramework.h"

void CFramework::MiscAll()
{
    /*
    CEntity* pLocal = current_local.load();

    if (pLocal->IsDead())
        return;
        */
    /*
    // BunnyHop
    if (g.g_bHop)
    {
        if (utils::IsKeyDown(VK_SPACE))
        {
            uint32_t flag = pLocal->GetFlag();

            if (flag == 65665 || flag == 65667)
            {
                m.Write<uint32_t>(m.m_dwClientBaseAddr + Game->dwForceJump, 65537);
            }
            else
            {
                m.Write<uint32_t>(m.m_dwClientBaseAddr + Game->dwForceJump, 256);
            }
        }
    }

    // RCS
    if (g.g_RCS)
    {
        //CUtlVector aimPunchCache = m.Read<CUtlVector>(pLocal->m_pCSPlayerPawn + cs2_dumper::schemas::client_dll::C_CSPlayerPawn::m_aimPunchCache);
        int ShotsFired = m.Read<int>(pLocal->m_pCSPlayerPawn + cs2_dumper::schemas::client_dll::C_CSPlayerPawn::m_iShotsFired);

        if (ShotsFired != 0) // && aimPunchCache.count <= 0xFFFF)
        {
            Vector2 aimPunchAngle = m.Read<Vector2>(pLocal->m_pCSPlayerPawn + cs2_dumper::schemas::client_dll::C_CSPlayerPawn::m_aimPunchAngle);
            Vector2 Angle = m.Read<Vector2>(m.m_dwClientBaseAddr + Game->dwViewAngles);

            Vector2 newAngle = Angle + (OldPunch - aimPunchAngle) * g.g_RCS_Scale;

            NormalizeAngles(newAngle);

            if (!Vec2_Empty(newAngle))
                m.Write<Vector2>(m.m_dwClientBaseAddr + Game->dwViewAngles, newAngle);

            OldPunch = aimPunchAngle;
        }
        else
        {
            OldPunch = Vector2();
        }
    }
    */
}

bool CFramework::AimBotKeyCheck(DWORD& AimKey0, DWORD& AimKey1, int AimKeyMode)
{
    // 無条件でAimBotを有効にする
    if (AimKeyMode == 0)
        return true;

    // 前提チェック(共通)
    if (AimKey0 == NULL || !utils::IsKeyDown(AimKey0) && !utils::IsKeyDown(AimKey1) || g.bShowMenu) {
        return false;
    }

    switch (AimKeyMode)
    {
    case 1: // and
        if (AimKey1 == NULL && utils::IsKeyDown(AimKey0))
            break;
        else if (!utils::IsKeyDown(AimKey0) || !utils::IsKeyDown(AimKey1))
            return false;
        else if (!utils::IsKeyDown(AimKey0))
            return false;
        break;
    case 2: // or
        if (AimKey1 == NULL && utils::IsKeyDown(AimKey0))
            break;
        else if (utils::IsKeyDown(AimKey0) || utils::IsKeyDown(AimKey1))
            break;

        break;
    }

    return true;
}
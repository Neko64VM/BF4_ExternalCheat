#include "CFramework.h"

// Skeleton BoneList
constexpr int aSkeleton[][2]{
    { 142, 9 },
    { 9, 11 },
    { 11, 15 },
    { 142,109},
    { 109,111 },
    { 111, 115},
    { 142, 5 },
    { 5,  188},
    { 5, 197},
    { 188, 184},
    { 197, 198},
};

void CFramework::RenderInfo()
{
    // FovCircle
    if (g.AimBotEnable && g.bShowFOV)
        DrawCircle(Vector2(g.rcSize.right / 2.f, g.rcSize.bottom / 2.f), g.AimFOV, g.bRainbowFOV ? GenerateRainbow(5.f) : g.Color_AimFOV, 0.35f);

    // Crosshair
    if (g.CrosshairEnable)
    {
        switch (g.CrosshairType)
        {
        case 0: {
            ImVec2 Center = ImVec2(g.rcSize.right / 2, g.rcSize.bottom / 2);
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(Center.x - g.CrosshairSize, Center.y), ImVec2((Center.x + g.CrosshairSize) + 1, Center.y), g.Color_Crosshair, 1);
            ImGui::GetBackgroundDrawList()->AddLine(ImVec2(Center.x, Center.y - g.CrosshairSize), ImVec2(Center.x, (Center.y + g.CrosshairSize) + 1), g.Color_Crosshair, 1);
        }   break;
        case 1:
            ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2((float)g.rcSize.right / 2.f, (float)g.rcSize.bottom / 2.f), g.CrosshairSize + 1, ImColor(0.f, 0.f, 0.f, 1.f), NULL);
            ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2((float)g.rcSize.right / 2.f, (float)g.rcSize.bottom / 2.f), g.CrosshairSize, g.Color_Crosshair, NULL);
            break;
        }
    }

    // SpectatorList
    auto spectator_list = GetSpectatorList();
    if (g.SpectatorList && spectator_list.size() > 0)
    {
        String(Vector2(g.rcSize.right / 2 - (ImGui::CalcTextSize("[ Spectator Found! ]").x), g.rcSize.top), TEXT_COLOR, 1.f, "[ Spectator Found! ]");

        ImGui::SetNextWindowBgAlpha(spectator_list.size() > 0 ? 0.9f : 0.35f);
        ImGui::SetNextWindowPos(ImVec2(12.f, 16.f));
        ImGui::SetNextWindowSize(ImVec2(250.f, 125.f));
        std::string title = "Spectator [" + std::to_string(spectator_list.size()) + "]";
        ImGui::Begin(title.c_str(), &g.bShowMenu, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

        for (const auto& name : spectator_list)
            ImGui::Text(name.c_str());

        spectator_list.clear();

        ImGui::End();
    }
}

void CFramework::RenderESP()
{
    // Local
    CPlayer local;
    uintptr_t pClientGameContext = m.Read<uintptr_t>(m.m_dwProcessBaseAddr + offset::ClientgameContext);
    local.m_address = m.ReadChain(pClientGameContext, { offset::PlayerManager, offset::LocalPlayer });

    // Localの更新に失敗したらこの関数を終了
    if (!local.UpdateStatic())
        return;

    if (!local.Update())
        return;

    if (local.IsInVehicle()) {
        if (!local.VehicleUpdate())
            return;
    }

    // ViewMatrixとかいろいろ
    AllBones* BoneList{};
    uintptr_t GameRenderer = m.Read<uintptr_t>(m.m_dwProcessBaseAddr + offset::GameRenderer);

    if (!GameRenderer)
        return;

    uintptr_t RenderView = m.Read<uintptr_t>(GameRenderer + 0x60);
    Matrix ViewMatrix = m.Read<Matrix>(RenderView + 0x420);
    ImColor shadow_color = WithAlpha(g.Color_ESP_Shadow, g.m_flShadowAlpha);

    CPlayer* pLocal = &local;

    // Soldier
    for (auto& player : GetPlayerList())
    {
        CPlayer* pEntity = &player;

        if (!pEntity->Update())
            continue;

        // 距離を取得
        const float flDistance = GetDistance(pLocal->m_vecAbsOrigin, pEntity->m_vecAbsOrigin);

        // 各種チェック
        if (g.ESP_MaxDistance < flDistance)
            continue;
        else if (!g.ESP_Team && pEntity->m_iTeamNum == pLocal->m_iTeamNum)
            continue;

        if (g.AimBotEnable || g.bSkeleton)
        {
            auto list = pEntity->GetBoneList();
            BoneList = &list;
        }

        // ソースエンジンの vecMin/vecMax に該当するのがこれ。あとは一緒
        Vector3 min = pEntity->GetAABB().Min + pEntity->m_vecAbsOrigin;
        Vector3 max = pEntity->GetAABB().Max + pEntity->m_vecAbsOrigin;

        int left, top, right, bottom;
        Vector2 flb, brt, blb, frt, frb, brb, blt, flt;

        Vector3 points[8] = { Vector3(min.x, min.y, min.z), Vector3(min.x, max.y, min.z), Vector3(max.x, max.y, min.z),
                    Vector3(max.x, min.y, min.z), Vector3(max.x, max.y, max.z), Vector3(min.x, max.y, max.z),
                    Vector3(min.x, min.y, max.z), Vector3(max.x, min.y, max.z) };

        if (!WorldToScreen(ViewMatrix, g.rcSize, points[3], flb) || !WorldToScreen(ViewMatrix, g.rcSize, points[5], brt) ||
            !WorldToScreen(ViewMatrix, g.rcSize, points[0], blb) || !WorldToScreen(ViewMatrix, g.rcSize, points[4], frt) ||
            !WorldToScreen(ViewMatrix, g.rcSize, points[2], frb) || !WorldToScreen(ViewMatrix, g.rcSize, points[1], brb) ||
            !WorldToScreen(ViewMatrix, g.rcSize, points[6], blt) || !WorldToScreen(ViewMatrix, g.rcSize, points[7], flt))
            continue;

        Vector2 vec2_array[] = { flb, brt, blb, frt, frb, brb, blt, flt };
        left = flb.x;
        top = flb.y;
        right = flb.x;
        bottom = flb.y;

        for (auto j = 1; j < 8; ++j)
        {
            if (left > vec2_array[j].x)
                left = vec2_array[j].x;
            if (bottom < vec2_array[j].y)
                bottom = vec2_array[j].y;
            if (right < vec2_array[j].x)
                right = vec2_array[j].x;
            if (top > vec2_array[j].y)
                top = vec2_array[j].y;
        }

        // サイズ算出
        const int Height = bottom - top;
        const int Width = right - left;
        const int Center = (right - left) / 2.f;
        const int bScale = (right - left) / 3.f;

        // 色を決める
        ImColor color = WithAlpha(pLocal->m_iTeamNum == pEntity->m_iTeamNum ? g.Color_ESP_Team : (pEntity->IsVisible() ? g.Color_ESP_Visible : g.Color_ESP_Enemy), g.m_flGlobalAlpha); // Apply alpha

        /*
        * // AimTarget color
        if (pEntity->m_address == lastTarget.m_address)
            tempColor = g.Color_ESP_AimTarget;
        */

        // Line
        if (g.bLine)
            DrawLine(Vector2(g.rcSize.right / 2.f, g.rcSize.bottom), Vector2(right - (Width / 2), bottom), color);

        // Box
        if (g.bBox)
        {
            // BoxFilled
            if (g.bFilled)
                ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(left, top), ImVec2(right, bottom), g.Color_ESP_Shadow);

            // Box - Shadow
            DrawLine(Vector2(left - 1, top - 1), Vector2(right + 2, top - 1), shadow_color);
            DrawLine(Vector2(left - 1, top), Vector2(left - 1, bottom + 2), shadow_color);
            DrawLine(Vector2(right + 1, top), Vector2(right + 1, bottom + 2), shadow_color);
            DrawLine(Vector2(left - 1, bottom + 1), Vector2(right + 1, bottom + 1), shadow_color);

            switch (g.ESP_BoxType)
            {
            case 0:
                // Main
                DrawLine(Vector2(left, top), Vector2(right, top), color);
                DrawLine(Vector2(left, top), Vector2(left, bottom), color);
                DrawLine(Vector2(right, top), Vector2(right, bottom), color);
                DrawLine(Vector2(left, bottom), Vector2(right + 1, bottom), color);
                break;
            case 1:
                DrawLine(Vector2(left, top), Vector2(left + bScale, top), color); // Top
                DrawLine(Vector2(right, top), Vector2(right - bScale, top), color);
                DrawLine(Vector2(left, top), Vector2(left, top + bScale), color); // Left
                DrawLine(Vector2(left, bottom), Vector2(left, bottom - bScale), color);
                DrawLine(Vector2(right, top), Vector2(right, top + bScale), color); // Right
                DrawLine(Vector2(right, bottom), Vector2(right, bottom - bScale), color);
                DrawLine(Vector2(left, bottom), Vector2(left + bScale, bottom), color); // Bottom
                DrawLine(Vector2(right, bottom), Vector2(right - bScale, bottom), color);
                break;
            }
        }

        if (g.bSkeleton)
        {
            // Skeleton
            for (int j = 0; j < 11; j++)
            {
                if (Vec3_Empty(BoneList->bone[aSkeleton[j][0]].pos) || Vec3_Empty(BoneList->bone[aSkeleton[j][1]].pos))
                    break;

                Vector2 vOut0{}, vOut1{};
                if (WorldToScreen(ViewMatrix, g.rcSize, BoneList->bone[aSkeleton[j][0]].pos, vOut0) && WorldToScreen(ViewMatrix, g.rcSize, BoneList->bone[aSkeleton[j][1]].pos, vOut1))
                    DrawLine(vOut0, vOut1, color);
            }

            // Head Circle
            if (!Vec3_Empty(BoneList->bone[104].pos) || !Vec3_Empty(BoneList->bone[142].pos)) {
                Vector2 vHead{}, vNeck{};
                if (WorldToScreen(ViewMatrix, g.rcSize, BoneList->bone[104].pos, vHead) && WorldToScreen(ViewMatrix, g.rcSize, BoneList->bone[142].pos, vNeck)) {
                    DrawCircle(vHead, (vNeck.y - vHead.y) * 1.5f, color);
                }
            }
        }

        // Healthbar
        if (g.bHealth)
            HealthBar(left - 4.f, bottom, 2, -Height, pEntity->m_fHealth, pEntity->m_fMaxHealth, shadow_color, g.m_flGlobalAlpha); // Health

        // Distance
        if (g.bDistance) {
            const std::string DistStr = std::to_string((int)flDistance) + "m";
            StringEx(Vector2(right - Center - (ImGui::CalcTextSize(DistStr.c_str()).x / 2.f), bottom + 1), shadow_color, g.m_flGlobalAlpha, ImGui::GetFontSize(), DistStr.c_str());
        }

        // Name
        if (g.bName)
            StringEx(Vector2(right - Center - (ImGui::CalcTextSize(pEntity->pName).x / 2.f), top - ImGui::GetFontSize() - 1), shadow_color, g.m_flGlobalAlpha, ImGui::GetFontSize(), pEntity->pName);

        /*
        // AimBot
        if (g.g_AimBot)
        {
            if (!this->IsAimBotAllow())
                continue;
            else if (g.g_AimMaxDistance < flDistance)
                continue;
            else if (g.g_VisCheck && !pEntity->IsVisible())
                continue;
            else if (pLocal->IsInVehicle())
                continue;

            for (const auto& BoneID : CheckBoneList)
            {
                Vector2 vOut{};
                if (!WorldToScreen(ViewMatrix, g.rcSize, BoneList->bone[BoneID].pos, vOut))
                    continue;

                // Fov check
                FOV = abs((ScreenMiddle - vOut).Length());

                if (FOV > g.g_AimFOV)
                {
                    switch (g.g_AimTargetSelectMode)
                    {
                    case 0:
                        if (MinFov > FOV)
                        {
                            MinFov = FOV;
                            target = player;
                        }
                        break;
                    case 1:
                        if (MinDistance > flDistance)
                        {
                            MinDistance = flDistance;
                            target = player;
                        }
                        break;
                    default:
                        break;
                    }

                    break;
                }
            }
        }
        */
    }

    // Vehicle
    if (g.ESP_Vehicle && GetVehicleList().size() > 0)
    {
        for (auto& vehicle : GetVehicleList())
        {
            CPlayer* pVehicle = &vehicle;

            if (!pVehicle->Update() || !pVehicle->VehicleUpdate())
                continue;

            // 距離を取得
            const float pDistance = GetDistance(pLocal->m_vecAbsOrigin, pVehicle->m_vecAbsOrigin);

            // 各種チェック
            if (g.ESP_MaxDistance < pDistance)
                continue;
            else if (!g.ESP_Team && pVehicle->m_iTeamNum == pLocal->m_iTeamNum)
                continue;

            // Like SourceEngine games
            AxisAlignedBox VehicleAABB = pVehicle->GetVehicleAABB();
            Vector3 CurrentPosition = Vec3_Empty(pVehicle->m_vecAbsOrigin) ? pVehicle->m_vecAbsOrigin : pVehicle->m_vecAbsOrigin;
            Vector3 min = VehicleAABB.Min + CurrentPosition;
            Vector3 max = VehicleAABB.Max + CurrentPosition;

            int left, top, right, bottom;
            Vector2 flb, brt, blb, frt, frb, brb, blt, flt;

            Vector3 points[8] = { Vector3(min.x, min.y, min.z), Vector3(min.x, max.y, min.z), Vector3(max.x, max.y, min.z),
                        Vector3(max.x, min.y, min.z), Vector3(max.x, max.y, max.z), Vector3(min.x, max.y, max.z),
                        Vector3(min.x, min.y, max.z), Vector3(max.x, min.y, max.z) };

            if (!WorldToScreen(ViewMatrix, g.rcSize, points[3], flb) || !WorldToScreen(ViewMatrix, g.rcSize, points[5], brt) ||
                !WorldToScreen(ViewMatrix, g.rcSize, points[0], blb) || !WorldToScreen(ViewMatrix, g.rcSize, points[4], frt) ||
                !WorldToScreen(ViewMatrix, g.rcSize, points[2], frb) || !WorldToScreen(ViewMatrix, g.rcSize, points[1], brb) ||
                !WorldToScreen(ViewMatrix, g.rcSize, points[6], blt) || !WorldToScreen(ViewMatrix, g.rcSize, points[7], flt))
                continue;

            Vector2 vec2_array[] = { flb, brt, blb, frt, frb, brb, blt, flt };
            left = flb.x;
            top = flb.y;
            right = flb.x;
            bottom = flb.y;

            for (auto j = 1; j < 8; ++j)
            {
                if (left > vec2_array[j].x)
                    left = vec2_array[j].x;
                if (bottom < vec2_array[j].y)
                    bottom = vec2_array[j].y;
                if (right < vec2_array[j].x)
                    right = vec2_array[j].x;
                if (top > vec2_array[j].y)
                    top = vec2_array[j].y;
            }

            // サイズ算出
            const int Height = bottom - top;
            const int Width = right - left;
            const int Center = (right - left) / 2.f;

            // 色を決める
            ImColor color = WithAlpha(pLocal->m_iTeamNum == pVehicle->m_iTeamNum ? g.Color_ESP_TeamVehicle : g.Color_ESP_EnemyVehicle, g.m_flGlobalAlpha);

            bool isSoldier = pVehicle->m_fHealth != 0.f ? true : false;

            float Health = isSoldier ? pVehicle->m_fVehicleHealth : pVehicle->m_fHealth;
            float MaxHealth = isSoldier ? pVehicle->m_fVehicleMaxHealth : pVehicle->m_fMaxHealth;

            if (!isSoldier)
            {
                // Line
                if (g.bLine)
                    DrawLine(Vector2(g.rcSize.right / 2.f, g.rcSize.bottom), Vector2(right - (Width / 2), bottom), color);

                // Box
                if (g.bBox)
                {
                    // Box - Shadow
                    DrawLine(Vector2(left - 1, top - 1), Vector2(right + 2, top - 1), shadow_color);
                    DrawLine(Vector2(left - 1, top), Vector2(left - 1, bottom + 2), shadow_color);
                    DrawLine(Vector2(right + 1, top), Vector2(right + 1, bottom + 2), shadow_color);
                    DrawLine(Vector2(left - 1, bottom + 1), Vector2(right + 1, bottom + 1), shadow_color);

                    // Main
                    DrawLine(Vector2(left, top), Vector2(right, top), color);
                    DrawLine(Vector2(left, top), Vector2(left, bottom), color);
                    DrawLine(Vector2(right, top), Vector2(right, bottom), color);
                    DrawLine(Vector2(left, bottom), Vector2(right + 1, bottom), color);
                }

                // Healthbar
                if (g.bHealth) {
                    HealthBar(left - 4.f, bottom, 2, -Height, pVehicle->m_fVehicleHealth, pVehicle->m_fVehicleMaxHealth, shadow_color, g.m_flGlobalAlpha); // Health
                }

                // Distance
                if (g.bDistance) {
                    const std::string DistStr = std::to_string((int)pDistance) + "m";
                    StringEx(Vector2(right - Center - (ImGui::CalcTextSize(DistStr.c_str()).x / 2.f), bottom + 1), shadow_color, g.m_flGlobalAlpha, ImGui::GetFontSize(), DistStr.c_str());
                }

                // Name
                if (g.bName) {
                    StringEx(Vector2(right - Center - (ImGui::CalcTextSize(pVehicle->pName).x / 2.f), top - ImGui::GetFontSize() - 1), shadow_color, g.m_flGlobalAlpha, ImGui::GetFontSize(), pVehicle->pName);
                }
            }
            else
            {
                // Skeleton
                if (g.bSkeleton && isSoldier)
                {
                    AllBones bx = pVehicle->GetBoneList(), * bone = &bx;

                    // Skeleton
                    for (int j = 0; j < 11; j++)
                    {
                        if (Vec3_Empty(bone->bone[aSkeleton[j][0]].pos) || Vec3_Empty(bone->bone[aSkeleton[j][1]].pos))
                            break;

                        Vector2 vOut0{}, vOut1{};
                        if (WorldToScreen(ViewMatrix, g.rcSize, bone->bone[aSkeleton[j][0]].pos, vOut0) && WorldToScreen(ViewMatrix, g.rcSize, bone->bone[aSkeleton[j][1]].pos, vOut1))
                            DrawLine(vOut0, vOut1, color);
                    }

                    // Head Circle
                    if (!Vec3_Empty(bone->bone[104].pos) || !Vec3_Empty(bone->bone[142].pos)) {
                        Vector2 vHead{}, vNeck{};
                        if (WorldToScreen(ViewMatrix, g.rcSize, bone->bone[104].pos, vHead) && WorldToScreen(ViewMatrix, g.rcSize, bone->bone[142].pos, vNeck)) {
                            DrawCircle(vHead, (vNeck.y - vHead.y) * 1.5f, color, g.m_flGlobalAlpha);
                        }
                    }
                }
            }
        }
    }
}
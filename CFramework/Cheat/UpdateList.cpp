#include "CFramework.h"

const int ReadCount = 72;

struct entitylist_t {
    uintptr_t address[ReadCount];
};

std::vector<CPlayer> CFramework::GetPlayerList() {
    std::lock_guard<std::mutex> lock(p_mutex);
    return m_vecPlayerList;
}

std::vector<CPlayer> CFramework::GetVehicleList() {
    std::lock_guard<std::mutex> lock(v_mutex);
    return m_vecVehicleList;
}

std::vector<std::string> CFramework::GetSpectatorList() {
    std::lock_guard<std::mutex> lock(s_mutex);
    return m_vecSpectatorList;
}

void CFramework::UpdateList()
{
    while (g_ApplicationActive)
    {
        std::vector<CPlayer> player_list;
        std::vector<CPlayer> vehicle_list;
        std::vector<std::string> spectator_list;
        std::this_thread::sleep_for(std::chrono::milliseconds(333));

        // R
        uintptr_t pClientGameContext = m.Read<uintptr_t>(m.m_dwProcessBaseAddr + offset::ClientgameContext);
        uintptr_t pPlayerManager = m.Read<uintptr_t>(pClientGameContext + offset::PlayerManager);
        uintptr_t pEntityList = m.Read<uintptr_t>(pPlayerManager + offset::ClientPlayer);

        // Local
        CPlayer local = CPlayer();
        local.m_address = m.Read<uintptr_t>(pPlayerManager + offset::LocalPlayer);

        if (!local.UpdateStatic())
            continue;

        // 1RPMで全プレイヤーのポインタを読み取る
        entitylist_t player = m.Read<entitylist_t>(pEntityList);

        for (int i = 0; i < ReadCount; i++)
        {
            // ポインタの有効性チェック
            if (player.address[i] != NULL && player.address[i] != local.m_address)
            {
                CPlayer p = CPlayer();
                p.m_address = player.address[i];

                if (!p.UpdateStatic())
                    continue;

                if (p.IsSpectator()) {
                    // Spectator ?
                    spectator_list.push_back(p.pName);
                }
                else if (p.IsInVehicle()) {
                    // InVehicle?
                    vehicle_list.push_back(p);
                }
                else {
                    // TeamCheck
                    if (!g.ESP_Team && local.m_iTeamNum == p.m_iTeamNum)
                        continue;

                    // Soldier
                    player_list.push_back(p);
                }
            }
        }

        std::lock_guard<std::mutex> lock_a(p_mutex);
        std::lock_guard<std::mutex> lock_b(v_mutex);
        std::lock_guard<std::mutex> lock_c(s_mutex);
        m_vecPlayerList = player_list;
        m_vecVehicleList = vehicle_list;
        m_vecSpectatorList = spectator_list;
        player_list.clear();
        vehicle_list.clear();
        spectator_list.clear();
    }
}
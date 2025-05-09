#pragma once
#include "../Framework/ImGui/imgui.h"
#include "../Framework/ImGui/imgui_impl_win32.h"
#include "../Framework/ImGui/imgui_impl_dx11.h"
#include "../Framework/ImGui/Fonts/fa.h"
#include "../Framework/ImGui/Fonts/IconsFontAwesome6.h"
#include "../Framework/ImGui/Custom.h"
#include "SDK/CEntity/CEntity.h"
#include <mutex>
#include <atomic>
#pragma comment(lib, "WinMM.lib")
#pragma comment(lib, "freetype.lib")

class CFramework
{
public:
    ImFont* icon;

    void UpdateList();
    void MiscAll();

	void RenderInfo();
	void RenderMenu();
	void RenderESP();
private:
    // Thread safe
    std::mutex p_mutex;
    std::mutex v_mutex;
    std::mutex s_mutex;
    std::vector<CPlayer> m_vecPlayerList{};
    std::vector<CPlayer> m_vecVehicleList{};
    std::vector<std::string> m_vecSpectatorList{};
    std::vector<CPlayer> GetPlayerList();
    std::vector<CPlayer> GetVehicleList();
    std::vector<std::string> GetSpectatorList();

    ImColor TEXT_COLOR = { 1.f, 1.f, 1.f, 1.f };
    ImColor BLACK_COLOR = ImColor(0.f, 0.f, 0.f, 1.f);

    // AimBot KeyChecker
    bool AimBotKeyCheck(DWORD &AimKey0, DWORD &AimKey1, int AimKeyMode);

    // Render.cpp
    ImVec2 ToImVec2(const ImVec2& value); // Convert to int
    ImVec2 ToImVec2(const Vector2& value);
    ImColor WithAlpha(const ImColor& color, const float& alpha);
    float GetHueFromTime(float speed = 5.0f);
    ImColor GenerateRainbow(float speed = 5.0f);
    void DrawLine(const Vector2 p1, const Vector2 p2, ImColor color, float thickness = 1.f);
    void DrawBox(int right, int left, int top, int bottom, ImColor color, float thickness = 1.f);
    void DrawCircle(const Vector2 pos, float size, ImColor color);
    void DrawCircle(const Vector2 pos, float size, ImColor color, float alpha);
    void DrawCircleFilled(const Vector2 pos, float size, ImColor color, float alpha);
    void RectFilled(int x0, int y0, int x1, int y1, ImColor color);
    void RectFilled(int x0, int y0, int x1, int y1, ImColor color, float alpha);
    void HealthBar(int x, int y, int w, int h, int value, int v_max, ImColor shadow, float global_alpha);
    void String(const Vector2 pos, ImColor color, float alpha, const char* text);
    void StringEx(Vector2 pos, ImColor shadow_color, float global_alpht, float font_size, const char* text);
};
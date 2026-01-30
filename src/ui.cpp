#include "ui.hpp"
#include "appstate.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "IconsFontAwesome7.h"
#include "IconsMaterialSymbols.h"
#include <format>
#include <initializer_list>

constexpr const char* CurrentTab_to_str(CurrentTab ct) {
    if (ct == CurrentTab::Alarms)
        return "Alarms";
    else if (ct == CurrentTab::PomodoroTimer)
        return "Pomodoro Timer";
    else if (ct == CurrentTab::Timer)
        return "Timer";
    else if (ct == CurrentTab::Stopwatch)
        return "Stopwatch";
    return "Error kasdjflksjdfjaslkfdj";
}

constexpr const char* CurrentTab_to_icon(CurrentTab ct) {
    if (ct == CurrentTab::Alarms)
        return ICON_MS_ALARM;
    else if (ct == CurrentTab::PomodoroTimer)
        return ICON_MS_TIMELAPSE;
    else if (ct == CurrentTab::Timer)
        return ICON_FA_HOURGLASS_START;
    else if (ct == CurrentTab::Stopwatch)
        return ICON_FA_STOPWATCH;
    return "Error alskjdfkls;akls";
}

void draw_sidebar(CurrentTab& current_tab) {
    ImGuiViewportP* viewport = (ImGuiViewportP*)(void*)ImGui::GetMainViewport();
    // Toolbar Window (Panel) uses as BeginViewportSideBar.
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNavFocus;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 10));
    ImGui::PushFont(NULL, 22.0f);

    float default_alpha_button = ImGui::GetStyleColorVec4(ImGuiCol_Button).w;
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Button].w = 0.0f;

    ImGui::BeginViewportSideBar("##ToolBar", viewport, ImGuiDir_Left, 0.0f, windowFlags);

    for (CurrentTab ct : {CurrentTab::PomodoroTimer, CurrentTab::Timer, CurrentTab::Alarms, CurrentTab::Stopwatch}) {
        bool changed = false;
        ImVec4 orig = colors[ImGuiCol_Button];
        if (ct == current_tab)
            colors[ImGuiCol_Button] = ImVec4(0.59f, 0.59f, 0.59f, 0.40f);

        if (ImGui::Button(std::format("{} {}", CurrentTab_to_icon(ct), CurrentTab_to_str(ct)).c_str())) {
            current_tab = ct;
            changed = true;
        }

        if (ct == current_tab && !changed)
            colors[ImGuiCol_Button] = orig;
    }

    ImGui::End();
    ImGui::PopStyleVar();
    ImGui::PopFont();

    ImGui::GetStyle().Colors[ImGuiCol_Button].w = default_alpha_button;
}


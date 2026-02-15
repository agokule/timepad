#include "timer_creater.hpp"
#include "imgui.h"
#include <cstdio>
#include <optional>
#include "misc.hpp"
#include "ui/timer_display.hpp"

TimerCreater::TimerCreater(): hoursM(0), minutesM(0), secondsM(0) {
}

std::optional<TimerDisplay> TimerCreater::draw() {
    std::optional<TimerDisplay> return_val = std::nullopt;
    ImGui::Begin("Create a Timer");

    constexpr const char* formats[] = { "%d h", "%d m", "%d s" };
    constexpr const char* ids[] = { "h", "m", "s" };

    for (int i = 0; i < 3; i++) {
        char buf[64];
        snprintf(buf, 64, formats[i], (&hoursM)[i]);
        float width = ImGui::CalcTextSize(buf).x + ImGui::GetStyle().FramePadding.x * 4;

        int min = 0, max = 0;
        if (i != 0)
            max = 59;

        ImGui::SetNextItemWidth(width);
        ImGui::DragInt(ids[i], &(&hoursM)[i], 0.2, min, max, formats[i]);
        ImGui::SameLine();
    }

    HelpMarker(
                "Click and drag to edit value.\n"
                "Hold Shift or Alt for faster/slower edit.\n"
                "Double-Click or Ctrl+Click to input value.");

    if (ImGui::Button("Create Timer")) {
        int sec = secondsM;
        sec += minutesM * 60;
        sec += hoursM * 3600;
        if (sec != 0)
            return_val = {sec};
    }

    ImGui::End();
    return return_val;
}


#include "misc.hpp"
#include "imgui.h"
#include "IconsFontAwesome7.h"
#include <format>

void TimerInput(int* hours, int* minutes, int* seconds) {
    constexpr const char* std_formats[] = { "{} h", "{} m", "{} s" };
    constexpr const char* c_formats[] = { "%d h", "%d m", "%d s" };
    constexpr const char* ids[] = { "h", "m", "s" };
    int* times[] = {hours, minutes, seconds};

    for (int i = 0; i < 3; i++) {
        auto disp = std::vformat(std_formats[i], std::make_format_args(*times[i]));
        float width = ImGui::CalcTextSize(disp.c_str()).x + ImGui::GetStyle().FramePadding.x * 4;

        int min = 0, max = 0;
        if (i != 0)
            max = 59;

        ImGui::SetNextItemWidth(width);
        ImGui::DragInt(ids[i], times[i], 0.2, min, max, c_formats[i]);
        ImGui::SameLine();
    }

    HelpMarker(
                "Click and drag to edit value.\n"
                "Hold Shift or Alt for faster/slower edit.\n"
                "Double-Click or Ctrl+Click to input value.");
}

// taken from the imgui example code and modified
void HelpMarker(const char* desc) {
    ImGui::TextDisabled(ICON_FA_CIRCLE_QUESTION);
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}


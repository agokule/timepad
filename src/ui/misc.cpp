#include "misc.hpp"
#include "imgui.h"
#include "IconsFontAwesome7.h"

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


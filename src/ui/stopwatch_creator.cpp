#include "stopwatch_creator.hpp"
#include "imgui.h"
#include "ui/stopwatch_display.hpp"
#include <optional>

std::optional<StopwatchDisplay> StopwatchCreator::draw() {
    std::optional<StopwatchDisplay> ret = std::nullopt;

    auto pos = ImGui::GetIO().DisplaySize;
    pos.x -= 150; pos.y -= 50;

    ImGui::SetNextWindowPos(pos);
    ImGui::Begin("StopwatchCreator", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings);

    if (ImGui::Button("Create Stopwatch"))
        ret = StopwatchDisplay {};

    ImGui::End();

    return ret;
}


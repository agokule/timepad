#include "timer_creator.hpp"
#include "imgui.h"
#include <optional>
#include "misc.hpp"
#include "ui/timer_display.hpp"

TimerCreater::TimerCreater(): hoursM(0), minutesM(0), secondsM(0) {
}

std::optional<TimerDisplay> TimerCreater::draw() {
    std::optional<TimerDisplay> return_val = std::nullopt;
    ImGui::Begin("Create a Timer");

    TimerInput(nullptr, &hoursM, &minutesM, &secondsM);

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


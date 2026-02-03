#pragma once

#include <optional>
#include "timer_display.hpp"

class TimerCreater {
public:
    TimerCreater();

    std::optional<TimerDisplay> draw();
private:

    int hoursM;
    int minutesM;
    int secondsM;
};


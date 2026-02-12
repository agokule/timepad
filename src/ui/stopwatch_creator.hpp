#pragma once

#include <optional>
#include "stopwatch_display.hpp"

class StopwatchCreator {
public:
    std::optional<StopwatchDisplay> draw();
private:
};


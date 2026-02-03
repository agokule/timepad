#pragma once
#include <SDL3/SDL.h>
#include <optional>

enum class WhatIsFullscreen {
    Timer, Stopwatch, Pomodoro
};

enum class FocusType {
    None, Fullscreen, Popout
};

struct FocusState {
    std::optional<WhatIsFullscreen> what_is_focused = std::nullopt;
    FocusType type = FocusType::None;
    std::optional<unsigned long> id_of_focussed = std::nullopt;
};

enum class CurrentTab {
    PomodoroTimer = 1,
    Timer,
    Alarms,
    Stopwatch
};


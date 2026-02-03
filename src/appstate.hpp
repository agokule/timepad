#pragma once
#include <SDL3/SDL.h>
#include "ui/timer_creater.hpp"
#include "ui/timer_display.hpp"
#include <vector>

enum class CurrentTab {
    PomodoroTimer = 1,
    Timer,
    Alarms,
    Stopwatch
};

struct AppState {
    SDL_Window* window;
    SDL_Renderer* renderer;
    CurrentTab current_tab;
    std::vector<TimerDisplay> timers;
    TimerCreater timer_creater;
};



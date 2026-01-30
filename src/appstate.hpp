#pragma once
#include <SDL3/SDL.h>

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
};



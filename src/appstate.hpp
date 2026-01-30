#pragma once
#include <SDL3/SDL.h>
#include "circular_progress_bar.hpp"

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
    CircularProgressBar progress = {250, 250, 50, 20};
};



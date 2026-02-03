#include "SDL3/SDL_blendmode.h"
#include "SDL3/SDL_log.h"
#include "ui/timer_display.hpp"
#include <iostream>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include <thread>
#include "IconsFontAwesome7.h"
#include "IconsMaterialSymbols.h"
#include "appstate.hpp"
#include "ui/sidebar.hpp"
#include "ui/timer_creater.hpp"
#include <vector>

struct AppState {
    SDL_Window* window;
    SDL_Renderer* renderer;
    CurrentTab current_tab;
    std::vector<TimerDisplay> timers;
    TimerCreater timer_creater;
    FocusState focus_state;
};

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    AppState *state = new AppState;

    /* Create the window */
    if (!SDL_CreateWindowAndRenderer("Hello World", 800, 600, SDL_WINDOW_RESIZABLE, &state->window, &state->renderer)) {
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    state->current_tab = CurrentTab::PomodoroTimer;

    SDL_SetRenderVSync(state->renderer, 1);
    SDL_SetRenderDrawBlendMode(state->renderer, SDL_BLENDMODE_BLEND);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    io.Fonts->AddFontFromFileTTF("./assets/fonts/Roboto-Regular.ttf", 17.0f);

    // merge in icons from Font Awesome
    ImFontConfig icons_config; 
    icons_config.MergeMode = true; 
    io.Fonts->AddFontFromFileTTF("./assets/fonts/Font Awesome 7 Free-Regular-400.otf", 0.0f, &icons_config);
    io.Fonts->AddFontFromFileTTF("./assets/fonts/Font Awesome 7 Free-Solid-900.otf", 0.0f, &icons_config);
    io.Fonts->AddFontFromFileTTF("./assets/fonts/MaterialSymbolsRounded-Regular.ttf", 0.0f, &icons_config);

    ImGui::StyleColorsDark();
    ImGui_ImplSDL3_InitForSDLRenderer(state->window, state->renderer);
    ImGui_ImplSDLRenderer3_Init(state->renderer);

    std::cout << "This thread id: " << std::this_thread::get_id() << std::endl;
    std::cerr << "sldkfjsdklfjsjfthread id: " << std::this_thread::get_id() << std::endl;

    *appstate = state;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    AppState &state = *static_cast<AppState*>(appstate);

    if (event->type == SDL_EVENT_QUIT)
        return SDL_APP_SUCCESS;

    ImGui_ImplSDL3_ProcessEvent(event);
    if (ImGui::GetIO().WantCaptureMouse) return SDL_APP_CONTINUE;
    if (ImGui::GetIO().WantCaptureKeyboard) return SDL_APP_CONTINUE;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    AppState &state = *static_cast<AppState*>(appstate);
    SDL_Renderer *renderer = state.renderer;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    ImGui_ImplSDL3_NewFrame();
    ImGui_ImplSDLRenderer3_NewFrame();

    ImGui::NewFrame();

    if (state.focus_state.type == FocusType::None) {
        draw_sidebar(state.current_tab);

        if (state.current_tab == CurrentTab::Timer) {
            for (TimerDisplay& timer : state.timers) {
                timer.update();
                auto focus_state = timer.draw(renderer);
                if (focus_state.has_value())
                    state.focus_state = *focus_state;
            }

            auto new_timer = state.timer_creater.draw();
            if (new_timer.has_value())
                state.timers.push_back(*new_timer);
        }

        ImGui::ShowDemoWindow();
    } else if (state.focus_state.type == FocusType::Fullscreen) {
        assert(state.focus_state.what_is_focused.has_value());
        if (*state.focus_state.what_is_focused == WhatIsFullscreen::Timer) {
            assert(state.focus_state.id_of_focussed.has_value());
            for (auto& timer : state.timers) {
                if (timer.get_id() != *state.focus_state.id_of_focussed)
                    continue;
                auto focus_state = timer.draw(renderer);
                if (focus_state.has_value())
                    state.focus_state = *focus_state;
                break;
            }
        }
    }

    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);

    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    AppState *state = static_cast<AppState *>(appstate);
    ImGui_ImplSDL3_Shutdown();
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui::DestroyContext();
    delete state;
}

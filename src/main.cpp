#include "audio_player.hpp"
#include "ui/misc.hpp"
#include "ui/pomodoro_timer.hpp"
#include "ui/stopwatch_creator.hpp"
#include <cstdlib>
#include <print>
#include "ui/timer_display.hpp"
#include <algorithm>
#include <iostream>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include <thread>
#include "appstate.hpp"
#include "ui/sidebar.hpp"
#include "ui/timer_creator.hpp"
#include <vector>
#include "miniaudio.h"
#include "constants.hpp"
#include <filesystem>

using namespace std::chrono_literals;
namespace fs = std::filesystem;

struct PopoutWindow {
    SDL_Window* window;
    SDL_Renderer* renderer;
    ImGuiContext* imgui_ctx;
    SDL_WindowID window_id;
    FocusState focus_state;
    bool should_close;
};

struct PomodoroTimerCreator {
    // 30 min * 60sec/min = number of seconds per 30min
    unsigned work_time_s = 30 * 60;
    unsigned break_time_s = 5 * 60;
    unsigned repeat = 3;
};

struct AppState {
    SDL_Window* window;
    SDL_Renderer* renderer;
    ImGuiContext* main_imgui_ctx;
    CurrentTab current_tab;
    std::vector<TimerDisplay> timers;
    TimerCreater timer_creater;
    StopwatchCreator stopwatch_creator;
    std::vector<StopwatchDisplay> stopwatches;
    FocusState focus_state;
    std::vector<PopoutWindow> popouts;
    PomodoroTimerCreator pomodoro_creator;
    std::optional<PomodoroTimer> pomodoro_timer;
    AudioPlayer audio_player {ASSETS_FOLDER "sound/freesound_community-kitchen-timer-87485.mp3"};
};

void configure_imgui_ctx() {
    ImGuiIO &io = ImGui::GetIO();

#ifdef DISTRIBUTION
    auto config_home = std::getenv("XDG_CONFIG_HOME");
    fs::path global_config;
    if (config_home)
        global_config = config_home;
    else {
        fs::path home = std::getenv("HOME");
        global_config = home / ".config/";
    }
    fs::path config_dir = global_config / "timepad/";
    if (!fs::exists(config_dir))
        fs::create_directory(config_dir);
    static fs::path ini_file = config_dir / "imgui.ini";
    io.IniFilename = ini_file.c_str();
#endif // ifdef DISTRIBUTION

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    io.Fonts->AddFontFromFileTTF(ASSETS_FOLDER "fonts/Roboto-Regular.ttf", 17.0f);

    // merge in icons from Font Awesome
    ImFontConfig icons_config; 
    icons_config.MergeMode = true; 
    io.Fonts->AddFontFromFileTTF(ASSETS_FOLDER "fonts/Font Awesome 7 Free-Regular-400.otf", 0.0f, &icons_config);
    io.Fonts->AddFontFromFileTTF(ASSETS_FOLDER "fonts/Font Awesome 7 Free-Solid-900.otf", 0.0f, &icons_config);
    io.Fonts->AddFontFromFileTTF(ASSETS_FOLDER "fonts/MaterialSymbolsRounded-Regular.ttf", 0.0f, &icons_config);

    ImGui::StyleColorsDark();
}

void configure_sdl_renderer(SDL_Renderer* renderer) {
    SDL_SetRenderVSync(renderer, 2);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
}

void create_popout_window(AppState& app, FocusState focus) {
    PopoutWindow popout = {};
    
    // Create window for each new popout
    popout.window = SDL_CreateWindow(
        "Timer Popout", 
        200, 200, 
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALWAYS_ON_TOP
    );

    popout.focus_state = focus;
    
    popout.window_id = SDL_GetWindowID(popout.window);
    
    // Create renderer for the popout window
    popout.renderer = SDL_CreateRenderer(popout.window, nullptr);
    configure_sdl_renderer(popout.renderer);
    
    // Create separate ImGui context for popout
    popout.imgui_ctx = ImGui::CreateContext();
    ImGui::SetCurrentContext(popout.imgui_ctx);
    configure_imgui_ctx();
    
    // Copy ImGui style from main window (optional, for consistency)
    ImGui::GetStyle() = ImGui::GetStyle();
    
    // Initialize ImGui backends for SDL3 renderer
    ImGui_ImplSDL3_InitForSDLRenderer(popout.window, popout.renderer);
    ImGui_ImplSDLRenderer3_Init(popout.renderer);
    
    popout.should_close = false;
    
    // Add to vector
    app.popouts.push_back(popout);

    ImGui::SetCurrentContext(app.main_imgui_ctx);
}

void destroy_popout_window(PopoutWindow& popout, AppState& state) {
    ImGui::SetCurrentContext(popout.imgui_ctx);
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext(popout.imgui_ctx);
    
    SDL_DestroyRenderer(popout.renderer);
    SDL_DestroyWindow(popout.window);
    
    popout.should_close = true;

    if (*popout.focus_state.what_is_focused == WhatIsFullscreen::Timer) {
        for (auto& timer : state.timers)
            if (timer.get_id() == *popout.focus_state.id_of_focussed)
                timer.set_focus_type(FocusType::None);
    } else if (*popout.focus_state.what_is_focused == WhatIsFullscreen::Stopwatch) {
        for (auto& sw : state.stopwatches)
            if (sw.get_id() == *popout.focus_state.id_of_focussed)
                sw.set_focus_type(FocusType::None);
    } else if (*popout.focus_state.what_is_focused == WhatIsFullscreen::Pomodoro && state.pomodoro_timer.has_value())
        state.pomodoro_timer->set_focus_type(FocusType::None);

    ImGui::SetCurrentContext(state.main_imgui_ctx);
}

void close_popout_by_window_id(AppState& app, SDL_WindowID window_id) {
    app.popouts.erase(
            std::remove_if(app.popouts.begin(), app.popouts.end(), [window_id, &app](PopoutWindow& pp) {
                if (pp.window_id == window_id && !pp.should_close)
                    destroy_popout_window(pp, app);
                return pp.should_close;
                }),
            app.popouts.end()
        );
}

auto find_popout_by_window_id(AppState& app, SDL_WindowID window_id) {
    return std::find_if(app.popouts.begin(), app.popouts.end(), [window_id](PopoutWindow& pp) {
            return pp.window_id == window_id;
            });
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    AppState *state = new AppState;

    /* Create the window */
    if (!SDL_CreateWindowAndRenderer("Timepad", 800, 600, SDL_WINDOW_RESIZABLE, &state->window, &state->renderer)) {
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    state->current_tab = CurrentTab::PomodoroTimer;

    configure_sdl_renderer(state->renderer);

    IMGUI_CHECKVERSION();
    state->main_imgui_ctx = ImGui::CreateContext();
    configure_imgui_ctx();

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

    // Handle window close events
    if (event->type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
        // Close the specific popout window
        close_popout_by_window_id(state, event->window.windowID);
        return SDL_APP_CONTINUE;
    }

    SDL_Window* focused = SDL_GetMouseFocus();
    if (focused) {
        auto id = SDL_GetWindowID(focused);
        ImGui::GetCurrentContext();
        if (focused != state.window)
            ImGui::SetCurrentContext(find_popout_by_window_id(state, id)->imgui_ctx);
        else if (focused == state.window)
            ImGui::SetCurrentContext(state.main_imgui_ctx);
    }

    ImGui_ImplSDL3_ProcessEvent(event);
    if (ImGui::GetIO().WantCaptureMouse) return SDL_APP_CONTINUE;
    if (ImGui::GetIO().WantCaptureKeyboard) return SDL_APP_CONTINUE;

    return SDL_APP_CONTINUE;
}

void render_popout_window(AppState& app, PopoutWindow& popout) {
    ImGui::SetCurrentContext(popout.imgui_ctx);

    SDL_SetRenderDrawColor(popout.renderer, 50, 50, 50, 255);
    SDL_RenderClear(popout.renderer);
    
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    
    auto id = popout.focus_state.id_of_focussed;

    if (*popout.focus_state.what_is_focused == WhatIsFullscreen::Timer) {
        for (auto& timer : app.timers)
            if (timer.get_id() == *id)
                timer.draw(popout.renderer, app.audio_player);
    } else if (*popout.focus_state.what_is_focused == WhatIsFullscreen::Stopwatch) {
        for (auto& sw : app.stopwatches)
            if (sw.get_id() == *id)
                sw.draw();
    } else if (*popout.focus_state.what_is_focused == WhatIsFullscreen::Pomodoro) {
        if (app.pomodoro_timer.has_value())
            app.pomodoro_timer->draw(popout.renderer, app.audio_player);
        if ((app.pomodoro_timer.has_value() && app.pomodoro_timer->is_done()) || !app.pomodoro_timer.has_value())
            popout.should_close = true;
    }
    
    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), popout.renderer);
    SDL_RenderPresent(popout.renderer);
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    AppState &state = *static_cast<AppState*>(appstate);
    SDL_Renderer *renderer = state.renderer;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    ImGui::SetCurrentContext(state.main_imgui_ctx);

    ImGui_ImplSDL3_NewFrame();
    ImGui_ImplSDLRenderer3_NewFrame();

    ImGui::NewFrame();

    if (state.focus_state.type == FocusType::None)
        draw_sidebar(state.current_tab);

    if (state.current_tab == CurrentTab::Timer) {
        if (state.focus_state.type == FocusType::Fullscreen)
            assert(state.focus_state.what_is_focused.has_value());

        for (TimerDisplay& timer : state.timers) {
            if (timer.get_focus_type() == FocusType::Popout)
                continue;
            if (state.focus_state.type == FocusType::Fullscreen &&
                *state.focus_state.what_is_focused == WhatIsFullscreen::Timer &&
                *state.focus_state.id_of_focussed != timer.get_id())
                continue;

            auto focus_state = timer.draw(renderer, state.audio_player);
            if (focus_state.has_value() && focus_state->type != FocusType::Popout)
                state.focus_state = *focus_state;
            else if (focus_state.has_value() && focus_state->type == FocusType::Popout) {
                create_popout_window(state, *focus_state);
                state.focus_state = {};
            }
        }

        if (state.focus_state.type == FocusType::None) {
            auto new_timer = state.timer_creater.draw();
            if (new_timer.has_value())
                state.timers.push_back(*new_timer);
        }
    } else if (state.current_tab == CurrentTab::Stopwatch) {
        if (state.focus_state.type == FocusType::None){
            auto stopwatch = state.stopwatch_creator.draw();
            if (stopwatch.has_value())
                state.stopwatches.push_back(*stopwatch);
        }

        for (auto& sw : state.stopwatches) {
            if (sw.get_focus_type() == FocusType::Popout)
                continue;
            if (state.focus_state.type == FocusType::Fullscreen &&
                *state.focus_state.what_is_focused == WhatIsFullscreen::Stopwatch &&
                *state.focus_state.id_of_focussed != sw.get_id())
                continue;

            auto focus_state = sw.draw();
            if (focus_state.has_value() && focus_state->type != FocusType::Popout)
                state.focus_state = *focus_state;
            else if (focus_state.has_value() && focus_state->type == FocusType::Popout) {
                create_popout_window(state, *focus_state);
                state.focus_state = {};
            }
        }
    } else if (state.current_tab == CurrentTab::PomodoroTimer) {
        if (!state.pomodoro_timer.has_value()) {
            static int break_h {}, break_m {5}, break_s {};
            static int work_h {}, work_m {30}, work_s {};
            static int repeat {3};

            ImGui::Begin("Start a pomodoro timer");
            TimerInput("Work Time", &work_h, &work_m, &work_s);
            TimerInput("Break Time", &break_h, &break_m, &break_s);
            ImGui::InputInt("Repeat amount", &repeat);

            if (ImGui::Button("Create"))
                state.pomodoro_timer.emplace(
                        work_h * 3600 + work_m * 60 + work_s,
                        break_h * 3600 + break_m * 60 + break_s,
                        repeat);
            ImGui::End();
        } else if (state.pomodoro_timer->get_focus_type() != FocusType::Popout) {
            auto focus_state = state.pomodoro_timer->draw(renderer, state.audio_player);
            if (focus_state.has_value() && focus_state->type != FocusType::Popout)
                state.focus_state = *focus_state;
            else if (focus_state.has_value() && focus_state->type == FocusType::Popout) {
                create_popout_window(state, *focus_state);
                state.focus_state = {};
            }
        }
        if (state.pomodoro_timer.has_value() && state.pomodoro_timer->is_done())
            state.pomodoro_timer.reset();

    } else if (state.current_tab == CurrentTab::Alarms) {
        ImGui::Begin("Under Construction");
        ImGui::Text("This part of the app is not done yet, check back after an update!");
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);

    SDL_RenderPresent(renderer);

    for (auto it = state.popouts.begin(); it != state.popouts.end(); it++) {
        auto& popout = *it;
        render_popout_window(state, popout);
        if (popout.should_close) {
            destroy_popout_window(popout, state);
            it = state.popouts.erase(it);
            if (it == state.popouts.end()) break;
        }
    }
    ImGui::SetCurrentContext(state.main_imgui_ctx);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    AppState *state = static_cast<AppState *>(appstate);
    ImGui_ImplSDL3_Shutdown();
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui::DestroyContext();
    delete state;
}

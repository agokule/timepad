#include "SDL3/SDL_log.h"
#include <iostream>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include <thread>

struct AppState {
    SDL_Window* window;
    SDL_Renderer* renderer;
};

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    AppState *state = new AppState;

    /* Create the window */
    if (!SDL_CreateWindowAndRenderer("Hello World", 800, 600, SDL_WINDOW_RESIZABLE, &state->window, &state->renderer)) {
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

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

    ImGui_ImplSDL3_NewFrame();
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui::NewFrame();

    ImGui::Button("Hello");

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

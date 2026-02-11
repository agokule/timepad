#include "timer_display.hpp"
#include "IconsFontAwesome7.h"
#include "IconsMaterialSymbols.h"
#include "SDL3/SDL_timer.h"
#include "appstate.hpp"
#include "audio_player.hpp"
#include "imgui.h"
#include "ui/circular_progress_bar.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <optional>
#include <print>

TimerDisplay::TimerDisplay() 
    : timer_secondsM(60)
    , start_time_msM(0)
    , paused_time_msM(0)
    , paused_time_start_msM(0)
    , progress_barM(0, 0, 100, 12)
    , idM(SDL_GetTicks())
    , focusM(FocusType::None)
{
}

TimerDisplay::TimerDisplay(int timer_seconds)
    : timer_secondsM(timer_seconds)
    , start_time_msM(0)
    , paused_time_msM(0)
    , paused_time_start_msM(0)
    , progress_barM(0, 0, 100, 12)
    , idM(SDL_GetTicks())
    , focusM(FocusType::None)
{
}

void TimerDisplay::set_timer_value(int seconds) {
    timer_secondsM = seconds;
}

void TimerDisplay::update_progress_bar() {
    progress_barM.set_progress(calculate_time_progress_ms() / (timer_secondsM * 1000.0f));
}

void TimerDisplay::reset(AudioPlayer& ap) {
    std::println("Timer Reset");
    start_time_msM = 0;
    paused_time_msM = 0;
    paused_time_start_msM = 0;
    progress_barM.reset();

    if (ap.is_playing_or_not()) {
        ap.pause();
        ap.seek_to(0);
    }
}

void TimerDisplay::format_time(int seconds, char* buffer, size_t buffer_size) {
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    int secs = seconds % 60;
    snprintf(buffer, buffer_size, "%02d:%02d:%02d", hours, minutes, secs);
}

unsigned long TimerDisplay::calculate_time_progress_ms() {
    if (start_time_msM == 0)
        return 0;
    auto now = SDL_GetTicks();
    auto paused_time_ms = paused_time_msM;
    if (paused_time_start_msM != 0)
        paused_time_ms += now - paused_time_start_msM;

    auto progress_ms = (now - start_time_msM) - paused_time_ms;
    return progress_ms;
}

std::optional<FocusState> TimerDisplay::draw_header() {
    std::optional<FocusState> return_val = std::nullopt; 

    ImGui::BeginGroup();
    float defualt_button_size = 35.0f;
    float button_size = defualt_button_size;
    ImVec2 window_size = ImGui::GetWindowSize();

    if (std::min(window_size.x, window_size.y) < 250.0f) {
        button_size = 20.0f;
        ImGui::PushFont(nullptr, ImGui::GetFontSize() - 5.0f);
    }
    
    // Left side - label text
    char time_buffer[32];
    format_time(timer_secondsM, time_buffer, sizeof(time_buffer));
    ImGui::Text("%s", time_buffer);
    
    // Expand button
    if (focusM != FocusType::Popout) {
        // Right side - action buttons
        ImGui::SameLine(ImGui::GetContentRegionAvail().x - 80 + (defualt_button_size - button_size) * 2);

        const char* icon = ICON_FA_EXPAND;
        if (focusM == FocusType::Fullscreen)
            icon = ICON_FA_COMPRESS;
        if (ImGui::Button(icon, ImVec2(button_size, button_size))) {
            if (focusM == FocusType::None)
                return_val = {
                    {WhatIsFullscreen::Timer},
                    FocusType::Fullscreen ,
                    {idM}
                };
            else if (focusM == FocusType::Fullscreen)
                return_val = {
                    std::nullopt,
                    FocusType::None,
                    std::nullopt,
                };
        }
        
        ImGui::SameLine();
        
        // Picture-In-Picture button
        if (ImGui::Button(ICON_MS_PIP, ImVec2(button_size, button_size)))
            return_val = {
                {WhatIsFullscreen::Timer},
                FocusType::Popout,
                {idM}
            };
    }

    if (button_size < defualt_button_size)
        ImGui::PopFont();
    
    ImGui::EndGroup();

    return return_val;
}

void TimerDisplay::draw_timer_text() {
    char time_buffer[32];
    auto progress_seconds = (start_time_msM != 0 ? calculate_time_progress_ms() / 1000 : 0);
    auto time_to_format = (long)timer_secondsM - (long)progress_seconds;
    format_time(std::max(0l, time_to_format), time_buffer, sizeof(time_buffer));
    
    // Large font for timer display
    ImGui::PushFont(NULL, 30.0f);

    // Calculate center position for text
    ImVec2 window_center = ImGui::GetWindowSize();
    window_center.x *= 0.5f;
    window_center.y *= 0.45f;
    
    // Calculate text size and center it
    ImVec2 text_size = ImGui::CalcTextSize(time_buffer);
    float surface_area_ratio = (text_size.x * text_size.y) / (window_center.x * window_center.y);
    float optimal_font_size {};
    if (surface_area_ratio >= 0.15f)
        optimal_font_size = 32.0f - 36 * surface_area_ratio;
    else
        optimal_font_size = 32.0f + (1 / surface_area_ratio * 0.7f);
    ImGui::PopFont();
    ImGui::PushFont(nullptr, optimal_font_size);
    text_size = ImGui::CalcTextSize(time_buffer);

    ImGui::SetCursorPos(ImVec2(window_center.x - text_size.x * 0.5f, 
                                window_center.y - text_size.y * 0.5f));
    
    auto color = ImVec4(0.263f, 0.49f, 0.525f, 1.0f);
    if (calculate_time_progress_ms() >= timer_secondsM * 1000)
        color.w = std::abs(std::sin(calculate_time_progress_ms() / 500.0));
    ImGui::TextColored(color, "%s", time_buffer);
    
    ImGui::PopFont();
}

void TimerDisplay::draw_control_buttons(AudioPlayer& ap) {
    ImVec2 window_size = ImGui::GetWindowSize();
    float defualt_button_size = 50.0f;
    float button_size = defualt_button_size;
    float spacing = 20.0f;

    if (std::min(window_size.x, window_size.y) < 250.0f)
        button_size = 30.0f;
    
    // Calculate center position for buttons
    float total_width = button_size * 2 + spacing;
    float start_x = (window_size.x - total_width) * 0.5f;
    float button_y = window_size.y - 60.0f + (defualt_button_size - button_size);

    if (button_size < defualt_button_size)
        ImGui::PushFont(nullptr, ImGui::GetFontSize() - 5.0f);
    
    ImGui::SetCursorPos(ImVec2(start_x, button_y));
    
    // Style the play button with blue background
    ImVec4* colors = ImGui::GetStyle().Colors;
    ImVec4 original_button = colors[ImGuiCol_Button];
    ImVec4 original_button_hovered = colors[ImGuiCol_ButtonHovered];
    ImVec4 original_button_active = colors[ImGuiCol_ButtonActive];
    
    // Play button - blue background
    colors[ImGuiCol_Button] = ImVec4(0.26f, 0.52f, 0.96f, 1.0f);  // Blue
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.36f, 0.62f, 1.0f, 1.0f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.16f, 0.42f, 0.86f, 1.0f);
    
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, button_size * 0.5f);  // Make circular
    
    const char* play_text = nullptr;
    if (start_time_msM == 0)
        play_text = ICON_FA_PLAY;
    else if (paused_time_start_msM == 0)
        play_text = ICON_FA_PAUSE;
    else
        play_text = ICON_FA_PLAY;
    if (ImGui::Button(play_text, ImVec2(button_size, button_size))) {
        if (start_time_msM == 0)
            this->start();
        else if (paused_time_start_msM == 0)
            paused_time_start_msM = SDL_GetTicks();
        else if (paused_time_start_msM != 0) {
            paused_time_msM += SDL_GetTicks() - paused_time_start_msM;
            paused_time_start_msM = 0;

            std::println("{} {} {}", paused_time_start_msM, paused_time_msM, start_time_msM);
        }
    }
    
    ImGui::PopStyleVar();
    
    // Restore original colors
    colors[ImGuiCol_Button] = original_button;
    colors[ImGuiCol_ButtonHovered] = original_button_hovered;
    colors[ImGuiCol_ButtonActive] = original_button_active;
    
    ImGui::SameLine(0.0f, spacing);
    
    // Reset button - default styling
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, button_size * 0.5f);  // Make circular
    
    if (ImGui::Button(ICON_MS_RESTORE, ImVec2(button_size, button_size))) {
        reset(ap);
    }
    
    ImGui::PopStyleVar();
    if (button_size < defualt_button_size)
        ImGui::PopFont();
}

void TimerDisplay::start() {
    start_time_msM = SDL_GetTicks();
    std::println("Starting timer: {}", start_time_msM);
}

std::optional<FocusState> TimerDisplay::draw(SDL_Renderer* renderer, AudioPlayer& ap) {
    ImGui::SetNextWindowBgAlpha(0.3);

    std::optional<FocusState> return_val = std::nullopt; 

    ImGui::SetNextWindowSizeConstraints({150.0f, 150.0f}, {FLT_MAX, FLT_MAX});
    
    ImGui::SetNextWindowSize({350.0f, 350.0f}, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos({300, 300}, ImGuiCond_FirstUseEver);

    if (focusM != FocusType::None) {
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::SetNextWindowSizeConstraints({-1, -1}, {-1, -1});
    }

    ImGui::Begin(std::format("Timer Display ##{},{}", idM, (int)focusM).c_str(), nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings);

    // Draw header with label and action buttons
    return_val = draw_header();
    if (return_val.has_value())
        this->focusM = return_val->type;
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    // Draw the timer text in the center
    draw_timer_text();

    // Calculate center for circular progress bar
    ImVec2 window_size = ImGui::GetWindowSize();
    ImVec2 window_pos = ImGui::GetWindowPos();

    float center_x = window_size.x * 0.5f;
    float center_y = window_size.y * 0.45f;
    float radius = 0.4f * std::min(window_size.x, window_size.y - 65);

    float optimal_thickness = 4.0f / 100 * std::min(window_size.x, window_size.y);
    progress_barM.set_thickness(optimal_thickness);

    // Update progress bar position
    progress_barM.set_position(center_x + window_pos.x, center_y + window_pos.y);
    progress_barM.set_radius(radius);

    // Draw the circular progress bar
    update_progress_bar();
    progress_barM.draw(renderer);
    
    // Draw control buttons at the bottom
    draw_control_buttons(ap);

    constexpr unsigned timer_sound_goes_off_ms = 10'500;
    if ((timer_secondsM * 1000) - calculate_time_progress_ms() <= timer_sound_goes_off_ms &&
        !ap.is_playing_or_not() && start_time_msM != 0) {

        ap.play();
        if (timer_secondsM * 1000 < timer_sound_goes_off_ms)
            ap.seek_to(timer_sound_goes_off_ms / 1000.0 - timer_secondsM);
    }

    ImGui::End();

    return return_val;
}

#include "stopwatch_display.hpp"
#include "IconsFontAwesome7.h"
#include "IconsMaterialSymbols.h"
#include "SDL3/SDL_timer.h"
#include "imgui.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <format>
#include <print>

StopwatchDisplay::StopwatchDisplay()
    : start_time_msM(0)
    , paused_time_msM(0)
    , paused_time_start_msM(0)
    , idM(SDL_GetTicks())
    , focusM(FocusType::None)
{
}

void StopwatchDisplay::start() {
    start_time_msM = SDL_GetTicks();
    paused_time_start_msM = 0;
    std::println("Starting stopwatch: {}", start_time_msM);
}

unsigned long StopwatchDisplay::calculate_time_progress_ms() {
    if (start_time_msM == 0)
        return 0;
    
    auto now = SDL_GetTicks();
    auto paused_time_ms = paused_time_msM;
    
    if (paused_time_start_msM != 0)
        paused_time_ms += now - paused_time_start_msM;

    auto progress_ms = (now - start_time_msM) - paused_time_ms;
    return progress_ms;
}

std::optional<FocusState> StopwatchDisplay::draw_header() {
    std::optional<FocusState> return_val = std::nullopt;

    ImGui::BeginGroup();
    float default_button_size = 35.0f;
    float button_size = default_button_size;
    ImVec2 window_size = ImGui::GetWindowSize();

    if (std::min(window_size.x, window_size.y) < 250.0f) {
        button_size = 20.0f;
        ImGui::PushFont(nullptr, ImGui::GetFontSize() - 5.0f);
    }
    
    // Expand button
    if (focusM != FocusType::Popout) {
        // Right side - action buttons
        ImGui::SameLine(ImGui::GetContentRegionAvail().x - 80 + (default_button_size - button_size) * 2);

        const char* icon = ICON_FA_EXPAND;
        if (focusM == FocusType::Fullscreen)
            icon = ICON_FA_COMPRESS;
        
        if (ImGui::Button(icon, ImVec2(button_size, button_size))) {
            if (focusM == FocusType::None)
                return_val = {
                    {WhatIsFullscreen::Stopwatch},
                    FocusType::Fullscreen,
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
                {WhatIsFullscreen::Stopwatch},
                FocusType::Popout,
                {idM}
            };
    }

    if (button_size < default_button_size)
        ImGui::PopFont();
    
    ImGui::EndGroup();

    return return_val;
}

void StopwatchDisplay::draw_stopwatch_text() {
    unsigned long progress_ms = calculate_time_progress_ms();
    
    // Calculate hours, minutes, seconds, and centiseconds
    int total_seconds = progress_ms / 1000;
    int centiseconds = (progress_ms % 1000) / 10;
    int hours = total_seconds / 3600;
    int minutes = (total_seconds % 3600) / 60;
    int seconds = total_seconds % 60;
    
    // Format the time display
    char time_buffer[64];
    snprintf(time_buffer, sizeof(time_buffer), "%02d:%02d:%02d.%02d", 
             hours, minutes, seconds, centiseconds);
    
    // Calculate center position
    ImVec2 window_size = ImGui::GetWindowSize();
    float center_x = window_size.x * 0.5f;
    float center_y = window_size.y * 0.45f;
    
    // Large font for timer display - dynamically sized
    ImGui::PushFont(NULL, 40.0f);
    
    // Calculate text size and optimal font size based on window
    ImVec2 text_size = ImGui::CalcTextSize(time_buffer);
    float surface_area_ratio = (text_size.x * text_size.y) / (center_x * center_y);
    float optimal_font_size {};
    if (surface_area_ratio >= 0.15f)
        optimal_font_size = 40.0f - 14 * surface_area_ratio;
    else
        optimal_font_size = 40.0f + (1 / surface_area_ratio * 0.6f);
    
    ImGui::PopFont();
    ImGui::PushFont(nullptr, optimal_font_size);
    text_size = ImGui::CalcTextSize(time_buffer);
    
    // Center the text
    ImGui::SetCursorPos(ImVec2(center_x - text_size.x * 0.5f, 
                                center_y - text_size.y * 0.5f));
    
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "%s", time_buffer);
    
    ImGui::PopFont();
    
    // Draw labels (hr, min, sec) below the time
    float label_font_size = std::max(10.0f, optimal_font_size * 0.25f);
    ImGui::PushFont(nullptr, label_font_size);
    
    // Calculate positions for labels
    float label_y = center_y + text_size.y * 0.5f + 5.0f;
    
    // Calculate approximate positions for each label
    float char_width = text_size.x / 11.0f; // Approximate width per character
    
    // "hr" label position
    float hr_x = center_x - text_size.x * 0.5f + char_width * 1.0f;
    ImGui::SetCursorPos(ImVec2(hr_x - ImGui::CalcTextSize("hr").x * 0.5f, label_y));
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "hr");
    
    // "min" label position
    float min_x = center_x - text_size.x * 0.5f + char_width * 4.0f;
    ImGui::SetCursorPos(ImVec2(min_x - ImGui::CalcTextSize("min").x * 0.5f, label_y));
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "min");
    
    // "sec" label position
    float sec_x = center_x - text_size.x * 0.5f + char_width * 7.0f;
    ImGui::SetCursorPos(ImVec2(sec_x - ImGui::CalcTextSize("sec").x * 0.5f, label_y));
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "sec");
    
    ImGui::PopFont();
}

void StopwatchDisplay::draw_control_buttons() {
    ImVec2 window_size = ImGui::GetWindowSize();
    float default_button_size = 40.0f;
    float button_size = default_button_size;
    float spacing = 20.0f;

    if (std::min(window_size.x, window_size.y) < 175.0f) {
        button_size = 20.0f;
        spacing = 10.0f;
    } else if (std::min(window_size.x, window_size.y) < 250.0f) {
        button_size = 30.0f;
        spacing = 10.0f;
    }
    
    // Calculate center position for buttons (2 buttons now)
    float total_width = button_size * 2 + spacing;
    float start_x = (window_size.x - total_width) * 0.5f;
    float button_y = window_size.y - 50.0f + (default_button_size - button_size);

    if (button_size < default_button_size)
        ImGui::PushFont(nullptr, ImGui::GetFontSize() - 0.3f * (default_button_size - button_size));
    
    ImGui::SetCursorPos(ImVec2(start_x, button_y));
    
    // Style variables
    ImVec4* colors = ImGui::GetStyle().Colors;
    ImVec4 original_button = colors[ImGuiCol_Button];
    ImVec4 original_button_hovered = colors[ImGuiCol_ButtonHovered];
    ImVec4 original_button_active = colors[ImGuiCol_ButtonActive];
    
    // Play/Pause button - blue background
    colors[ImGuiCol_Button] = ImVec4(0.26f, 0.52f, 0.96f, 1.0f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.36f, 0.62f, 1.0f, 1.0f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.16f, 0.42f, 0.86f, 1.0f);
    
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, button_size * 0.5f);
    
    const char* play_pause_text = nullptr;
    if (start_time_msM == 0)
        play_pause_text = ICON_FA_PLAY;
    else if (paused_time_start_msM == 0)
        play_pause_text = ICON_FA_PAUSE;
    else
        play_pause_text = ICON_FA_PLAY;
    
    if (ImGui::Button(play_pause_text, ImVec2(button_size, button_size))) {
        if (start_time_msM == 0) {
            // Start the stopwatch
            start();
        } else if (paused_time_start_msM == 0) {
            // Pause
            paused_time_start_msM = SDL_GetTicks();
        } else {
            // Resume
            paused_time_msM += SDL_GetTicks() - paused_time_start_msM;
            paused_time_start_msM = 0;
        }
    }
    
    ImGui::PopStyleVar();
    
    // Restore original colors for other buttons
    colors[ImGuiCol_Button] = original_button;
    colors[ImGuiCol_ButtonHovered] = original_button_hovered;
    colors[ImGuiCol_ButtonActive] = original_button_active;
    
    ImGui::SameLine(0.0f, spacing);
    
    // Reset button
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, button_size * 0.5f);
    
    if (ImGui::Button(ICON_MS_RESTORE, ImVec2(button_size, button_size))) {
        // Reset the stopwatch
        start_time_msM = 0;
        paused_time_msM = 0;
        paused_time_start_msM = 0;
        std::println("Stopwatch Reset");
    }
    
    ImGui::PopStyleVar();

    if (button_size < default_button_size)
        ImGui::PopFont();
}

std::optional<FocusState> StopwatchDisplay::draw() {
    ImGui::SetNextWindowBgAlpha(0.3);
    
    std::optional<FocusState> return_val = std::nullopt;
    
    ImGui::SetNextWindowSizeConstraints({150.0f, 150.0f}, {FLT_MAX, FLT_MAX});
    ImGui::SetNextWindowSize({400.0f, 300.0f}, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos({300, 300}, ImGuiCond_FirstUseEver);
    
    if (focusM != FocusType::None) {
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::SetNextWindowSizeConstraints({-1, -1}, {-1, -1});
    }
    
    ImGui::Begin(std::format("Stopwatch Display ##{},{}", idM, (int)focusM).c_str(), 
                 nullptr, 
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings);
    
    // Draw header with expand and PIP buttons
    return_val = draw_header();
    if (return_val.has_value())
        this->focusM = return_val->type;
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    // Draw the stopwatch time display
    draw_stopwatch_text();
    
    // Draw control buttons (play/pause, lap, reset)
    draw_control_buttons();
    
    ImGui::End();
    
    return return_val;
}

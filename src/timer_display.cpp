#include "timer_display.hpp"
#include "IconsFontAwesome7.h"
#include "IconsMaterialSymbols.h"
#include "SDL3/SDL_timer.h"
#include "imgui.h"
#include <cstdio>

TimerDisplay::TimerDisplay() 
    : timer_secondsM(60)
    , start_time_msM(0)
    , progress_barM(0, 0, 100, 12)
{
}

void TimerDisplay::set_timer_value(int seconds) {
    timer_secondsM = seconds;
}

void TimerDisplay::update() {
    auto now = SDL_GetTicks();
    auto progress_secondsM = (now - start_time_msM);

    progress_barM.set_progress((float)progress_secondsM / (timer_secondsM * 1000));
}

void TimerDisplay::reset() {
    start_time_msM = 0;
    progress_barM.reset();
}

void TimerDisplay::format_time(int seconds, char* buffer, size_t buffer_size) {
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    int secs = seconds % 60;
    snprintf(buffer, buffer_size, "%02d:%02d:%02d", hours, minutes, secs);
}

void TimerDisplay::draw_header() {
    ImGui::BeginGroup();
    
    // Left side - label text
    char time_buffer[32];
    format_time(timer_secondsM, time_buffer, sizeof(time_buffer));
    ImGui::Text("%s", time_buffer);
    
    // Right side - action buttons
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 80);
    
    // Expand button
    if (ImGui::Button(ICON_FA_EXPAND, ImVec2(35, 35))) {
        // Expand action
    }
    
    ImGui::SameLine();
    
    // Settings/Close button
    if (ImGui::Button(ICON_MS_PIP, ImVec2(35, 35))) {
        // Close action
    }
    
    ImGui::EndGroup();
}

void TimerDisplay::draw_timer_text() {
    char time_buffer[32];
    auto progress_secondsM = (SDL_GetTicks() - start_time_msM) / 1000;
    format_time(timer_secondsM - progress_secondsM, time_buffer, sizeof(time_buffer));
    
    // Calculate center position for text
    ImVec2 window_center = ImGui::GetWindowSize();
    window_center.x *= 0.5f;
    window_center.y *= 0.5f;
    
    // Large font for timer display
    ImGui::PushFont(NULL, 26.0f);  // You can push a larger font here if available
    
    // Calculate text size and center it
    ImVec2 text_size = ImGui::CalcTextSize(time_buffer);
    ImGui::SetCursorPos(ImVec2(window_center.x - text_size.x * 0.5f, 
                                window_center.y - text_size.y * 0.5f));
    
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "%s", time_buffer);
    
    ImGui::PopFont();
}

void TimerDisplay::draw_control_buttons() {
    ImVec2 window_size = ImGui::GetWindowSize();
    float button_size = 50.0f;
    float spacing = 20.0f;
    
    // Calculate center position for buttons
    float total_width = button_size * 2 + spacing;
    float start_x = (window_size.x - total_width) * 0.5f;
    float button_y = window_size.y - 70.0f;
    
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
    
    const char* play_text = start_time_msM != 0 ? ICON_FA_PAUSE : ICON_FA_PLAY;
    if (ImGui::Button(play_text, ImVec2(button_size, button_size)))
        this->start();
    
    ImGui::PopStyleVar();
    
    // Restore original colors
    colors[ImGuiCol_Button] = original_button;
    colors[ImGuiCol_ButtonHovered] = original_button_hovered;
    colors[ImGuiCol_ButtonActive] = original_button_active;
    
    ImGui::SameLine(0.0f, spacing);
    
    // Reset button - default styling
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, button_size * 0.5f);  // Make circular
    
    if (ImGui::Button(ICON_MS_RESTORE, ImVec2(button_size, button_size))) {
        reset();
    }
    
    ImGui::PopStyleVar();
}

void TimerDisplay::start() {
    start_time_msM = SDL_GetTicks();
}

void TimerDisplay::draw(SDL_Renderer* renderer) {
    ImGui::SetNextWindowBgAlpha(0.3);
    ImGui::Begin("Timer Display");

    // Draw header with label and action buttons
    draw_header();
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    // Draw the timer text in the center
    draw_timer_text();

    // Calculate center for circular progress bar
    ImVec2 window_size = ImGui::GetWindowSize();
    ImVec2 window_pos = ImGui::GetWindowPos();
    float center_x = window_size.x * 0.5f;
    float center_y = window_size.y * 0.45f;  // Slightly above center
    float radius = 120.0f;

    // Update progress bar position
    progress_barM.set_position(center_x + window_pos.x, center_y + window_pos.y);
    progress_barM.set_radius(radius);

    // Draw the circular progress bar
    progress_barM.draw(renderer);
    
    // Draw control buttons at the bottom
    draw_control_buttons();

    ImGui::End();
}

#include "timer_display.hpp"
#include "IconsFontAwesome7.h"
#include "IconsMaterialSymbols.h"
#include "imgui.h"
#include <format>
#include <cstdio>

TimerDisplay::TimerDisplay() 
    : timer_seconds(60)
    , progress(0.0f)
    , label("1 min")
    , is_playing(false)
{
    // Create circular progress bar
    // These values will be adjusted based on window size during draw
    progress_bar = std::make_unique<CircularProgressBar>(0, 0, 100, 12);
    
    // Set colors for the progress bar
    progress_bar->set_background_color(240, 240, 240, 255);
    progress_bar->set_progress_color(66, 133, 244, 255);  // Blue color
}

TimerDisplay::~TimerDisplay() {
}

void TimerDisplay::set_timer_value(int seconds) {
    timer_seconds = seconds;
}

void TimerDisplay::set_progress(float prog) {
    progress = prog;
    progress_bar->set_progress(prog);
}

void TimerDisplay::reset() {
    progress = 0.0f;
    is_playing = false;
    progress_bar->reset();
}

void TimerDisplay::set_label(const char* lbl) {
    label = lbl;
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
    ImGui::Text("%s", label);
    
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
    format_time(timer_seconds, time_buffer, sizeof(time_buffer));
    
    // Calculate center position for text
    ImVec2 window_center = ImGui::GetWindowSize();
    window_center.x *= 0.5f;
    window_center.y *= 0.5f;
    
    // Large font for timer display
    ImGui::PushFont(NULL);  // You can push a larger font here if available
    
    // Calculate text size and center it
    ImVec2 text_size = ImGui::CalcTextSize(time_buffer);
    ImGui::SetCursorPos(ImVec2(window_center.x - text_size.x * 0.5f, 
                                window_center.y - text_size.y * 0.5f));
    
    ImGui::TextColored(ImVec4(0.3f, 0.3f, 0.3f, 1.0f), "%s", time_buffer);
    
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
    
    const char* play_text = is_playing ? ICON_FA_PAUSE : ICON_FA_PLAY;
    if (ImGui::Button(play_text, ImVec2(button_size, button_size))) {
        is_playing = !is_playing;
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
        reset();
    }
    
    ImGui::PopStyleVar();
}

void TimerDisplay::draw(SDL_Renderer* renderer) {
    ImGui::Begin("Timer Display");

    // Draw header with label and action buttons
    draw_header();
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    // Calculate center for circular progress bar
    ImVec2 window_size = ImGui::GetWindowSize();
    ImVec2 window_pos = ImGui::GetWindowPos();
    float center_x = window_size.x * 0.5f;
    float center_y = window_size.y * 0.45f;  // Slightly above center
    float radius = 120.0f;
    
    // Update progress bar position
    progress_bar->set_position(center_x + window_pos.x, center_y + window_pos.y);
    progress_bar->set_radius(radius);
    
    // Draw the timer text in the center
    draw_timer_text();
    
    // Draw control buttons at the bottom
    draw_control_buttons();

    ImGui::End();

    // Draw the circular progress bar
    progress_bar->draw(renderer);
}

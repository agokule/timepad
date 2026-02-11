#ifndef TIMER_DISPLAY_HPP
#define TIMER_DISPLAY_HPP

#include "circular_progress_bar.hpp"
#include <SDL3/SDL.h>
#include "appstate.hpp"
#include "audio_player.hpp"

class TimerDisplay {
public:
    TimerDisplay();
    TimerDisplay(int timer_seconds);

    // Draw the timer UI
    std::optional<FocusState> draw(SDL_Renderer* renderer, AudioPlayer& ap);

    // Set the timer value (total time) in seconds
    void set_timer_value(int seconds);
    
    // update the timer's progress
    void update_progress_bar();

    void start();
    
    // Reset the timer
    void reset();
    
    // Set the label text (e.g., "1 min")
    void set_label(const char* label);

    const CircularProgressBar& get_progress() const { return progress_barM; }
    const unsigned long& get_id() const { return idM; }
    FocusType get_focus_type() const { return focusM; }
    void set_focus_type(FocusType new_type) { focusM = new_type; }

private:
    CircularProgressBar progress_barM;
    int timer_secondsM;
    unsigned long start_time_msM;
    unsigned long paused_time_msM;
    unsigned long paused_time_start_msM;
    unsigned long idM;
    FocusType focusM;
    
    // Helper methods
    std::optional<FocusState> draw_header();
    void draw_timer_text();
    void draw_control_buttons();
    unsigned long calculate_time_progress_ms();
    void format_time(int seconds, char* buffer, size_t buffer_size);
};

#endif // TIMER_DISPLAY_HPP

#ifndef TIMER_DISPLAY_HPP
#define TIMER_DISPLAY_HPP

#include "circular_progress_bar.hpp"
#include <SDL3/SDL.h>

class TimerDisplay {
public:
    TimerDisplay();

    // Draw the timer UI
    void draw(SDL_Renderer* renderer);

    // Set the timer value (total time) in seconds
    void set_timer_value(int seconds);
    
    // update the timer's progress
    void update();

    void start();
    
    // Reset the timer
    void reset();
    
    // Set the label text (e.g., "1 min")
    void set_label(const char* label);

    const CircularProgressBar& get_progress() { return progress_barM; }

private:
    CircularProgressBar progress_barM;
    int timer_secondsM;
    unsigned long long start_time_msM;
    
    // Helper methods
    void draw_header();
    void draw_timer_text();
    void draw_control_buttons();
    void format_time(int seconds, char* buffer, size_t buffer_size);
};

#endif // TIMER_DISPLAY_HPP

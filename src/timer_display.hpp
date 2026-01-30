#ifndef TIMER_DISPLAY_HPP
#define TIMER_DISPLAY_HPP

#include "circular_progress_bar.hpp"
#include <SDL3/SDL.h>
#include <memory>

class TimerDisplay {
public:
    TimerDisplay();
    ~TimerDisplay();

    // Draw the timer UI
    void draw(SDL_Renderer* renderer);

    // Set the timer value in seconds
    void set_timer_value(int seconds);
    
    // Set the progress (0.0 to 1.0)
    void set_progress(float progress);
    
    // Reset the timer
    void reset();
    
    // Set the label text (e.g., "1 min")
    void set_label(const char* label);

private:
    std::unique_ptr<CircularProgressBar> progress_bar;
    int timer_seconds;
    float progress;
    const char* label;
    
    // UI state
    bool is_playing;
    
    // Helper methods
    void draw_header();
    void draw_timer_text();
    void draw_control_buttons();
    void format_time(int seconds, char* buffer, size_t buffer_size);
};

#endif // TIMER_DISPLAY_HPP

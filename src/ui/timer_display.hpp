#ifndef TIMER_DISPLAY_HPP
#define TIMER_DISPLAY_HPP

#include "circular_progress_bar.hpp"
#include <SDL3/SDL.h>
#include "appstate.hpp"
#include "audio_player.hpp"

std::string format_time(int seconds);

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
    void reset(AudioPlayer& ap);
    
    // Set the label text (e.g., "1 min")
    void set_label(std::string label);

    const CircularProgressBar& get_progress() const { return progress_barM; }
    const unsigned long& get_id() const { return idM; }
    FocusType get_focus_type() const { return focusM; }
    void set_focus_type(FocusType new_type) { focusM = new_type; }
    bool is_done() const { return calculate_time_progress_ms() >= timer_secondsM * 1000; }

private:
    CircularProgressBar progress_barM;
    int timer_secondsM;
    unsigned long start_time_msM;
    unsigned long paused_time_msM;
    unsigned long paused_time_start_msM;
    unsigned long idM;
    FocusType focusM;
    std::string titleM;
    
    // Helper methods
    std::optional<FocusState> draw_header();
    void draw_timer_text();
    void draw_control_buttons(AudioPlayer& ap);
    unsigned long calculate_time_progress_ms() const;
};

#endif // TIMER_DISPLAY_HPP

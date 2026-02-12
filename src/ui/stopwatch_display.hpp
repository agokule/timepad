#pragma once
#include <optional>
#include "appstate.hpp"

class StopwatchDisplay {
public:
    StopwatchDisplay();

    // Draw the Stopwatch UI
    std::optional<FocusState> draw();

    void start();

    const unsigned long& get_id() const { return idM; }
    FocusType get_focus_type() const { return focusM; }
    void set_focus_type(FocusType new_type) { focusM = new_type; }
private:
    unsigned long start_time_msM;
    unsigned long paused_time_msM;
    unsigned long paused_time_start_msM;
    unsigned long idM;
    FocusType focusM;

    unsigned long calculate_time_progress_ms();
    std::optional<FocusState> draw_header();
    void draw_stopwatch_text();
    void draw_control_buttons();
};


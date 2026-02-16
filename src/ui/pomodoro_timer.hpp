#pragma once

#include "appstate.hpp"
#include "ui/timer_display.hpp"
#include <format>

enum class PomodoroState {
    Work, Break
};

constexpr const char* format_string {"{}/{} {} session - {}"};

class PomodoroTimer {
public:
    PomodoroTimer(int work_time_s, int break_time_s, int repeat)
         : work_time_sM {work_time_s}, break_time_sM {break_time_s}, repeatM {repeat},
           timerM {work_time_s}, work_times_completedM {}, break_times_completedM {}
    {
        timerM.set_label(std::format(format_string, work_times_completedM+1, repeatM, "work", format_time(work_time_sM)));
    }

    std::optional<FocusState> draw(SDL_Renderer *renderer, AudioPlayer &ap);

    PomodoroState get_current_state() const;
    bool is_done() {
        return work_times_completedM == repeatM && break_times_completedM == repeatM - 1;
    }

    void set_focus_type(FocusType ft) { timerM.set_focus_type(ft); }
    FocusType get_focus_type() const { return timerM.get_focus_type(); }

private:

    int work_time_sM;
    int break_time_sM;
    int repeatM;

    TimerDisplay timerM;

    int work_times_completedM;
    int break_times_completedM;
};


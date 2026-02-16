#include "pomodoro_timer.hpp"
#include "ui/timer_display.hpp"

std::optional<FocusState> PomodoroTimer::draw(SDL_Renderer *renderer, AudioPlayer &ap) {
    if (timerM.is_done()) {
        std::string new_title;
        switch (get_current_state()) {
            case PomodoroState::Break:
                break_times_completedM++;
                timerM.set_timer_value(work_time_sM);
                new_title = std::format(format_string, work_times_completedM+1, repeatM, "work", format_time(work_time_sM));
                break;

            case PomodoroState::Work:
                work_times_completedM++;
                timerM.set_timer_value(break_time_sM);
                new_title = std::format(format_string, break_times_completedM+1, repeatM-1, "break", format_time(break_time_sM));
        }

        timerM.reset(ap);
        timerM.set_label(new_title);
        timerM.start();
    }


    auto focus_state = timerM.draw(renderer, ap);
    if (focus_state.has_value() && focus_state->type != FocusType::None) {
        focus_state->what_is_focused = WhatIsFullscreen::Pomodoro;
        focus_state->id_of_focussed.reset();
    }
    return focus_state;
}

PomodoroState PomodoroTimer::get_current_state() const {
    if (work_times_completedM == break_times_completedM)
        return PomodoroState::Work;
    else
        return PomodoroState::Break;
}


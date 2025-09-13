#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include "timer.h"
#include "config.h"
#include "popup.h"
#include "command_queue.h"

// Global state for the timer
static bool is_paused = false;
static time_t next_break_time = 0;
static bool in_deep_work_session = false;
static time_t session_end_time = 0;

void start_timer(AppConfig config)
{
    time_t ctime = time(NULL);
    struct tm *lt = localtime(&ctime);
    int inter_sec = config.interval_minutes * 60;
    int s_hour, e_hour, s_min, e_min;
    sscanf(config.start_time, "%2d:%2d", &s_hour, &s_min);
    sscanf(config.end_time, "%2d:%2d", &e_hour, &e_min);
    
    // Initialize next break time
    next_break_time = time(NULL) + inter_sec;

    while (true)
    {   
        // Check for commands from controller every 5 seconds
        process_command_queue();
        
        time_t current_time = time(NULL);
        struct tm *lt = localtime(&current_time);
        int c_hour = lt->tm_hour;
        int c_min = lt->tm_min;
        
        int start_min = s_hour * 60 + s_min;
        int end_min = e_hour * 60 + e_min;
        int cur_min = c_hour * 60 + c_min;

        bool active = false;
        
        if (start_min < end_min) {
            active = (cur_min >= start_min && cur_min <= end_min);
        } else {
            active = (cur_min >= start_min || cur_min <= end_min);
        }
        
        // Check if we're in active hours and not paused
        if (active && !is_paused) {
            // Check if deep work session has ended
            if (in_deep_work_session && current_time >= session_end_time) {
                show_popup("Deep work session complete! Great job! 🎉", 5);
                in_deep_work_session = false;
                // Reset normal break timer
                next_break_time = current_time + inter_sec;
            }
            
            // Only show regular breaks if not in deep work session
            if (!in_deep_work_session && current_time >= next_break_time) {
                if (config.eye_care == 0) {
                    show_popup(config.message, 5);
                } else if (config.eye_care == 1) {
                    show_popup("Break Time ദ്ദി( • ᗜ - ) ✧", 3);
                    sleep(3);
                    show_popup("Let's unwind your eyes \n and neck (˶ᵔ ᵕ ᵔ˶)", 3);
                    sleep(3);
                    show_popup("Close your eyes for 5 sec \n and roll them (˶ᵔ ᵕ ᵔ˶)", 6);
                    sleep(6);
                    show_popup("Look at smth far away \n for 20 sec (˶ᵔ ᵕ ᵔ˶)", 21);
                    sleep(21);
                    show_popup("Stretch your neck to the left (˶ᵔ ᵕ ᵔ˶)", 3);
                    sleep(3);
                    show_popup("Now to the right (˶ᵔ ᵕ ᵔ˶)", 3);
                    sleep(3);
                    show_popup("Now look up for 3 sec (˶ᵔ ᵕ ᵔ˶)", 3);
                    sleep(3);
                    show_popup("Now look down for 3 sec (˶ᵔ ᵕ ᵔ˶)", 3);
                    sleep(3);
                    show_popup("Good job! wait for me again!ദ്ദി(˵ •̀ ᴗ - ˵ ) ✧", 2);
                }
                
                // Set next break time
                next_break_time = current_time + inter_sec;
            }
        }
        
        // Sleep for 5 seconds before checking again
        sleep(5);
    }
}

// Command execution functions
void set_deep_work_session(int duration_minutes) {
    time_t current_time = time(NULL);
    session_end_time = current_time + (duration_minutes * 60);
    in_deep_work_session = true;
    
    char message[256];
    snprintf(message, sizeof(message), "Starting %d-minute deep work session! 🎯\nBreaks paused until %s", 
             duration_minutes, ctime(&session_end_time));
    show_popup(message, 5);
}

void toggle_pause_resume() {
    is_paused = !is_paused;
    if (is_paused) {
        show_popup("Restly paused ⏸️\nBreaks disabled until resumed", 3);
    } else {
        show_popup("Restly resumed ▶️\nBreaks re-enabled", 3);
        // Reset break timer when resuming
        time_t current_time = time(NULL);
        next_break_time = current_time + (20 * 60); // Default 20 min interval
    }
}

void reschedule_next_break(int delay_minutes) {
    next_break_time += (delay_minutes * 60);
    char message[128];
    snprintf(message, sizeof(message), "Break rescheduled by %d minutes ⏰", delay_minutes);
    show_popup(message, 3);
}

// Update execute_command implementation
void execute_command(CommandAction* action) {
    switch (action->type) {
        case CMD_SET_SESSION:
            set_deep_work_session(action->params.session.duration_minutes);
            break;
            
        case CMD_TOGGLE_PAUSE:
            toggle_pause_resume();
            break;
            
        case CMD_RESCHEDULE_BREAK:
            reschedule_next_break(action->params.reschedule.delay_minutes);
            break;
            
        case CMD_SUMMARIZE_DAY:
            show_popup("Day summary feature coming soon! 📊", 3);
            // TODO: Call AI summary script
            break;
            
        case CMD_NL_COMMAND:
            // For now, show the command - later we'll parse it with AI
            char message[300];
            snprintf(message, sizeof(message), "Received command: %s\n(AI parsing coming soon!)", 
                     action->params.nl_command.text);
            show_popup(message, 5);
            break;
            
        case CMD_UNKNOWN:
        default:
            break;
    }
}
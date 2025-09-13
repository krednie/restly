#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include "command_queue.h"
#include "config.h"

#define MAX_LINE_LENGTH 1024

static char* get_queue_file_path() {
    static char path[512];
    const char* home = getenv("HOME");
    if (!home) {
        return NULL;
    }
    snprintf(path, sizeof(path), "%s/.config/restly/commands/queue.jsonl", home);
    return path;
}

static void ensure_queue_dir() {
    const char* home = getenv("HOME");
    if (!home) return;
    
    char dir_path[512];
    snprintf(dir_path, sizeof(dir_path), "%s/.config/restly/commands", home);
    
    // Create directory recursively
    char *p = dir_path;
    while (*p) {
        if (*p == '/' && p != dir_path) {
            *p = '\0';
            mkdir(dir_path, 0755);
            *p = '/';
        }
        p++;
    }
    mkdir(dir_path, 0755);
}

CommandAction parse_command_line(const char* line) {
    CommandAction action = {0};
    action.type = CMD_UNKNOWN;
    
    if (!line || strlen(line) == 0) {
        return action;
    }
    
    // Simple JSON parsing for our specific format
    // Look for "action":"set_session"
    if (strstr(line, "\"action\":\"set_session\"")) {
        action.type = CMD_SET_SESSION;
        
        // Extract duration_minutes
        char* duration_str = strstr(line, "\"duration_minutes\":");
        if (duration_str) {
            duration_str += strlen("\"duration_minutes\":");
            action.params.session.duration_minutes = atoi(duration_str);
        } else {
            action.params.session.duration_minutes = 45; // default
        }
        
        // Extract type
        if (strstr(line, "\"type\":\"deep_work\"")) {
            strncpy(action.params.session.type, "deep_work", sizeof(action.params.session.type) - 1);
        } else {
            strncpy(action.params.session.type, "work", sizeof(action.params.session.type) - 1);
        }
    }
    else if (strstr(line, "\"action\":\"toggle_pause\"")) {
        action.type = CMD_TOGGLE_PAUSE;
    }
    else if (strstr(line, "\"action\":\"summarize_day\"")) {
        action.type = CMD_SUMMARIZE_DAY;
    }
    else if (strstr(line, "\"action\":\"reschedule_break\"")) {
        action.type = CMD_RESCHEDULE_BREAK;
        
        // Extract delay_minutes if present
        char* delay_str = strstr(line, "\"delay_minutes\":");
        if (delay_str) {
            delay_str += strlen("\"delay_minutes\":");
            action.params.reschedule.delay_minutes = atoi(delay_str);
        } else {
            action.params.reschedule.delay_minutes = 15; // default
        }
    }
    else if (strstr(line, "\"action\":\"nl_command\"")) {
        action.type = CMD_NL_COMMAND;
        
        // Extract the text field - simple extraction between quotes
        char* text_start = strstr(line, "\"text\":\"");
        if (text_start) {
            text_start += strlen("\"text\":\"");
            char* text_end = strchr(text_start, '"');
            if (text_end && (text_end - text_start) < sizeof(action.params.nl_command.text) - 1) {
                strncpy(action.params.nl_command.text, text_start, text_end - text_start);
                action.params.nl_command.text[text_end - text_start] = '\0';
            }
        }
    }
    
    return action;
}

int process_command_queue() {
    const char* queue_file = get_queue_file_path();
    if (!queue_file) {
        return 0;
    }
    
    FILE* file = fopen(queue_file, "r");
    if (!file) {
        // No queue file yet, that's fine
        return 0;
    }
    
    char line[MAX_LINE_LENGTH];
    int commands_processed = 0;
    
    // Read all commands
    while (fgets(line, sizeof(line), file)) {
        // Remove newline
        line[strcspn(line, "\n")] = 0;
        
        if (strlen(line) == 0) continue;
        
        CommandAction action = parse_command_line(line);
        if (action.type != CMD_UNKNOWN) {
            // Process the command
            execute_command(&action);
            commands_processed++;
        }
    }
    
    fclose(file);
    
    // Clear the queue file after processing
    if (commands_processed > 0) {
        truncate(queue_file, 0);
    }
    
    return commands_processed;
}



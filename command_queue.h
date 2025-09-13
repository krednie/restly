#ifndef COMMAND_QUEUE_H
#define COMMAND_QUEUE_H

typedef enum {
    CMD_UNKNOWN = 0,
    CMD_SET_SESSION,
    CMD_TOGGLE_PAUSE,
    CMD_RESCHEDULE_BREAK,
    CMD_SUMMARIZE_DAY,
    CMD_NL_COMMAND
} CommandType;

typedef struct {
    int duration_minutes;
    char type[32]; // "deep_work", "work", etc.
} SessionParams;

typedef struct {
    int delay_minutes;
} RescheduleParams;

typedef struct {
    char text[256];
} NLCommandParams;

typedef struct {
    CommandType type;
    union {
        SessionParams session;
        RescheduleParams reschedule;
        NLCommandParams nl_command;
    } params;
} CommandAction;

// Function prototypes
int process_command_queue(void);
void execute_command(CommandAction* action);
CommandAction parse_command_line(const char* line);

#endif
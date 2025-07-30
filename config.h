#ifndef CONFIG_H
#define CONFIG_H
#include <stdio.h>

typedef struct {
    int interval_minutes;
    int duration_seconds;
    char *message;
    char start_time[6];
    char end_time[6];
    int eye_care;
}AppConfig;

AppConfig parse_arguments(int argc, char *argv[]);

#endif
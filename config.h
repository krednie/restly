#include <stdio.h>

typedef struct {
    int interval_minutes;
    int duration_minutes;
    char *message[1000];
    char *start_time[6];
    char *end_time[6];
    int eye_care = 0;
}AppConfig;

AppConfig parse_arguements(int argc, char *argv[]);

#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "daemon.h"

AppConfig parse_arguments(int argc, char *argv[])
{
    AppConfig config = {
        .interval_minutes = 20,
        .duration_seconds = 20,
        .message = NULL,
        .eye_care = 1,
        .start_time = "00:00",
        .end_time = "23:59"
    };

      config.message = malloc(strlen("Time to rest your eyes!") + 1);
    if (config.message != NULL)
    {
        strcpy(config.message, "Time to rest your eyes!");
    }

    for (int i = 1; i < argc; i++) 
    {
        if ((strcmp(argv[i], "--interval") == 0 || strcmp(argv[i], "-i") == 0) && i + 1 < argc)

        {
            config.interval_minutes = atof(argv[++i]);
        }
        else if ((strcmp(argv[i], "--duration") == 0 || strcmp(argv[i], "-d") == 0) && i + 1 < argc)
        {
            config.duration_seconds = atof(argv[++i]);
        }
        else if ((strcmp(argv[i], "--message") == 0 || strcmp(argv[i], "-m") == 0) && i + 1 < argc)
        {
           free (config.message);
           char *msg_arg = argv[++i];
            config.message = malloc(strlen(msg_arg) + 1);
            if (config.message != NULL)
            {
                strcpy(config.message, msg_arg); 
            }
        }
        else if (strcmp(argv[i], "--active-hours") == 0 && i + 1 < argc)
        {
            sscanf(argv[++i], "%5[^-]-%5s", config.start_time, config.end_time);
        }
        else if ((strcmp(argv[i], "--eyecare") == 0 || strcmp(argv[i], "-e") == 0) && i+1 <argc)
        {
            config.eye_care = atoi(argv[++i]);
        }
        else if ((strcmp(argv[i], "--stop") == 0))
        {
            stopdaemon();
            exit(EXIT_SUCCESS);
        }

    }
    return config;
}


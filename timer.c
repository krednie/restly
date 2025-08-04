#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include "timer.h"
#include "config.h"
#include "popup.h"

void start_timer(AppConfig config)
{
    time_t ctime = time(NULL);
    int inter_sec = config.interval_minutes * 60;
    int s_hour, e_hour, s_min, e_min;
    sscanf( config.start_time, "%2d:%2d", &s_hour, &s_min );
    sscanf( config.end_time, "%2d:%2d", &e_hour, &e_min );

    
    while (true)
    {
            sleep(inter_sec);

        if (config.eye_care == 0)
        {
        show_popup(config.message, 5);
        }

        else if (config.eye_care == 1)

        {
            show_popup("Break Time ദ്ദി( • ᴗ - ) ✧", 3);
            sleep(3);
            show_popup("Let’s unwind your eyes \n and neck (˶ᵔ ᵕ ᵔ˶)", 3);
            sleep(3);
            show_popup("Close your eyes for 5 sec \n and roll them (˶ᵔ ᵕ ᵔ˶)", 6);
            sleep(6);
            show_popup("Look at smth far away \n for 20 sec (˶ᵔ ᵕ ᵔ˶)", 21);
            sleep(21);
            show_popup("Stretch your neck to the left (˶ᵔ ᵕ ᵔ˶)" , 3);
            sleep(3);
            show_popup("Now to the right (˶ᵔ ᵕ ᵔ˶)", 3);
            sleep(3);
            show_popup("Now look up for 3 sec (˶ᵔ ᵕ ᵔ˶)" , 3);
            sleep(3);
            show_popup("Now look down for 3 sec (˶ᵔ ᵕ ᵔ˶)" , 3);
            sleep(3);
            show_popup("Good job! wait for me again!ദ്ദി(˵ •̀ ᴗ - ˵ ) ✧", 2);
            
        }

    }
}

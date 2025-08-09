#include <stdio.h>
#include "timer.h"
#include "daemon.h"
#include "config.h"


int main(int argc, char *argv[])


{
    AppConfig config = parse_arguments(argc, argv);
    daemonize();

 start_timer(config);
}
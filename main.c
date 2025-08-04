#include <stdio.h>
#include "timer.h"
#include "config.h"

// int main(int argc, char *argv[])
// {
// send_notif("yash", "dsfsdfsd");
// return 0;
// }

int main( int argc, char *argv[])
{
 AppConfig config = parse_arguments(argc, argv);

 start_timer(config);
}
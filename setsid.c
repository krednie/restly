#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("Before setsid: SID = %d\n", getsid(0));

    pid_t pid = fork();

    if (pid > 0)
    {
        exit(0);
    }
    if (setsid() < 0) {
        perror("setsid failed");
        return 1;
    }

    printf("After setsid: SID = %d\n", getsid(0));
    pause(); // wait forever
}

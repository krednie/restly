#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include "daemon.h"
#define PIDFILE "/tmp/restlychild.pid"


void daemonize()
{
    pid_t pid = fork();

    
    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid > 0)
    {
        exit(EXIT_SUCCESS);
    }
     
    if (setsid() < 0)
     {
        perror("setsid failed");
        exit(EXIT_FAILURE);
     }
        fclose(stdin);
        fclose(stdout);
        fclose(stderr);

        FILE *pidfile = fopen("/tmp/restlychild.pid", "w");
        if (pidfile)
        {
            fprintf(pidfile, "%d\n", getpid());
            fclose(pidfile);
        }
}

void stopdaemon()
{
    const char *pidfilepath = "/tmp/restlychild.pid";
    FILE *pidfile = fopen(pidfilepath, "r");

    if (!pidfile)
    {
        perror("cant find pidfile!\n");
        return;
    }

    int pid;
    if (fscanf(pidfile, "%d", &pid) != 1)
    {
        fprintf(stderr, "failed to read pidfile!\n");
        fclose(pidfile);
        return;
    }
    fclose(pidfile);

    if (kill(pid, SIGTERM) == 0)
    {
        printf("daemon pid: %d stopped", pid);
        remove(pidfilepath);
    }
    else
    {
        perror("failed to stop daemon. please restart your pc to end it");
    }


}
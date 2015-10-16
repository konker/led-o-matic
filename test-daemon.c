#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <pthread.h>

#define LOG_FILE "led-o-maticd.log"
#define PID_FILE "led-o-maticd.pid"


int main() {
    // Process id and session id for the daemon
    pid_t pid, sid;

    // Log file and PID file output streams
    FILE *logfp, *pidfp;

    // ----------------------------------------------------------------------
    // Fork a process
    pid = fork();
    if (pid < 0) {
        fprintf(stderr, "Could not fork: %d\n", pid);
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        fprintf(stderr, "Forked PID: %d\n", pid);

        // ----------------------------------------------------------------------
        // Create PID file
        pidfp = fopen(PID_FILE, "w");
        if (pidfp == NULL) {
            fprintf(stderr, "Could not open PID file: %s\n", PID_FILE);
            exit(EXIT_FAILURE);
        }

        // Write the PID to the PID file
        fprintf(pidfp, "%d", pid);
        fclose(pidfp);

        // Exit the parent process
        exit(EXIT_SUCCESS);
    }

    // ----------------------------------------------------------------------
    // Change the file mask mode
    umask(0);

    // ----------------------------------------------------------------------
    // Initialize log
    logfp = fopen(LOG_FILE, "a");
    if (logfp == 0) {
        fprintf(stderr, "Could not open log file: %s\n", LOG_FILE);
        exit(EXIT_FAILURE);
    }
    fprintf(logfp, "----------------------------------------------\n");

    // ----------------------------------------------------------------------
    // Create a new SID for the child process
    sid = setsid();
    if (sid < 0) {
        fprintf(logfp, "Could not set SID: %d\n", sid);
        fclose(logfp);
        exit(EXIT_FAILURE);
    }
    fprintf(logfp, "Set SID: %d\n", sid);

    // ----------------------------------------------------------------------
    // Change the current working directory to /
    if ((chdir("/")) < 0) {
        fprintf(logfp, "Could not change CWD\n");
        fclose(logfp);
        exit(EXIT_FAILURE);
    }

    // ----------------------------------------------------------------------
    // Close standard file descriptiors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // ----------------------------------------------------------------------
    // Main loop
    int i = 0;
    while (1) {
        fprintf(logfp, "TESTD\n");
        fflush(logfp);
        if (++i >= 3) {
            fprintf(logfp, "Goodbye\n");

            // ----------------------------------------------------------------------
            //[TODO: how to tidy up at exit?]
            fclose(logfp);
            exit(EXIT_SUCCESS);
        }
        sleep(1);
    }
}


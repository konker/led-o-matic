#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <syslog.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <klm_matrix.h>
#include <klm_segment.h>
#include "led-o-maticd.h"
#include "led-o-maticd_args.h"
#include "led-o-maticd_config.h"
#include "led-o-maticd_udp.h"
#include "led-o-maticd_matrix.h"


// Global so that it's visible to the signal handler
static ledomaticd lomd;

/**
  // ----------------------------------------------------------------------
  Signal handler
*/
static void signal_handler(int signum) {
    LEDOMATIC_LOG(lomd, "Got signal: %d. Exiting..\n", signum);
    lomd.running = false;
}

/**
  // ----------------------------------------------------------------------
  [TODO: comment]
*/
int main(int argc, char **argv) {
    // ----------------------------------------------------------------------
    // Initialize ledomaticd structure
    lomd.running = true;
    lomd.micros_0 = 0;
    lomd.micros_1 = 0;

    // ----------------------------------------------------------------------
    // Parse command line arguments
    lomd.args.config_file = LEDOMATIC_DEFAULT_CONFIG_FILE;
    lomd.args.log_file = LEDOMATIC_DEFAULT_LOG_FILE;
    lomd.args.pid_file = LEDOMATIC_DEFAULT_PID_FILE;
    ledomatic_args_parse(argc, argv, &lomd);


    // ----------------------------------------------------------------------
    // Initialize log
    lomd.logfp = fopen(lomd.args.log_file, "a");
    if (lomd.logfp == 0) {
        fprintf(stderr, "Could not open log file: %s\n", lomd.args.log_file);
        exit(EXIT_FAILURE);
    }
    LEDOMATIC_LOG(lomd, "----------------------------------------------\n");
    LEDOMATIC_LOG(lomd, "Log file: %s\n", lomd.args.log_file);
    LEDOMATIC_LOG(lomd, "PID file: %s\n", lomd.args.pid_file);
    LEDOMATIC_LOG(lomd, "Config file: %s\n", lomd.args.config_file);

    // ----------------------------------------------------------------------
    // Parse the config file
    if (!ledomatic_config_parse(&lomd)) {
        LEDOMATIC_LOG(lomd, "Can't load config file %s. Aborting\n", lomd.args.config_file);
        fclose(lomd.logfp);
        exit(EXIT_FAILURE);
    }
    LEDOMATIC_LOG(lomd, "Config loaded from '%s': driver=%s, matrix_width=%d, matrix_height=%d\n",
                  lomd.args.config_file, lomd.config.driver, lomd.config.matrix_width, lomd.config.matrix_height);

#ifndef LEDOMATIC_NO_FORK
    // ----------------------------------------------------------------------
    // Process id and session id for the daemon
    pid_t pid, sid;

    // Log file and PID file output streams
    FILE *pidfp;

    // ----------------------------------------------------------------------
    // Fork a process
    pid = fork();
    if (pid < 0) {
        LEDOMATIC_LOG(lomd, "Could not fork: %d\n", pid);
        fclose(lomd.logfp);
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        LEDOMATIC_LOG(lomd, "Forked PID: %d\n", pid);

        // ----------------------------------------------------------------------
        // Create PID file
        pidfp = fopen(lomd.args.pid_file, "w");
        if (pidfp == NULL) {
            LEDOMATIC_LOG(lomd, "Could not open PID file: %s\n", lomd.args.pid_file);
            fclose(lomd.logfp);
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
    // Create a new SID for the child process
    sid = setsid();
    if (sid < 0) {
        LEDOMATIC_LOG(lomd, "Could not set SID: %d\n", sid);
        fclose(lomd.logfp);
        exit(EXIT_FAILURE);
    }
    LEDOMATIC_LOG(lomd, "Set SID: %d\n", sid);

    // ----------------------------------------------------------------------
    // Change the current working directory to /
    if ((chdir("/")) < 0) {
        LEDOMATIC_LOG(lomd, "Could not change CWD\n");
        fclose(lomd.logfp);
        exit(EXIT_FAILURE);
    }

    // ----------------------------------------------------------------------
    // Close standard file descriptiors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
#endif

    // ----------------------------------------------------------------------
    // Signal handler
    signal(SIGINT, signal_handler);

    // ----------------------------------------------------------------------
    // Initialize matrix system
    ledomatic_matrix_init(&lomd);

    // ----------------------------------------------------------------------
    // UDP listener thread
    pthread_t ledomatic_udp_listener_tid;
    int rc1 = pthread_create(
                    &ledomatic_udp_listener_tid,
                    NULL,
                    ledomatic_udp_listener_thread,
                    &lomd);
    if (rc1) {
        LEDOMATIC_LOG(lomd, "ERROR: in create of UDP listener thread: %d\n", rc1);
        fclose(lomd.logfp);
        exit(EXIT_FAILURE);
    }

    // ----------------------------------------------------------------------
    // The main loop
    while (lomd.running) {
        // Delay at the beginning of each complete scan to make loop time consistent
        if (lomd.matrix->scan_row == 0) {
            usleep(LEDOMATIC_TICK_PERIOD_MICROS);
        }

        klm_mat_scan(lomd.matrix);
    }

    // Main loop now finished...

    // ----------------------------------------------------------------------
    // Stop the UDP listening thread
    pthread_join(ledomatic_udp_listener_tid, NULL);

    // ----------------------------------------------------------------------
    // Clean up and exit
    ledomatic_matrix_exit(&lomd);

#ifndef LEDOMATIC_NO_FORK
    if (unlink(lomd.args.pid_file) == -1) {
        LEDOMATIC_LOG(lomd, "%s\n", strerror(errno));
    }
#endif

    LEDOMATIC_LOG(lomd, "Main exit\n");
    fclose(lomd.logfp);
    return EXIT_SUCCESS;
}


#include <iostream>
#include <fstream>

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
    std::ofstream logfs, pidfs;

    // ----------------------------------------------------------------------
    // Fork a process
    pid = fork();
    if (pid < 0) {
        std::clog << "Could not fork: " << pid << std::endl;
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        std::clog << "Forked PID: " << pid << std::endl;
        pidfs.open(PID_FILE, std::ios::trunc);
        if (!pidfs.is_open()) {
            std::cerr << "Could not open PID file: " << PID_FILE << std::endl;
            exit(EXIT_FAILURE);
        }

        // Write the PID to the PID file
        pidfs << pid;
        pidfs.close();

        // Exit the parent process
        exit(EXIT_SUCCESS);
    }

    // ----------------------------------------------------------------------
    // Change the file mask mode
    umask(0);

    // ----------------------------------------------------------------------
    // Initialize log
    logfs.open(LOG_FILE, std::ios::app);
    if (!logfs.is_open()) {
        std::cerr << "Could not open log file: " << LOG_FILE << std::endl;
        exit(EXIT_FAILURE);
    }
    // Set std::clog to write to the log file
    std::clog.rdbuf(logfs.rdbuf());
    std::clog << "----------------------------------------------" << std::endl;
    std::clog << "Log file initialized: " << LOG_FILE << std::endl;

    // ----------------------------------------------------------------------
    // Create a new SID for the child process
    sid = setsid();
    if (sid < 0) {
        //[TODO: log]
        std::clog << "Could not set SID: " << sid << std::endl;
        logfs.close();
        exit(EXIT_FAILURE);
    }
    std::clog << "Set SID: " << sid << std::endl;

    // ----------------------------------------------------------------------
    // Change the current working directory to /
    if ((chdir("/")) < 0) {
        std::clog << "Could not change CWD" << std::endl;
        logfs.close();
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
        std::clog << "TESTD" << std::endl;
        if (++i >= 3) {
            std::clog << "Goodbye" << std::endl;

            // ----------------------------------------------------------------------
            //[TODO: how to tidy up at exit?]
            logfs.close();
            exit(EXIT_SUCCESS);
        }
        sleep(1);
    }

    // ----------------------------------------------------------------------
    //[TODO: how to tidy up at exit?]
    logfs.close();
    exit(EXIT_SUCCESS);
}


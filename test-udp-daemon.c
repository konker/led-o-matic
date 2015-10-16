#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define LOG_FILE "led-o-maticd.log"
#define PID_FILE "led-o-maticd.pid"

#define UDP_PORT "49501"
#define MAXBUFLEN 100

/**
  Thanks to Beej
  http://beej.us/guide/bgnet/examples/listener.c
*/

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

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
    // UDP listener
    fprintf(logfp, "Starting UDP socket listener...\n");
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    char buf[MAXBUFLEN];
    socklen_t addr_len;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG; // use my IP

    if ((rv = getaddrinfo(NULL, UDP_PORT, &hints, &servinfo)) != 0) {
        fprintf(logfp, "Error: getaddrinfo: %s\n", gai_strerror(rv));
        fclose(logfp);
        exit(EXIT_FAILURE);
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1) {
            fprintf(logfp, "%s\n", strerror(errno));
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            fprintf(logfp, "%s\n", strerror(errno));
            continue;
        }

        // Success, we have bound to a port
        break;
    }

    if (p == NULL) {
        fprintf(logfp, "Failed to bind to socket. Exiting\n");
        fclose(logfp);
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(servinfo);

    // ----------------------------------------------------------------------
    // Main loop
    while (1) {
        addr_len = sizeof their_addr;
        printf("Listener: waiting to recvfrom...\n");
        numbytes = recvfrom(sockfd,
                            buf,
                            MAXBUFLEN-1,
                            0,
                            (struct sockaddr *)&their_addr,
                            &addr_len);
        if (numbytes == -1) {
            fprintf(logfp, "Listener: error in recvfrom\n");
            fclose(logfp);
            exit(EXIT_FAILURE);
        }

        printf("Listener: got packet from %s\n",
            inet_ntop(their_addr.ss_family,
                get_in_addr((struct sockaddr *)&their_addr),
                s, sizeof s));
        printf("Listener: packet is %d bytes long\n", numbytes);
        buf[numbytes] = '\0';
        fprintf(logfp, "Listener: packet contains \"%s\"\n", buf);
        fflush(logfp);

        /*
        close(sockfd);
        fclose(logfp);
        exit(EXIT_SUCCESS);
        */

        /*
    int i = 0;
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
        */
    }
}


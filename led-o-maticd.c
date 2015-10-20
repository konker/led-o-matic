#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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
#include <pthread.h>
#include <signal.h>

#include <kulm.h>

#define LEDOMATIC_LOG_FILE "/var/log/led-o-maticd.log"
#define LEDOMATIC_PID_FILE "/var/run/led-o-maticd.pid"

#define LEDOMATIC_UDP_PORT "7890"
#define LEDOMATIC_MAXBUFLEN 100

#define LEDOMATIC_MATRIX_WIDTH 64
#define LEDOMATIC_MATRIX_HEIGHT 16
#define LEDOMATIC_MATRIX_ROW_WIDTH (LEDOMATIC_MATRIX_WIDTH / 8)

// Clean up and exit daemon
#define LEDOMATIC_CMD_EXIT "exit"
// Stop all scrolling
#define LEDOMATIC_CMD_STOP "stop"
// Resume all scrolling
#define LEDOMATIC_CMD_START "start"
// Switch display off
#define LEDOMATIC_CMD_OFF "off"
// Switch display on
#define LEDOMATIC_CMD_ON "on"
// Reverse display colors
#define LEDOMATIC_CMD_REVERSE "reverse"
// Clear the display
#define LEDOMATIC_CMD_CLEAR "clear"
// Set text1
#define LEDOMATIC_CMD_TEXT1 "text1:%[^\t\n]"
// Set text1 scroll speed
#define LEDOMATIC_CMD_SPEED1 "speed1:%f"
// Set text2
#define LEDOMATIC_CMD_TEXT2 "text2:%[^\t\n]"
// Set text2 scroll speed
#define LEDOMATIC_CMD_SPEED2 "speed2:%f"

#define LEDOMATIC_LOG(...) fprintf(ledomatic_logfp, __VA_ARGS__); \
                           fflush(ledomatic_logfp);

static int ledomatic_sockfd;
static FILE *ledomatic_logfp;
volatile static bool ledomatic_running;

static uint8_t ledomatic_display_buffer[
                    LEDOMATIC_MATRIX_HEIGHT *
                    LEDOMATIC_MATRIX_ROW_WIDTH];
static kulm_matrix *ledomatic_matrix;


/**
  // ----------------------------------------------------------------------
  Signal handler
  [TODO: comment]
*/
static void signal_handler(int signum) {
    LEDOMATIC_LOG("Got signal: %d. Exiting..\n", signum);
    ledomatic_running = false;
}

/**
  // ----------------------------------------------------------------------
  Main matrix scan thread function
  [TODO: comment]
*/
static void *matrix_scanner() {
    LEDOMATIC_LOG("Matrix scanner: starting\n");
    while (ledomatic_running) {
        kulm_scan(ledomatic_matrix);
    }
    LEDOMATIC_LOG("Matrix scanner: exiting\n");
    pthread_exit(NULL);
    return NULL;
}

/**
  // ----------------------------------------------------------------------
  Handle commands received via UDP socket
  [TODO: comment]
*/
static void handle_command(char *buf) {
    float num;
    char str[KULM_TEXT1_LEN];
    if (strcasecmp(buf, LEDOMATIC_CMD_EXIT) == 0) {
        LEDOMATIC_LOG("UDP listener: [exit]\n");
        ledomatic_running = false;
    }
    else if (strcasecmp(buf, LEDOMATIC_CMD_STOP) == 0) {
        LEDOMATIC_LOG("UDP listener: [stop]\n");
        kulm_stop(ledomatic_matrix);
    }
    else if (strcasecmp(buf, LEDOMATIC_CMD_START) == 0) {
        LEDOMATIC_LOG("UDP listener: [start]\n");
        kulm_start(ledomatic_matrix);
    }
    else if (strcasecmp(buf, LEDOMATIC_CMD_OFF) == 0) {
        LEDOMATIC_LOG("UDP listener: [off]\n");
        kulm_off(ledomatic_matrix);
    }
    else if (strcasecmp(buf, LEDOMATIC_CMD_ON) == 0) {
        LEDOMATIC_LOG("UDP listener: [on]\n");
        kulm_on(ledomatic_matrix);
    }
    else if (strcasecmp(buf, LEDOMATIC_CMD_REVERSE) == 0) {
        LEDOMATIC_LOG("UDP listener: [reverse]\n");
        kulm_reverse(ledomatic_matrix);
    }
    else if (strcasecmp(buf, LEDOMATIC_CMD_CLEAR) == 0) {
        LEDOMATIC_LOG("UDP listener: [clear]\n");
        kulm_clear(ledomatic_matrix);
    }
    else if (sscanf(buf, LEDOMATIC_CMD_TEXT1, str) == 1) {
        LEDOMATIC_LOG("UDP listener: [text1 => %s]\n", str);
        kulm_set_text1(ledomatic_matrix, str);
    }
    else if (sscanf(buf, LEDOMATIC_CMD_SPEED1, &num) == 1) {
        LEDOMATIC_LOG("UDP listener: [speed1 => %f]\n", num);
        kulm_set_text1_speed(ledomatic_matrix, num);
    }
    else if (sscanf(buf, LEDOMATIC_CMD_TEXT2, str) == 1) {
        LEDOMATIC_LOG("UDP listener: [text2 => %s]\n", str);
        kulm_set_text2(ledomatic_matrix, str);
    }
    else if (sscanf(buf, LEDOMATIC_CMD_SPEED2, &num) == 1) {
        LEDOMATIC_LOG("UDP listener: [speed2 => %f]\n", num);
        kulm_set_text2_speed(ledomatic_matrix, num);
    }
    else {
        LEDOMATIC_LOG("UDP listener: [unknown]\n");
    }
}

/**
  // ----------------------------------------------------------------------
  UDP listener
  http://beej.us/guide/bgnet/examples/listener.c
  [TODO: comment]
*/
static void * udp_listener() {
    LEDOMATIC_LOG("UDP listener: starting...\n");
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    char buf[LEDOMATIC_MAXBUFLEN];
    socklen_t addr_len;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG; // use my IP

    if ((rv = getaddrinfo(NULL, LEDOMATIC_UDP_PORT, &hints, &servinfo)) != 0) {
        LEDOMATIC_LOG("UDP listener: Error: getaddrinfo: %s\n", gai_strerror(rv));
        fclose(ledomatic_logfp);
        exit(EXIT_FAILURE);
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        ledomatic_sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (ledomatic_sockfd == -1) {
            LEDOMATIC_LOG("%s\n", strerror(errno));
            continue;
        }

        if (bind(ledomatic_sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(ledomatic_sockfd);
            LEDOMATIC_LOG("%s\n", strerror(errno));
            continue;
        }

        // Success, we have bound to a port
        break;
    }

    if (p == NULL) {
        LEDOMATIC_LOG("UDP listener: Failed to bind to socket. Exiting\n");
        fclose(ledomatic_logfp);
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(servinfo);

    // Listen loop
    while (ledomatic_running) {
        addr_len = sizeof their_addr;
        printf("UDP listener: waiting to recvfrom...\n");
        numbytes = recvfrom(ledomatic_sockfd,
                            buf,
                            LEDOMATIC_MAXBUFLEN-1,
                            0,
                            (struct sockaddr *)&their_addr,
                            &addr_len);
        if (numbytes == -1) {
            LEDOMATIC_LOG("UDP listener: error in recvfrom\n");
            fclose(ledomatic_logfp);
            pthread_exit(NULL);
        }

        if (ledomatic_running) {
            buf[numbytes-1] = '\0';
            //LEDOMATIC_LOG("UDP listener: received: \"%s\"\n", buf);
            handle_command(buf);
        }
    }

    LEDOMATIC_LOG("UDP listener: exiting\n");
    pthread_exit(NULL);
    return NULL;
}

/**
  // ----------------------------------------------------------------------
  // Main loop procedure
  [TODO: comment]
*/
static void main_loop() {
    struct timespec delay_t;
    delay_t.tv_sec = 0;
    delay_t.tv_nsec = 100000; // 100ms

    while (ledomatic_running) {
        kulm_tick(ledomatic_matrix);
        nanosleep(&delay_t, NULL);
    }
}

int main() {
    // Process id and session id for the daemon
    pid_t pid, sid;

    // Log file and PID file output streams
    FILE *pidfp;

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
        pidfp = fopen(LEDOMATIC_PID_FILE, "w");
        if (pidfp == NULL) {
            fprintf(stderr, "Could not open PID file: %s\n", LEDOMATIC_PID_FILE);
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
    ledomatic_logfp = fopen(LEDOMATIC_LOG_FILE, "a");
    if (ledomatic_logfp == 0) {
        fprintf(stderr, "Could not open log file: %s\n", LEDOMATIC_LOG_FILE);
        exit(EXIT_FAILURE);
    }
    LEDOMATIC_LOG("----------------------------------------------\n");

    // ----------------------------------------------------------------------
    // Create a new SID for the child process
    sid = setsid();
    if (sid < 0) {
        LEDOMATIC_LOG("Could not set SID: %d\n", sid);
        fclose(ledomatic_logfp);
        exit(EXIT_FAILURE);
    }
    LEDOMATIC_LOG("Set SID: %d\n", sid);

    // ----------------------------------------------------------------------
    // Change the current working directory to /
    if ((chdir("/")) < 0) {
        LEDOMATIC_LOG("Could not change CWD\n");
        fclose(ledomatic_logfp);
        exit(EXIT_FAILURE);
    }

    // ----------------------------------------------------------------------
    // Close standard file descriptiors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // ----------------------------------------------------------------------
    // Initialize matrix
    kulm_begin();

    ledomatic_matrix = kulm_create(
                            ledomatic_display_buffer,
                            LEDOMATIC_MATRIX_WIDTH,
                            LEDOMATIC_MATRIX_HEIGHT);
    kulm_init(ledomatic_matrix, 0, 2, 3, 12, 13, 14, 21, 22);
    ledomatic_running = true;

    // ----------------------------------------------------------------------
    // Signal handler
    signal(SIGINT, signal_handler);

    // ----------------------------------------------------------------------
    // UDP listener thread
    pthread_t udp_listener_tid;
    int rc1 = pthread_create(&udp_listener_tid, NULL, udp_listener, NULL);
    if (rc1) {
        LEDOMATIC_LOG("ERROR: in create of UDP listener thread: %d\n", rc1);
        fclose(ledomatic_logfp);
        exit(EXIT_FAILURE);
    }

    // ----------------------------------------------------------------------
    // Matrix scanner thread
    pthread_t matrix_scanner_tid;
    int rc2 = pthread_create(&matrix_scanner_tid, NULL, matrix_scanner, NULL);
    if (rc2) {
        LEDOMATIC_LOG("ERROR: in create of Matrix scanner thread: %d\n", rc2);
        fclose(ledomatic_logfp);
        exit(EXIT_FAILURE);
    }

    // ----------------------------------------------------------------------
    // Call the main loop
    main_loop();

    // ----------------------------------------------------------------------
    // Stop the UDP listening thread
    shutdown(ledomatic_sockfd, SHUT_RDWR);
    pthread_join(udp_listener_tid, NULL);

    // ----------------------------------------------------------------------
    // Stop the Matrix scanner thread
    pthread_join(matrix_scanner_tid, NULL);

    // ----------------------------------------------------------------------
    // Clean up and exit
    kulm_destroy(ledomatic_matrix);
    if (unlink(LEDOMATIC_PID_FILE) == -1) {
        LEDOMATIC_LOG("%s\n", strerror(errno));
    }

    LEDOMATIC_LOG("Main exit\n");
    fclose(ledomatic_logfp);
    pthread_exit(NULL);
    return EXIT_SUCCESS;
}


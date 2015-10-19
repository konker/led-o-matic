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

#include <wiringPi.h>
#include <UltrathinLEDMatrix.h>

#define LOG_FILE "/var/log/led-o-maticd.log"
#define PID_FILE "/var/run/led-o-maticd.pid"

#define UDP_PORT "7890"
#define MAXBUFLEN 100

#define MATRIX_WIDTH 64
#define MATRIX_HEIGHT 16
#define MATRIX_ROW_WIDTH (MATRIX_WIDTH / 8)

// Clean up and exit daemon
#define CMD_EXIT "exit"
// Stop all scrolling
#define CMD_STOP "stop"
// Resume all scrolling
#define CMD_START "start"
// Switch display off
#define CMD_OFF "off"
// Switch display on
#define CMD_ON "on"
// Reverse display colors
#define CMD_REVERSE "reverse"
// Clear the display
#define CMD_CLEAR "clear"
// Set text1
#define CMD_TEXT1 "text1:%[^\t\n]"
// Set text1 scroll speed
#define CMD_SPEED1 "speed1:%f"
// Set text2
#define CMD_TEXT2 "text2:%[^\t\n]"
// Set text2 scroll speed
#define CMD_SPEED2 "speed2:%f"

#define LOG(...) fprintf(g_logfp, __VA_ARGS__); \
                 fflush(g_logfp);

int g_sockfd;
static FILE *g_logfp;
volatile static bool g_running;

uint8_t g_display_buffer[MATRIX_HEIGHT * MATRIX_ROW_WIDTH];
struct UltrathinLEDMatrix * g_matrix;


/**
  // ----------------------------------------------------------------------
  Signal handler
  [TODO: comment]
*/
static void signal_handler(int signum) {
    LOG("Got signal: %d. Exiting..\n", signum);
    g_running = false;
}

/**
  // ----------------------------------------------------------------------
  Main matrix scan thread function
  [TODO: comment]
*/
static void *matrix_scanner() {
    LOG("Matrix scanner: starting\n");
    while (g_running) {
        ultrathin_matrix_scan(g_matrix);
    }
    LOG("Matrix scanner: exiting\n");
    pthread_exit(NULL);
    return NULL;
}

/**
  // ----------------------------------------------------------------------
  Handle commands received via UDP socket
  [TODO: comment]
*/
void handle_command(char *buf) {
    float num;
    char str[TEXT1_LEN];
    if (strcasecmp(buf, CMD_EXIT) == 0) {
        LOG("UDP listener: [exit]\n");
        g_running = false;
    }
    else if (strcasecmp(buf, CMD_STOP) == 0) {
        LOG("UDP listener: [stop]\n");
        ultrathin_matrix_stop(g_matrix);
    }
    else if (strcasecmp(buf, CMD_START) == 0) {
        LOG("UDP listener: [start]\n");
        ultrathin_matrix_start(g_matrix);
    }
    else if (strcasecmp(buf, CMD_OFF) == 0) {
        LOG("UDP listener: [off]\n");
        ultrathin_matrix_off(g_matrix);
    }
    else if (strcasecmp(buf, CMD_ON) == 0) {
        LOG("UDP listener: [on]\n");
        ultrathin_matrix_on(g_matrix);
    }
    else if (strcasecmp(buf, CMD_REVERSE) == 0) {
        LOG("UDP listener: [reverse]\n");
        ultrathin_matrix_reverse(g_matrix);
    }
    else if (strcasecmp(buf, CMD_CLEAR) == 0) {
        LOG("UDP listener: [clear]\n");
        ultrathin_matrix_clear(g_matrix);
    }
    else if (sscanf(buf, CMD_TEXT1, str) == 1) {
        LOG("UDP listener: [text1 => %s]\n", str);
        ultrathin_matrix_set_text1(g_matrix, str);
    }
    else if (sscanf(buf, CMD_SPEED1, &num) == 1) {
        LOG("UDP listener: [speed1 => %f]\n", num);
        ultrathin_matrix_set_text1_speed(g_matrix, num);
    }
    else if (sscanf(buf, CMD_TEXT2, str) == 1) {
        LOG("UDP listener: [text2 => %s]\n", str);
        ultrathin_matrix_set_text2(g_matrix, str);
    }
    else if (sscanf(buf, CMD_SPEED2, &num) == 1) {
        LOG("UDP listener: [speed2 => %f]\n", num);
        ultrathin_matrix_set_text2_speed(g_matrix, num);
    }
    else {
        LOG("UDP listener: [unknown]\n");
    }
}

/**
  // ----------------------------------------------------------------------
  UDP listener
  http://beej.us/guide/bgnet/examples/listener.c
  [TODO: comment]
*/
static void * udp_listener() {
    LOG("UDP listener: starting...\n");
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    char buf[MAXBUFLEN];
    socklen_t addr_len;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG; // use my IP

    if ((rv = getaddrinfo(NULL, UDP_PORT, &hints, &servinfo)) != 0) {
        LOG("UDP listener: Error: getaddrinfo: %s\n", gai_strerror(rv));
        fclose(g_logfp);
        exit(EXIT_FAILURE);
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        g_sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (g_sockfd == -1) {
            LOG("%s\n", strerror(errno));
            continue;
        }

        if (bind(g_sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(g_sockfd);
            LOG("%s\n", strerror(errno));
            continue;
        }

        // Success, we have bound to a port
        break;
    }

    if (p == NULL) {
        LOG("UDP listener: Failed to bind to socket. Exiting\n");
        fclose(g_logfp);
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(servinfo);

    // Listen loop
    while (g_running) {
        addr_len = sizeof their_addr;
        printf("UDP listener: waiting to recvfrom...\n");
        numbytes = recvfrom(g_sockfd,
                            buf,
                            MAXBUFLEN-1,
                            0,
                            (struct sockaddr *)&their_addr,
                            &addr_len);
        if (numbytes == -1) {
            LOG("UDP listener: error in recvfrom\n");
            fclose(g_logfp);
            pthread_exit(NULL);
        }

        if (g_running) {
            buf[numbytes-1] = '\0';
            //LOG("UDP listener: received: \"%s\"\n", buf);
            handle_command(buf);
        }
    }

    LOG("UDP listener: exiting\n");
    pthread_exit(NULL);
    return NULL;
}

/**
  // ----------------------------------------------------------------------
  // Main loop procedure
  [TODO: comment]
*/
void main_loop() {
    while (g_running) {
        ultrathin_matrix_tick(g_matrix);
        delay(100);
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
    g_logfp = fopen(LOG_FILE, "a");
    if (g_logfp == 0) {
        fprintf(stderr, "Could not open log file: %s\n", LOG_FILE);
        exit(EXIT_FAILURE);
    }
    LOG("----------------------------------------------\n");

    // ----------------------------------------------------------------------
    // Create a new SID for the child process
    sid = setsid();
    if (sid < 0) {
        LOG("Could not set SID: %d\n", sid);
        fclose(g_logfp);
        exit(EXIT_FAILURE);
    }
    LOG("Set SID: %d\n", sid);

    // ----------------------------------------------------------------------
    // Change the current working directory to /
    if ((chdir("/")) < 0) {
        LOG("Could not change CWD\n");
        fclose(g_logfp);
        exit(EXIT_FAILURE);
    }

    // ----------------------------------------------------------------------
    // Close standard file descriptiors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // ----------------------------------------------------------------------
    // Initialize matrix
    wiringPiSetup();

    g_matrix = ultrathin_matrix_create(g_display_buffer, MATRIX_WIDTH, MATRIX_HEIGHT);
    ultrathin_matrix_init(g_matrix, 0, 2, 3, 12, 13, 14, 21, 22);
    g_running = true;

    // ----------------------------------------------------------------------
    // Signal handler
    signal(SIGINT, signal_handler);

    // ----------------------------------------------------------------------
    // UDP listener thread
    pthread_t udp_listener_tid;
    int rc1 = pthread_create(&udp_listener_tid, NULL, udp_listener, NULL);
    if (rc1) {
        LOG("ERROR: in create of UDP listener thread: %d\n", rc1);
        fclose(g_logfp);
        exit(EXIT_FAILURE);
    }

    // ----------------------------------------------------------------------
    // Matrix scanner thread
    pthread_t matrix_scanner_tid;
    int rc2 = pthread_create(&matrix_scanner_tid, NULL, matrix_scanner, NULL);
    if (rc2) {
        LOG("ERROR: in create of Matrix scanner thread: %d\n", rc2);
        fclose(g_logfp);
        exit(EXIT_FAILURE);
    }

    // ----------------------------------------------------------------------
    // Call the main loop
    main_loop();

    // ----------------------------------------------------------------------
    // Stop the UDP listening thread
    shutdown(g_sockfd, SHUT_RDWR);
    pthread_join(udp_listener_tid, NULL);

    // ----------------------------------------------------------------------
    // Stop the Matrix scanner thread
    pthread_join(matrix_scanner_tid, NULL);

    // ----------------------------------------------------------------------
    // Clean up and exit
    free(g_matrix);
    if (unlink(PID_FILE) == -1) {
        LOG("%s\n", strerror(errno));
    }

    LOG("Main exit\n");
    fclose(g_logfp);
    pthread_exit(NULL);
    return EXIT_SUCCESS;
}


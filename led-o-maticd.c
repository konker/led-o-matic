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
#include <ini.h>

#include <kulm_matrix.h>
#include <kulm_segment.h>
#include <hexfont_iso-8859-15.h>

#define LEDOMATIC_USAGE "led-o-maticd -c <config file> [-l <log file>] [-p <pid file>] [-h]\n"

#define LEDOMATIC_DEFAULT_CONFIG_FILE "led-o-matic.ini"
#define LEDOMATIC_DEFAULT_LOG_FILE "/var/log/led-o-maticd.log"
#define LEDOMATIC_DEFAULT_PID_FILE "/var/run/led-o-maticd.pid"

#define LEDOMATIC_DEFAULT_UDP_PORT "7890"
#define LEDOMATIC_MAXBUFLEN 1024
#define LEDOMATIC_MAIN_LOOP_PERIOD_NANOS 10000000
#define LEDOMATIC_ONE_MILLION 1000000
#define LEDOMATIC_ONE_THOUSAND 1000

#define LEDOMATIC_DEFAULT_MATRIX_WIDTH 32
#define LEDOMATIC_DEFAULT_MATRIX_HEIGHT 16

#define LEDOMATIC_NOW_MICROSECS(var, time_spec_var) \
        clock_gettime(CLOCK_REALTIME, &time_spec_var); \
        var =  time_spec_var.tv_sec * LEDOMATIC_ONE_MILLION; \
        var += time_spec_var.tv_nsec / LEDOMATIC_ONE_THOUSAND; \

#define LEDOMATIC_STRINGIFY(x) #x
#define LEDOMATIC_TOSTRING(x) LEDOMATIC_STRINGIFY(x)

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
#define LEDOMATIC_CMD_TEXT "text:%" LEDOMATIC_TOSTRING(KULM_TEXT_LEN) "[^\t\n]"
// Set text1 scroll speed
#define LEDOMATIC_CMD_SPEED "speed:%f"
// Set text1 position
#define LEDOMATIC_CMD_POSITION "position:%f"

#define LEDOMATIC_LOG(...) fprintf(ledomatic_logfp, __VA_ARGS__); \
                           fflush(ledomatic_logfp);

static int ledomatic_sockfd;
static FILE *ledomatic_logfp;
volatile static bool ledomatic_running;

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
#ifndef KULM_NON_GPIO_MACHINE
static void *matrix_scanner() {
    LEDOMATIC_LOG("Matrix scanner: starting\n");

    while (ledomatic_running) {
        kulm_mat_scan(ledomatic_matrix);
    }

    LEDOMATIC_LOG("Matrix scanner: exiting\n");
    pthread_exit(NULL);
    return NULL;
}
#endif

#ifdef KULM_NON_GPIO_MACHINE
static void *matrix_dumper() {
    LEDOMATIC_LOG("Matrix dumper: starting\n");
    while (ledomatic_running) {
        kulm_mat_dump_buffer(ledomatic_matrix);

        sleep(1);
    }
    LEDOMATIC_LOG("Matrix dumper: exiting\n");
    pthread_exit(NULL);
    return NULL;
}
#endif

/**
  // ----------------------------------------------------------------------
  Config file handler
*/
typedef struct ledomatic_config {
    uint16_t matrix_width;
    uint16_t matrix_height;

} ledomatic_config;

static int config_handler(void* user, const char* section,
                          const char* name, const char* value)
{
    ledomatic_config* config = (ledomatic_config*)user;

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("matrix", "width")) {
        config->matrix_width = atoi(value);
    }
    else if (MATCH("matrix", "height")) {
        config->matrix_height = atoi(value);
    }
    else {
        LEDOMATIC_LOG("Warning: Unknown config item: %s, %s\n", section, name);
    }
    return 1;
}

/**
  // ----------------------------------------------------------------------
  Handle commands received via UDP socket
  [TODO: comment]
*/
static void handle_command(char *buf) {
    float num;
    char str[KULM_TEXT_LEN];
    if (strcasecmp(buf, LEDOMATIC_CMD_EXIT) == 0) {
        LEDOMATIC_LOG("UDP listener: [exit]\n");
        ledomatic_running = false;
    }
    else if (strcasecmp(buf, LEDOMATIC_CMD_STOP) == 0) {
        LEDOMATIC_LOG("UDP listener: [stop]\n");
        kulm_mat_simple_stop(ledomatic_matrix);
    }
    else if (strcasecmp(buf, LEDOMATIC_CMD_START) == 0) {
        LEDOMATIC_LOG("UDP listener: [start]\n");
        kulm_mat_simple_start(ledomatic_matrix);
    }
    else if (strcasecmp(buf, LEDOMATIC_CMD_OFF) == 0) {
        LEDOMATIC_LOG("UDP listener: [off]\n");
        kulm_mat_off(ledomatic_matrix);
    }
    else if (strcasecmp(buf, LEDOMATIC_CMD_ON) == 0) {
        LEDOMATIC_LOG("UDP listener: [on]\n");
        kulm_mat_on(ledomatic_matrix);
    }
    else if (strcasecmp(buf, LEDOMATIC_CMD_REVERSE) == 0) {
        LEDOMATIC_LOG("UDP listener: [reverse]\n");
        kulm_mat_simple_reverse(ledomatic_matrix);
    }
    else if (strcasecmp(buf, LEDOMATIC_CMD_CLEAR) == 0) {
        LEDOMATIC_LOG("UDP listener: [clear]\n");
        kulm_mat_simple_set_text(ledomatic_matrix, "");
        kulm_mat_clear(ledomatic_matrix);
    }
    else if (sscanf(buf, LEDOMATIC_CMD_TEXT, str) == 1) {
        LEDOMATIC_LOG("UDP listener: [text => %s]\n", str);
        kulm_mat_simple_set_text(ledomatic_matrix, str);
    }
    else if (sscanf(buf, LEDOMATIC_CMD_SPEED, &num) == 1) {
        LEDOMATIC_LOG("UDP listener: [speed => %f]\n", num);
        kulm_mat_simple_set_text_speed(ledomatic_matrix, num);
    }
    else if (sscanf(buf, LEDOMATIC_CMD_POSITION, &num) == 1) {
        LEDOMATIC_LOG("UDP listener: [position => %f]\n", num);
        kulm_mat_simple_set_text_position(ledomatic_matrix, num);
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
    LEDOMATIC_LOG("UDP listener: port [%s]: starting...\n", LEDOMATIC_DEFAULT_UDP_PORT);
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

    if ((rv = getaddrinfo(NULL, LEDOMATIC_DEFAULT_UDP_PORT, &hints, &servinfo)) != 0) {
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
    delay_t.tv_nsec = 0;

    struct timespec now_t;
    int64_t micros_0, micros_1;

    while (ledomatic_running) {
        LEDOMATIC_NOW_MICROSECS(micros_0, now_t)

        kulm_mat_tick(ledomatic_matrix);

        LEDOMATIC_NOW_MICROSECS(micros_1, now_t)

        // Delay to make loop time consistent
        delay_t.tv_nsec =
            LEDOMATIC_MAIN_LOOP_PERIOD_NANOS
                - ((micros_1 - micros_0) * LEDOMATIC_ONE_THOUSAND);

        nanosleep(&delay_t, NULL);
    }
}

int main(int argc, char **argv) {
    // ----------------------------------------------------------------------
    // Parse command line arguments
    char *config_file = LEDOMATIC_DEFAULT_CONFIG_FILE;
    char *log_file = LEDOMATIC_DEFAULT_LOG_FILE;
    char *pid_file = LEDOMATIC_DEFAULT_PID_FILE;
    int c;

    opterr = 0;
    while ((c = getopt(argc, argv, "hc:l:p:")) != -1) {
        switch (c) {
            case 'h':
                fprintf(stdout, LEDOMATIC_USAGE);
                exit(EXIT_SUCCESS);

            case 'c':
                config_file = optarg;
                break;

            case 'l':
                log_file = optarg;
                break;

            case 'p':
                pid_file = optarg;
                break;

            case '?':
                if (optopt == 'c') {
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                }
                else if (isprint(optopt)) {
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                }
                else {
                    fprintf(stderr,
                            "Unknown option character `\\x%x'.\n",
                            optopt);
                }
                return 1;

            default:
                exit(EXIT_FAILURE);
        }
    }
    fprintf(stderr, "Log file: %s\n", log_file);
    fprintf(stderr, "PID file: %s\n", pid_file);

    // ----------------------------------------------------------------------
    // Initialize log
    ledomatic_logfp = fopen(log_file, "a");
    if (ledomatic_logfp == 0) {
        fprintf(stderr, "Could not open log file: %s\n", log_file);
        exit(EXIT_FAILURE);
    }
    LEDOMATIC_LOG("----------------------------------------------\n");
    LEDOMATIC_LOG("Log file: %s\n", log_file);
    LEDOMATIC_LOG("PID file: %s\n", pid_file);
    LEDOMATIC_LOG("Config file: %s\n", config_file);

    // ----------------------------------------------------------------------
    // Parse the config file
    //[TODO]
    ledomatic_config config;

    if (ini_parse(config_file, config_handler, &config) < 0) {
        LEDOMATIC_LOG("Can't load config file %s. Aborting\n", config_file);
        fclose(ledomatic_logfp);
        exit(EXIT_FAILURE);
    }
    LEDOMATIC_LOG("Config loaded from '%s': matrix_width=%d, matrix_height=%d\n",
                  config_file, config.matrix_width, config.matrix_height);

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
        LEDOMATIC_LOG("Could not fork: %d\n", pid);
        fclose(ledomatic_logfp);
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        LEDOMATIC_LOG("Forked PID: %d\n", pid);

        // ----------------------------------------------------------------------
        // Create PID file
        pidfp = fopen(pid_file, "w");
        if (pidfp == NULL) {
            LEDOMATIC_LOG("Could not open PID file: %s\n", pid_file);
            fclose(ledomatic_logfp);
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
#endif

    // ----------------------------------------------------------------------
    // Signal handler
    signal(SIGINT, signal_handler);

    // ----------------------------------------------------------------------
    // Initialize WirinPi if necessary
#ifndef KULM_NON_GPIO_MACHINE
    if (wiringPiSetup()) {
        LEDOMATIC_LOG("ERROR Initializing WiringPi. Exiting. %s\n", strerror(errno));
        fclose(ledomatic_logfp);
        exit(EXIT_FAILURE);
    }
#endif

    // ----------------------------------------------------------------------
    // Create a matrix
    uint8_t ledomatic_display_buffer0[
                KULM_BUFFER_LEN(LEDOMATIC_DEFAULT_MATRIX_WIDTH, LEDOMATIC_DEFAULT_MATRIX_HEIGHT)];
    uint8_t ledomatic_display_buffer1[
                KULM_BUFFER_LEN(LEDOMATIC_DEFAULT_MATRIX_WIDTH, LEDOMATIC_DEFAULT_MATRIX_HEIGHT)];

    ledomatic_matrix = kulm_mat_create(
                            ledomatic_logfp,
                            ledomatic_display_buffer0,
                            ledomatic_display_buffer1,
                            LEDOMATIC_DEFAULT_MATRIX_WIDTH,
                            LEDOMATIC_DEFAULT_MATRIX_HEIGHT,
                            0, 2, 3, 12, 1, 14, 21, 22);

    // ----------------------------------------------------------------------
    // Initialize some font(s)
    hexfont * const font1 = hexfont_load_data(hexfont_iso_8859_15, 16);

    // ----------------------------------------------------------------------
    // Initialize the matrix
    kulm_mat_simple_init(ledomatic_matrix, font1);
    ledomatic_running = true;

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
#ifdef KULM_NON_GPIO_MACHINE
    int rc2 = pthread_create(&matrix_scanner_tid, NULL, matrix_dumper, NULL);
#else
    int rc2 = pthread_create(&matrix_scanner_tid, NULL, matrix_scanner, NULL);
#endif
    if (rc2) {
        LEDOMATIC_LOG("ERROR: in create of Matrix scanner thread: %d\n", rc2);
        fclose(ledomatic_logfp);
        exit(EXIT_FAILURE);
    }

    // ----------------------------------------------------------------------
    // Call the main loop
    main_loop();

    // Main loop now finished...

    // ----------------------------------------------------------------------
    // Stop the UDP listening thread
    shutdown(ledomatic_sockfd, SHUT_RDWR);
    pthread_join(udp_listener_tid, NULL);

    // ----------------------------------------------------------------------
    // Stop the Matrix scanner thread
    pthread_join(matrix_scanner_tid, NULL);

    // ----------------------------------------------------------------------
    // Clean up and exit
    kulm_mat_destroy(ledomatic_matrix);
#ifndef LEDOMATIC_NO_FORK
    if (unlink(pid_file) == -1) {
        LEDOMATIC_LOG("%s\n", strerror(errno));
    }
#endif

    LEDOMATIC_LOG("Main exit\n");
    fclose(ledomatic_logfp);
    pthread_exit(NULL);
    return EXIT_SUCCESS;
}


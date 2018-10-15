#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>

#include <klm_matrix.h>
#include <klm_segment.h>
#include "led-o-maticd.h"
#include "led-o-maticd_command.h"


/**
  // ----------------------------------------------------------------------
  UDP listener
  http://beej.us/guide/bgnet/examples/listener.c
  [TODO: comment]
*/
void * ledomatic_udp_listener_thread(void *arg) {
    ledomaticd *lomd = arg;
    LEDOMATIC_LOG(*lomd, "UDP listener(%p): host [%s] port [%s]: starting...\n",
                  lomd, lomd->config.udp_host, lomd->config.udp_port);

    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    char inbuf[LEDOMATIC_MAXBUFLEN];
    char outbuf[LEDOMATIC_MAXBUFLEN];
    socklen_t addr_len;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG; // use my IP

    if ((rv = getaddrinfo(lomd->config.udp_host, lomd->config.udp_port, &hints, &servinfo)) != 0) {
        LEDOMATIC_LOG(*lomd, "UDP listener: Error: getaddrinfo: %s\n", gai_strerror(rv));
        fclose(lomd->logfp);
        exit(EXIT_FAILURE);
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        lomd->sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (lomd->sockfd == -1) {
            LEDOMATIC_LOG(*lomd, "%s\n", strerror(errno));
            continue;
        }

        if (bind(lomd->sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(lomd->sockfd);
            LEDOMATIC_LOG(*lomd, "%s\n", strerror(errno));
            continue;
        }

        // Success, we have bound to a port
        break;
    }

    if (p == NULL) {
        LEDOMATIC_LOG(*lomd, "UDP listener: Failed to bind to socket. Exiting\n");
        fclose(lomd->logfp);
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(servinfo);

    // Listen loop
    while (lomd->running) {
        addr_len = sizeof their_addr;
        LEDOMATIC_LOG(*lomd, "UDP listener: waiting to recvfrom...\n");
        numbytes = recvfrom(lomd->sockfd,
                            inbuf,
                            LEDOMATIC_MAXBUFLEN-1,
                            0,
                            (struct sockaddr *)&their_addr,
                            &addr_len);

        if (numbytes == -1) {
            LEDOMATIC_LOG(*lomd, "UDP listener: error in recvfrom\n");
            fclose(lomd->logfp);
            pthread_exit(NULL);
        }

        if (lomd->running) {
            inbuf[numbytes-1] = '\0';
            outbuf[0] = '\0';
            LEDOMATIC_LOG(*lomd, "UDP listener: received: \"%s\"\n", inbuf);
            handle_command(lomd, inbuf, outbuf);

            size_t outbuflen = strlen(outbuf);
            if (outbuflen > 0) {
                numbytes = sendto(lomd->sockfd,
                                  outbuf,
                                  outbuflen,
                                  0,
                                  (struct sockaddr *)&their_addr,
                                  addr_len);

                if (numbytes == -1) {
                    LEDOMATIC_LOG(*lomd, "UDP listener: error in sendto\n");
                    fclose(lomd->logfp);
                    pthread_exit(NULL);
                }
            }
        }
    }

    LEDOMATIC_LOG(*lomd, "UDP listener: exiting\n");
    shutdown(lomd->sockfd, SHUT_RDWR);
    pthread_exit(NULL);
    return NULL;
}



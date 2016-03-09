#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>

#include "led-o-maticd.h"
#include "led-o-maticd_args.h"


int ledomatic_args_parse(int argc, char *argv[], ledomaticd * const lomd) {
    opterr = 0;
    int c;
    while ((c = getopt(argc, argv, "hc:l:p:")) != -1) {
        switch (c) {
            case 'h':
                fprintf(stdout, LEDOMATIC_USAGE);
                exit(EXIT_SUCCESS);

            case 'c':
                lomd->args.config_file = optarg;
                break;

            case 'l':
                lomd->args.log_file = optarg;
                break;

            case 'p':
                lomd->args.pid_file = optarg;
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
}


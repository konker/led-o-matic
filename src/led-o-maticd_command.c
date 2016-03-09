#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include <klm_matrix.h>
#include <klm_segment.h>
#include "led-o-maticd.h"
#include "led-o-maticd_command.h"


/**
  // ----------------------------------------------------------------------
  Handle commands received via UDP socket
  [TODO: comment]
*/
void handle_command(ledomaticd * const lomd, char *buf) {
    float num;
    char str[KLM_TEXT_LEN];
    if (strcasecmp(buf, LEDOMATIC_CMD_EXIT) == 0) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [exit]\n");
        lomd->running = false;
    }
    else if (strcasecmp(buf, LEDOMATIC_CMD_STOP) == 0) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [stop]\n");
        klm_mat_simple_stop(lomd->matrix);
    }
    else if (strcasecmp(buf, LEDOMATIC_CMD_START) == 0) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [start]\n");
        klm_mat_simple_start(lomd->matrix);
    }
    else if (strcasecmp(buf, LEDOMATIC_CMD_OFF) == 0) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [off]\n");
        klm_mat_off(lomd->matrix);
    }
    else if (strcasecmp(buf, LEDOMATIC_CMD_ON) == 0) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [on]\n");
        klm_mat_on(lomd->matrix);
    }
    else if (strcasecmp(buf, LEDOMATIC_CMD_REVERSE) == 0) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [reverse]\n");
        klm_mat_simple_reverse(lomd->matrix);
    }
    else if (strcasecmp(buf, LEDOMATIC_CMD_CLEAR) == 0) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [clear]\n");
        klm_mat_simple_set_text(lomd->matrix, "");
        klm_mat_clear(lomd->matrix);
    }
    else if (sscanf(buf, LEDOMATIC_CMD_TEXT, str) == 1) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [text => %s]\n", str);
        klm_mat_simple_set_text(lomd->matrix, str);
    }
    else if (sscanf(buf, LEDOMATIC_CMD_SPEED, &num) == 1) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [speed => %f]\n", num);
        klm_mat_simple_set_text_speed(lomd->matrix, num);
    }
    else if (sscanf(buf, LEDOMATIC_CMD_POSITION, &num) == 1) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [position => %f]\n", num);
        klm_mat_simple_set_text_position(lomd->matrix, num);
    }
    else {
        LEDOMATIC_LOG(*lomd, "UDP listener: [unknown]\n");
    }
}


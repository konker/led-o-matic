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
    int seg;
    float num;
    char str[KLM_TEXT_LEN];
    if (strcasecmp(buf, LEDOMATIC_CMD_EXIT) == 0) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [exit]\n");
        lomd->running = false;
    }
    else if (strcasecmp(buf, LEDOMATIC_CMD_CLEAR) == 0) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [clear]\n");
        klm_mat_clear(lomd->matrix);
    }
    else if (sscanf(buf, LEDOMATIC_CMD_STOP, &seg) == 1) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [stop %d]\n", seg);
        klm_seg_stop(klm_segment_list_get_nth(lomd->matrix->segment_list, seg));
    }
    else if (sscanf(buf, LEDOMATIC_CMD_START, &seg) == 1) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [start %d]\n", seg);
        klm_seg_start(klm_segment_list_get_nth(lomd->matrix->segment_list, seg));
    }
    else if (strcasecmp(buf, LEDOMATIC_CMD_OFF) == 0) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [off]\n");
        klm_mat_off(lomd->matrix);
    }
    else if (strcasecmp(buf, LEDOMATIC_CMD_ON) == 0) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [on]\n");
        klm_mat_on(lomd->matrix);
    }
    else if (sscanf(buf, LEDOMATIC_CMD_HIDE, &seg) == 1) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [hide %d]\n", seg);
        klm_seg_hide(klm_segment_list_get_nth(lomd->matrix->segment_list, seg));
    }
    else if (sscanf(buf, LEDOMATIC_CMD_SHOW, &seg) == 1) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [show %d]\n", seg);
        klm_seg_show(klm_segment_list_get_nth(lomd->matrix->segment_list, seg));
    }
    else if (sscanf(buf, LEDOMATIC_CMD_REVERSE, &seg) == 1) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [reverse %d]\n", seg);
        klm_seg_reverse(klm_segment_list_get_nth(lomd->matrix->segment_list, seg));
    }
    else if (sscanf(buf, LEDOMATIC_CMD_TEXT, &seg, str) == 2) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [text %d => %s]\n", seg, str);
        klm_seg_set_text(klm_segment_list_get_nth(lomd->matrix->segment_list, seg), str);
    }
    else if (sscanf(buf, LEDOMATIC_CMD_SPEED, &seg, &num) == 2) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [speed %d => %f]\n", seg, num);
        klm_seg_set_text_speed(klm_segment_list_get_nth(lomd->matrix->segment_list, seg), num);
    }
    else if (sscanf(buf, LEDOMATIC_CMD_POSITION, &seg, &num) == 2) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [position %d => %f]\n", seg, num);
        klm_seg_set_text_position(klm_segment_list_get_nth(lomd->matrix->segment_list, seg), num);
    }
    else {
        LEDOMATIC_LOG(*lomd, "UDP listener: [unknown]\n");
    }
}


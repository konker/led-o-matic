#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include <klm_matrix.h>
#include <klm_segment.h>
#include "led-o-maticd.h"
#include "led-o-maticd_command.h"


// Clean up and exit daemon
#define LEDOMATIC_CMD_EXIT "exit"
// Clear the display
#define LEDOMATIC_CMD_CLEAR "clear"
// Switch display off
#define LEDOMATIC_CMD_OFF "off"
// Switch display on
#define LEDOMATIC_CMD_ON "on"
// Stop scrolling a segment
#define LEDOMATIC_CMD_STOP "stop:%d"
// Resume scrolling a segment
#define LEDOMATIC_CMD_START "start:%d"
// Hide a segment
#define LEDOMATIC_CMD_HIDE "hide:%d"
// Show a segment
#define LEDOMATIC_CMD_SHOW "show:%d"
// Reverse display colors for a segment
#define LEDOMATIC_CMD_REVERSE "reverse:%d"
// Set the text for a segment
#define LEDOMATIC_CMD_TEXT "text:%d:%" LEDOMATIC_TOSTRING(KLM_TEXT_LEN) "[^\t\n]"
// Set scroll speed for a segment
#define LEDOMATIC_CMD_SPEED "speed:%d:%f"
// Set the text position for a segment
#define LEDOMATIC_CMD_POSITION "position:%d:%f"


/**
  // ----------------------------------------------------------------------
  Handle commands received via UDP socket
  [TODO: comment]
*/
void handle_command(ledomaticd * const lomd, char *buf) {
    int seg_index;
    float num;
    char str[KLM_TEXT_LEN];
    klm_segment *seg;

    if (strcasecmp(buf, LEDOMATIC_CMD_EXIT) == 0) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [exit]\n");
        lomd->running = false;
    }
    else if (strcasecmp(buf, LEDOMATIC_CMD_CLEAR) == 0) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [clear]\n");
        klm_mat_clear(lomd->matrix);
    }
    else if (sscanf(buf, LEDOMATIC_CMD_STOP, &seg_index) == 1) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [stop %d]\n", seg_index);
        seg = klm_segment_list_get_nth(lomd->matrix->segment_list, seg_index);
        if (seg == NULL) {
            LEDOMATIC_LOG(*lomd, "Warning: Bad segment index: %d - ignoring\n", seg_index);
            return;
        }
        klm_seg_stop(seg);
    }
    else if (sscanf(buf, LEDOMATIC_CMD_START, &seg_index) == 1) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [start %d]\n", seg_index);
        seg = klm_segment_list_get_nth(lomd->matrix->segment_list, seg_index);
        if (seg == NULL) {
            LEDOMATIC_LOG(*lomd, "Warning: Bad segment index: %d - ignoring\n", seg_index);
            return;
        }
        klm_seg_start(seg);
    }
    else if (strcasecmp(buf, LEDOMATIC_CMD_OFF) == 0) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [off]\n");
        klm_mat_off(lomd->matrix);
    }
    else if (strcasecmp(buf, LEDOMATIC_CMD_ON) == 0) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [on]\n");
        klm_mat_on(lomd->matrix);
    }
    else if (sscanf(buf, LEDOMATIC_CMD_HIDE, &seg_index) == 1) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [hide %d]\n", seg_index);
        seg = klm_segment_list_get_nth(lomd->matrix->segment_list, seg_index);
        if (seg == NULL) {
            LEDOMATIC_LOG(*lomd, "Warning: Bad segment index: %d - ignoring\n", seg_index);
            return;
        }
        klm_seg_hide(seg);
    }
    else if (sscanf(buf, LEDOMATIC_CMD_SHOW, &seg_index) == 1) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [show %d]\n", seg_index);
        seg = klm_segment_list_get_nth(lomd->matrix->segment_list, seg_index);
        if (seg == NULL) {
            LEDOMATIC_LOG(*lomd, "Warning: Bad segment index: %d - ignoring\n", seg_index);
            return;
        }
        klm_seg_show(seg);
    }
    else if (sscanf(buf, LEDOMATIC_CMD_REVERSE, &seg_index) == 1) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [reverse %d]\n", seg_index);
        seg = klm_segment_list_get_nth(lomd->matrix->segment_list, seg_index);
        if (seg == NULL) {
            LEDOMATIC_LOG(*lomd, "Warning: Bad segment index: %d - ignoring\n", seg_index);
            return;
        }
        klm_seg_reverse(seg);
    }
    else if (sscanf(buf, LEDOMATIC_CMD_TEXT, &seg_index, str) == 2) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [text %d => %s]\n", seg_index, str);
        seg = klm_segment_list_get_nth(lomd->matrix->segment_list, seg_index);
        if (seg == NULL) {
            LEDOMATIC_LOG(*lomd, "Warning: Bad segment index: %d - ignoring\n", seg_index);
            return;
        }
        klm_seg_set_text(seg, str);
    }
    else if (sscanf(buf, LEDOMATIC_CMD_SPEED, &seg_index, &num) == 2) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [speed %d => %f]\n", seg_index, num);
        seg = klm_segment_list_get_nth(lomd->matrix->segment_list, seg_index);
        if (seg == NULL) {
            LEDOMATIC_LOG(*lomd, "Warning: Bad segment index: %d - ignoring\n", seg_index);
            return;
        }
        klm_seg_set_text_speed(seg, num);
    }
    else if (sscanf(buf, LEDOMATIC_CMD_POSITION, &seg_index, &num) == 2) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [position %d => %f]\n", seg_index, num);
        seg = klm_segment_list_get_nth(lomd->matrix->segment_list, seg_index);
        if (seg == NULL) {
            LEDOMATIC_LOG(*lomd, "Warning: Bad segment index: %d - ignoring\n", seg_index);
            return;
        }
        klm_seg_set_text_position(seg, num);
    }
    else {
        LEDOMATIC_LOG(*lomd, "UDP listener: [unknown]\n");
    }
}


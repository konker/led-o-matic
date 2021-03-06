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
// Clear a segment
#define LEDOMATIC_CMD_CLEAR_SEG "clear:%d"
// Switch display off
#define LEDOMATIC_CMD_OFF "off"
// Switch display on
#define LEDOMATIC_CMD_ON "on"
// Hide a segment
#define LEDOMATIC_CMD_HIDE "hide:%d"
// Show a segment
#define LEDOMATIC_CMD_SHOW "show:%d"
// Reverse display colors for a segment
#define LEDOMATIC_CMD_REVERSE "reverse:%d"
// Reverse display colors for a segment
#define LEDOMATIC_CMD_SCAN_MODULATE "modulate:%hu"
// Set the text for a segment
#define LEDOMATIC_CMD_SET_TEXT "text:%d:%m[^\t\n]"
// Get the text for a segment
#define LEDOMATIC_CMD_GET_TEXT "text:%d"
// Stop scrolling a segment
#define LEDOMATIC_CMD_STOP "stop:%d"
// Resume scrolling a segment
#define LEDOMATIC_CMD_START "start:%d"
// Get/set scroll speed for a segment
#define LEDOMATIC_CMD_SPEED "speed:%d:%f:%f"
// Get/set the text position for a segment
#define LEDOMATIC_CMD_POSITION "position:%d:%f:%f"
// Get/set the centre coordinates of the text on the given segment
#define LEDOMATIC_CMD_CENTER "center:%d:%f:%f"
// Get the text pixel dimensions on the given segment
#define LEDOMATIC_CMD_DIMENSIONS "dimensions:%d"


/**
  // ----------------------------------------------------------------------
  Handle commands received via UDP socket
  [TODO: comment]
*/
void handle_command(ledomaticd * const lomd, char *inbuf, char *outbuf) {
    int seg_index;
    uint16_t scan_modulation;
    float num1, num2;
    char *strp;
    klm_segment *seg;

    if (strcasecmp(inbuf, LEDOMATIC_CMD_EXIT) == 0) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [exit]\n");
        lomd->running = false;
    }
    else if (strcasecmp(inbuf, LEDOMATIC_CMD_CLEAR) == 0) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [clear]\n");
        klm_mat_clear_text(lomd->matrix);
    }
    else if (sscanf(inbuf, LEDOMATIC_CMD_CLEAR_SEG, &seg_index) == 1) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [clear seg %d]\n", seg_index);
        seg = klm_segment_list_get_nth(lomd->matrix->segment_list, seg_index);
        if (seg == NULL) {
            LEDOMATIC_LOG(*lomd, "Warning: Bad segment index: %d - ignoring\n", seg_index);
            return;
        }
        klm_seg_clear_text(seg);
    }
    else if (strcasecmp(inbuf, LEDOMATIC_CMD_OFF) == 0) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [off]\n");
        klm_mat_off(lomd->matrix);
    }
    else if (strcasecmp(inbuf, LEDOMATIC_CMD_ON) == 0) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [on]\n");
        klm_mat_on(lomd->matrix);
    }
    else if (sscanf(inbuf, LEDOMATIC_CMD_HIDE, &seg_index) == 1) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [hide %d]\n", seg_index);
        seg = klm_segment_list_get_nth(lomd->matrix->segment_list, seg_index);
        if (seg == NULL) {
            LEDOMATIC_LOG(*lomd, "Warning: Bad segment index: %d - ignoring\n", seg_index);
            return;
        }
        klm_seg_hide(seg);
    }
    else if (sscanf(inbuf, LEDOMATIC_CMD_SHOW, &seg_index) == 1) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [show %d]\n", seg_index);
        seg = klm_segment_list_get_nth(lomd->matrix->segment_list, seg_index);
        if (seg == NULL) {
            LEDOMATIC_LOG(*lomd, "Warning: Bad segment index: %d - ignoring\n", seg_index);
            return;
        }
        klm_seg_show(seg);
    }
    else if (sscanf(inbuf, LEDOMATIC_CMD_REVERSE, &seg_index) == 1) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [reverse %d]\n", seg_index);
        seg = klm_segment_list_get_nth(lomd->matrix->segment_list, seg_index);
        if (seg == NULL) {
            LEDOMATIC_LOG(*lomd, "Warning: Bad segment index: %d - ignoring\n", seg_index);
            return;
        }
        klm_seg_reverse(seg);
    }
    else if (sscanf(inbuf, LEDOMATIC_CMD_SCAN_MODULATE, &scan_modulation) == 1) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [modulate %hu]\n", scan_modulation);
        klm_mat_set_scan_modulation(lomd->matrix, scan_modulation);
    }
    else if (sscanf(inbuf, LEDOMATIC_CMD_SET_TEXT, &seg_index, &strp) == 2) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [text %d => %s]\n", seg_index, strp);
        seg = klm_segment_list_get_nth(lomd->matrix->segment_list, seg_index);
        if (seg == NULL) {
            LEDOMATIC_LOG(*lomd, "Warning: Bad segment index: %d - ignoring\n", seg_index);
            return;
        }
        klm_seg_set_text(seg, strp);
        free(strp);
    }
    else if (sscanf(inbuf, LEDOMATIC_CMD_GET_TEXT, &seg_index) == 1) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [text? %d]\n", seg_index);
        seg = klm_segment_list_get_nth(lomd->matrix->segment_list, seg_index);
        if (seg == NULL) {
            LEDOMATIC_LOG(*lomd, "Warning: Bad segment index: %d - ignoring\n", seg_index);
            return;
        }
        sprintf(outbuf, "%s\n", seg->text);
    }
    else if (sscanf(inbuf, LEDOMATIC_CMD_STOP, &seg_index) == 1) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [stop %d]\n", seg_index);
        seg = klm_segment_list_get_nth(lomd->matrix->segment_list, seg_index);
        if (seg == NULL) {
            LEDOMATIC_LOG(*lomd, "Warning: Bad segment index: %d - ignoring\n", seg_index);
            return;
        }
        klm_seg_stop(seg);
    }
    else if (sscanf(inbuf, LEDOMATIC_CMD_START, &seg_index) == 1) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [start %d]\n", seg_index);
        seg = klm_segment_list_get_nth(lomd->matrix->segment_list, seg_index);
        if (seg == NULL) {
            LEDOMATIC_LOG(*lomd, "Warning: Bad segment index: %d - ignoring\n", seg_index);
            return;
        }
        klm_seg_start(seg);
    }
    else if (sscanf(inbuf, LEDOMATIC_CMD_SPEED, &seg_index, &num1, &num2) == 1) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [speed? %d]\n", seg_index);
        seg = klm_segment_list_get_nth(lomd->matrix->segment_list, seg_index);
        if (seg == NULL) {
            LEDOMATIC_LOG(*lomd, "Warning: Bad segment index: %d - ignoring\n", seg_index);
            return;
        }
        sprintf(outbuf, "%f,%f\n", seg->text_hspeed, seg->text_vspeed);
    }
    else if (sscanf(inbuf, LEDOMATIC_CMD_SPEED, &seg_index, &num1, &num2) == 3) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [speed %d => %f]\n", seg_index, num1);
        seg = klm_segment_list_get_nth(lomd->matrix->segment_list, seg_index);
        if (seg == NULL) {
            LEDOMATIC_LOG(*lomd, "Warning: Bad segment index: %d - ignoring\n", seg_index);
            return;
        }
        klm_seg_set_text_speed(seg, num1, num2);
    }
    else if (sscanf(inbuf, LEDOMATIC_CMD_POSITION, &seg_index, &num1, &num2) == 1) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [position? %d]\n", seg_index);
        seg = klm_segment_list_get_nth(lomd->matrix->segment_list, seg_index);
        if (seg == NULL) {
            LEDOMATIC_LOG(*lomd, "Warning: Bad segment index: %d - ignoring\n", seg_index);
            return;
        }
        sprintf(outbuf, "%f,%f\n", seg->text_hpos, seg->text_vpos);
    }
    else if (sscanf(inbuf, LEDOMATIC_CMD_POSITION, &seg_index, &num1, &num2) == 3) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [position %d => %f]\n", seg_index, num1);
        seg = klm_segment_list_get_nth(lomd->matrix->segment_list, seg_index);
        if (seg == NULL) {
            LEDOMATIC_LOG(*lomd, "Warning: Bad segment index: %d - ignoring\n", seg_index);
            return;
        }
        klm_seg_set_text_position(seg, num1, num2);
    }
    else if (sscanf(inbuf, LEDOMATIC_CMD_CENTER, &seg_index, &num1, &num2) == 1) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [center? %d]\n", seg_index);
        seg = klm_segment_list_get_nth(lomd->matrix->segment_list, seg_index);
        if (seg == NULL) {
            LEDOMATIC_LOG(*lomd, "Warning: Bad segment index: %d - ignoring\n", seg_index);
            return;
        }
        klm_seg_query_center_text(seg, &num1, &num2);
        sprintf(outbuf, "%f,%f\n", num1, num2);
    }
    else if (sscanf(inbuf, LEDOMATIC_CMD_CENTER, &seg_index, &num1, &num2) == 3) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [center %d, h=%d, v=%d]\n", seg_index, num1, num2);
        seg = klm_segment_list_get_nth(lomd->matrix->segment_list, seg_index);
        if (seg == NULL) {
            LEDOMATIC_LOG(*lomd, "Warning: Bad segment index: %d - ignoring\n", seg_index);
            return;
        }
        klm_seg_center_text(seg, (bool)num1, (bool)num2);
        sprintf(outbuf, "%f,%f\n", seg->text_hpos, seg->text_vpos);
    }
    else if (sscanf(inbuf, LEDOMATIC_CMD_DIMENSIONS, &seg_index) == 1) {
        LEDOMATIC_LOG(*lomd, "UDP listener: [dimensions? %d]\n", seg_index);
        seg = klm_segment_list_get_nth(lomd->matrix->segment_list, seg_index);
        if (seg == NULL) {
            LEDOMATIC_LOG(*lomd, "Warning: Bad segment index: %d - ignoring\n", seg_index);
            return;
        }
        sprintf(outbuf, "%d,%d\n", klm_seg_get_text_pixel_width(seg), klm_seg_get_text_pixel_height(seg));
    }
    else {
        LEDOMATIC_LOG(*lomd, "UDP listener: [unknown]\n");
    }
}


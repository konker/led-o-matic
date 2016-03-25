#include <pthread.h>

#include <stdlib.h>
#include <hexfont.h>
#include <klm_matrix.h>
#include "led-o-maticd.h"
#include "led-o-maticd_config.h"
#include "led-o-maticd_matrix.h"


bool ledomatic_matrix_init(ledomaticd * const lomd) {
    // ----------------------------------------------------------------------
    // Initialize matrix system
    if (!klm_mat_begin()) {
        return false;
    }

    // ----------------------------------------------------------------------
    // Create a matrix
    uint8_t *ledomatic_display_buffer0;
    uint8_t *ledomatic_display_buffer1;
    ledomatic_display_buffer0 =
        calloc(KLM_BUFFER_LEN(lomd->config.matrix_width, lomd->config.matrix_height),
               sizeof(*ledomatic_display_buffer1));
    ledomatic_display_buffer1 =
        calloc(KLM_BUFFER_LEN(lomd->config.matrix_width, lomd->config.matrix_height),
               sizeof(*ledomatic_display_buffer1));

    lomd->matrix = klm_mat_create(
                            lomd->logfp,
                            ledomatic_display_buffer0,
                            ledomatic_display_buffer1,
                            lomd->config.matrix_width,
                            lomd->config.matrix_height,
                            lomd->config.a,
                            lomd->config.b,
                            lomd->config.c,
                            lomd->config.d,
                            lomd->config.oe,
                            lomd->config.r1,
                            lomd->config.stb,
                            lomd->config.clk);

    // ----------------------------------------------------------------------
    // Initialize some font(s)
    hexfont_list * const font_list = hexfont_list_create(NULL);
    int8_t i;
    for (i=0; i<lomd->config.num_fonts; i++) {
        hexfont * const font = hexfont_load(lomd->config.fonts[i], 16);
        if (font == NULL) {
            LEDOMATIC_LOG(*lomd, "ERROR: Could not load font: %s\n", lomd->config.fonts[i]);
            return false;
        }

        // Add the font to the font list
        hexfont_list_append(font_list, font);
    }

    // ----------------------------------------------------------------------
    // Initialize the segments
    klm_segment_list * const segment_list = klm_segment_list_create(NULL);

    for (i=0; i<lomd->config.num_segments; i++) {
        klm_segment * const segment =
            klm_seg_create(
                    lomd->matrix,
                    lomd->config.segment_configs[i].x,
                    lomd->config.segment_configs[i].y,
                    lomd->config.segment_configs[i].width,
                    lomd->config.segment_configs[i].height,
                    lomd->config.segment_configs[i].font_index);

printf("Appending segment %p to list %p\n", segment, segment_list);
        // Add the segments to the segment list
        klm_segment_list_append(segment_list, segment);
    }
printf("Segment list length: %d\n", klm_segment_list_get_length(segment_list));
printf("Segment item 0: %p\n", klm_segment_list_get_nth(segment_list, 0));
printf("Segment item 1: %p\n", klm_segment_list_get_nth(segment_list, 1));
printf("Segment item 2: %p\n", klm_segment_list_get_nth(segment_list, 2));

    // ----------------------------------------------------------------------
    // Initialize the matrix with the segments and fonts
    klm_mat_init(
            lomd->matrix,
            font_list,
            segment_list);

    return true;
}


/**
  // ----------------------------------------------------------------------
  Clean up matrix resources
  [TODO: comment]
*/
void ledomatic_matrix_exit(ledomaticd * const lomd) {
    klm_mat_destroy(lomd->matrix);
    free(lomd->matrix->display_buffer0);
    free(lomd->matrix->display_buffer1);
}

/**
  // ----------------------------------------------------------------------
  Main matrix scan thread function
  [TODO: comment]
*/
void *ledomatic_matrix_scanner_thread(void *arg) {
    ledomaticd *lomd = arg;
    LEDOMATIC_LOG(*lomd, "Matrix scanner: starting: %p\n", lomd);

    while (lomd->running) {
        if (lomd->scan_lock) {
            continue;
        }
        klm_mat_scan(lomd->matrix);
    }

    LEDOMATIC_LOG(*lomd, "Matrix scanner: exiting\n");
    pthread_exit(NULL);
    return NULL;
}


#include <pthread.h>

#include <stdlib.h>
#include <hexfont.h>
#include <klm_config.h>
#include <klm_matrix.h>
#include "led-o-maticd.h"
#include "led-o-maticd_config.h"
#include "led-o-maticd_matrix.h"


uint8_t *ledomatic_display_buffer0;
uint8_t *ledomatic_display_buffer1;

bool ledomatic_matrix_init(ledomaticd * const lomd) {
    // ----------------------------------------------------------------------
    // Initialize matrix system
    if (!klm_mat_begin()) {
        return false;
    }

    // ----------------------------------------------------------------------
    // Create a matrix
    klm_config *config =
            klm_config_create(lomd->config.matrix_width, lomd->config.matrix_height);
    klm_config_set_pin(config, 'a', lomd->config.a);
    klm_config_set_pin(config, 'b', lomd->config.b);
    klm_config_set_pin(config, 'c', lomd->config.c);
    klm_config_set_pin(config, 'd', lomd->config.d);
    klm_config_set_pin(config, 'o', lomd->config.oe);
    klm_config_set_pin(config, 'r', lomd->config.r1);
    klm_config_set_pin(config, 's', lomd->config.stb);
    klm_config_set_pin(config, 'x', lomd->config.clk);

    lomd->matrix = klm_mat_create(lomd->logfp, config);

    // ----------------------------------------------------------------------
    // Initialize some font(s)
    hexfont_list * const font_list = hexfont_list_create(NULL);
    int8_t i;
    for (i=0; i<lomd->config.num_fonts; i++) {
        hexfont * const font = hexfont_load(
                lomd->config.font_configs[i].file,
                lomd->config.font_configs[i].height);

        if (font == NULL) {
            LEDOMATIC_LOG(*lomd, "ERROR: Could not load font: %s\n", lomd->config.font_configs[i]);
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

        // Add the segments to the segment list
        klm_segment_list_append(segment_list, segment);
        LEDOMATIC_LOG(*lomd, "Appended segment %p to list %p\n", segment, segment_list);
    }

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
    free(ledomatic_display_buffer0);
    free(ledomatic_display_buffer1);
}


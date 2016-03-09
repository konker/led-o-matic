#include <pthread.h>

#include <stdlib.h>
#include <hexfont.h>
#include <klm_matrix.h>
#include <hexfont_iso-8859-15.h>
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
        calloc(KLM_BUFFER_LEN(LEDOMATIC_DEFAULT_MATRIX_WIDTH, LEDOMATIC_DEFAULT_MATRIX_HEIGHT),
               sizeof(*ledomatic_display_buffer1));
    ledomatic_display_buffer1 =
        calloc(KLM_BUFFER_LEN(LEDOMATIC_DEFAULT_MATRIX_WIDTH, LEDOMATIC_DEFAULT_MATRIX_HEIGHT),
               sizeof(*ledomatic_display_buffer1));

    lomd->matrix = klm_mat_create(
                            lomd->logfp,
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
    klm_mat_simple_init(lomd->matrix, font1);

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
        klm_mat_scan(lomd->matrix);
    }

    LEDOMATIC_LOG(*lomd, "Matrix scanner: exiting\n");
    pthread_exit(NULL);
    return NULL;
}


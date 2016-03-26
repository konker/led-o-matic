#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#include <ini.h>

#include "led-o-maticd.h"
#include "led-o-maticd_config.h"

#define LEDOMATIC_CONFIG_NUM_SEGMENT_FIELDS 5

static ledomaticd *ledomatic_config_global_lomd;
static ledomatic_config_segment ledomatic_config_cur_segment;
static bool ledomatic_config_in_segment;
static uint8_t ledomatic_config_segment_field_count;
static uint8_t ledomatic_config_num_fonts;
static uint8_t ledomatic_config_num_segments;
static uint8_t ledomatic_config_cur_segment_index;
static uint8_t ledomatic_config_cur_font_index;

/**
  // ----------------------------------------------------------------------
  Config file handler
*/
int ledomatic_config_handler_p1(void* user, const char* section,
                                const char* name, const char* value)
{
    ledomatic_config* config = (ledomatic_config*)user;

    #define MATCH_SECTION(s) strcmp(section, s) == 0
    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH_SECTION("font")) {
        ledomatic_config_num_fonts++;
    }
    if (MATCH_SECTION("segment")) {
        if (!ledomatic_config_in_segment) {
            ledomatic_config_num_segments++;
            ledomatic_config_segment_field_count = 0;
        }
        ledomatic_config_in_segment = true;
    }
    else {
        if (ledomatic_config_in_segment && ledomatic_config_segment_field_count != LEDOMATIC_CONFIG_NUM_SEGMENT_FIELDS) {
            // Wrong number of fields in segment config, fatal error
            LEDOMATIC_LOG(*ledomatic_config_global_lomd, "Fatal Error: Wrong number fields in segment config\n");
            return -1;
        }
        ledomatic_config_in_segment = false;
    }

    if (MATCH("segment", "x")) {
        ledomatic_config_segment_field_count++;
    }
    else if (MATCH("segment", "y")) {
        ledomatic_config_segment_field_count++;
    }
    else if (MATCH("segment", "width")) {
        ledomatic_config_segment_field_count++;
    }
    else if (MATCH("segment", "height")) {
        ledomatic_config_segment_field_count++;
    }
    else if (MATCH("segment", "font_index")) {
        ledomatic_config_segment_field_count++;
    }

    if (ledomatic_config_segment_field_count == LEDOMATIC_CONFIG_NUM_SEGMENT_FIELDS) {
        // End of segment
        ledomatic_config_segment_field_count = 0;
        ledomatic_config_in_segment = false;
    }

    return 1;
}

int ledomatic_config_handler_p2(void* user, const char* section,
                                const char* name, const char* value)
{
    ledomatic_config* config = (ledomatic_config*)user;

    #define MATCH_SECTION(s) strcmp(section, s) == 0
    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH_SECTION("font")) {
        config->fonts[ledomatic_config_cur_font_index] = strdup(value);
        ledomatic_config_cur_font_index++;
    }
    else if (MATCH_SECTION("segment")) {
        if (!ledomatic_config_in_segment) {
            ledomatic_config_segment_field_count = 0;

            ledomatic_config_cur_segment.x = 0;
            ledomatic_config_cur_segment.y = 0;
            ledomatic_config_cur_segment.width = 0;
            ledomatic_config_cur_segment.height = 0;
            ledomatic_config_cur_segment.font_index = 0;
        }
        ledomatic_config_in_segment = true;
    }

    if (MATCH("udp", "host")) {
        config->udp_host = strdup(value);
    }
    else if (MATCH("udp", "port")) {
        config->udp_port = strdup(value);
    }
    else if (MATCH("segment", "x")) {
        config->segment_configs[ledomatic_config_cur_segment_index].x = atoi(value);
        ledomatic_config_segment_field_count++;
    }
    else if (MATCH("segment", "y")) {
        config->segment_configs[ledomatic_config_cur_segment_index].y = atoi(value);
        ledomatic_config_segment_field_count++;
    }
    else if (MATCH("segment", "width")) {
        config->segment_configs[ledomatic_config_cur_segment_index].width = atoi(value);
        ledomatic_config_segment_field_count++;
    }
    else if (MATCH("segment", "height")) {
        config->segment_configs[ledomatic_config_cur_segment_index].height = atoi(value);
        ledomatic_config_segment_field_count++;
    }
    else if (MATCH("segment", "font_index")) {
        config->segment_configs[ledomatic_config_cur_segment_index].font_index = atoi(value);
        ledomatic_config_segment_field_count++;
    }
    else if (MATCH("matrix", "width")) {
        config->matrix_width = atoi(value);
    }
    else if (MATCH("matrix", "height")) {
        config->matrix_height = atoi(value);
    }
    else if (MATCH("matrix", "a")) {
        config->a = atoi(value);
    }
    else if (MATCH("matrix", "b")) {
        config->b = atoi(value);
    }
    else if (MATCH("matrix", "c")) {
        config->c = atoi(value);
    }
    else if (MATCH("matrix", "d")) {
        config->d = atoi(value);
    }
    else if (MATCH("matrix", "oe")) {
        config->oe = atoi(value);
    }
    else if (MATCH("matrix", "r1")) {
        config->r1 = atoi(value);
    }
    else if (MATCH("matrix", "stb")) {
        config->stb = atoi(value);
    }
    else if (MATCH("matrix", "clk")) {
        config->clk = atoi(value);
    }
    else if (!MATCH("font", "file")) {
        LEDOMATIC_LOG(*ledomatic_config_global_lomd, "Warning: Unknown config item: %s, %s\n", section, name);
    }

    if (ledomatic_config_segment_field_count == LEDOMATIC_CONFIG_NUM_SEGMENT_FIELDS) {
        // End of segment
        ledomatic_config_cur_segment_index,
        config->segment_configs[ledomatic_config_cur_segment_index].x,
        config->segment_configs[ledomatic_config_cur_segment_index].y,
        config->segment_configs[ledomatic_config_cur_segment_index].width,
        config->segment_configs[ledomatic_config_cur_segment_index].height,
        config->segment_configs[ledomatic_config_cur_segment_index].font_index;

        ledomatic_config_segment_field_count = 0;
        ledomatic_config_in_segment = false;
        ledomatic_config_cur_segment_index++;
    }

    return 1;
}

bool ledomatic_config_parse(ledomaticd * const lomd) {
    ledomatic_config_global_lomd = lomd;
    ledomatic_config_in_segment = false;
    ledomatic_config_segment_field_count = 0;
    ledomatic_config_num_fonts = 0;
    ledomatic_config_num_segments = 0;
    ledomatic_config_cur_segment_index = 0;
    ledomatic_config_cur_font_index = 0;

    if (ini_parse(lomd->args.config_file, &ledomatic_config_handler_p1, &lomd->config) < 0) {
        return false;
    }
    LEDOMATIC_LOG(*lomd, "Config first pass: fonts: %d, segments: %d\n",
                  ledomatic_config_num_fonts, ledomatic_config_num_segments);

    lomd->config.num_fonts = ledomatic_config_num_fonts;
    lomd->config.fonts =
        calloc(sizeof(*(lomd->config.fonts)), ledomatic_config_num_fonts);

    lomd->config.num_segments = ledomatic_config_num_segments;
    lomd->config.segment_configs =
        calloc(sizeof(*(lomd->config.segment_configs)), lomd->config.num_segments);

    ledomatic_config_cur_font_index = 0;
    ledomatic_config_cur_segment_index = 0;

    ledomatic_config_in_segment = false;
    ledomatic_config_segment_field_count = 0;
    ledomatic_config_cur_segment.x = 0;
    ledomatic_config_cur_segment.y = 0;
    ledomatic_config_cur_segment.width = 0;
    ledomatic_config_cur_segment.height = 0;
    ledomatic_config_cur_segment.font_index = 0;

    if (ini_parse(lomd->args.config_file, &ledomatic_config_handler_p2, &lomd->config) < 0) {
        return false;
    }
    return true;
}

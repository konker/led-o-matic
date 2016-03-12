/**
 * Konker's LED matrix library
 *
 * A library for driving a red LED matrix
 *
 * Copyright 2015, Konrad Markus <konker@luxvelocitas.com>
 *
 * This file is part of konker_led_matrix.
 *
 * konker_led_matrix is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * konker_led_matrix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with konker_led_matrix.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __LEDOMATICD_CONFIG_H__
#define __LEDOMATICD_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

// Forward declare ledomaticd
typedef struct ledomaticd ledomaticd;

typedef struct {
    int16_t x;
    int16_t y;
    uint16_t width;
    uint16_t height;
    uint8_t font_index;

} ledomatic_config_segment;

typedef struct {
    // UDP listener
    char *udp_host;
    char *udp_port;

    // matrix
    uint16_t matrix_width;
    uint16_t matrix_height;
    uint8_t a;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t oe;
    uint8_t r1;
    uint8_t stb;
    uint8_t clk;

    // Fonts
    uint8_t num_fonts;
    char **fonts;

    // Segments
    uint8_t num_segments;
    ledomatic_config_segment *segment_configs;

} ledomatic_config;

int ledomatic_config_handler(void* user, const char* section,
                             const char* name, const char* value);
bool ledomatic_config_parse(ledomaticd * const lomd);


#ifdef __cplusplus
}
#endif

#endif // __LEDOMATICD_CONFIG_H__




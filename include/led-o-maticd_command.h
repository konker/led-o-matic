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

#ifndef __LEDOMATICD_COMMAND_H__
#define __LEDOMATICD_COMMAND_H__

#include "led-o-maticd.h"


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

#ifdef __cplusplus
extern "C" {
#endif

void handle_command(ledomaticd * const lomd, char *buf);

#ifdef __cplusplus
}
#endif

#endif // __LEDOMATICD_COMMAND_H__




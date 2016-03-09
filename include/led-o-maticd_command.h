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
// Stop all scrolling
#define LEDOMATIC_CMD_STOP "stop"
// Resume all scrolling
#define LEDOMATIC_CMD_START "start"
// Switch display off
#define LEDOMATIC_CMD_OFF "off"
// Switch display on
#define LEDOMATIC_CMD_ON "on"
// Reverse display colors
#define LEDOMATIC_CMD_REVERSE "reverse"
// Clear the display
#define LEDOMATIC_CMD_CLEAR "clear"
// Set text1
#define LEDOMATIC_CMD_TEXT "text:%" LEDOMATIC_TOSTRING(KLM_TEXT_LEN) "[^\t\n]"
// Set text1 scroll speed
#define LEDOMATIC_CMD_SPEED "speed:%f"
// Set text1 position
#define LEDOMATIC_CMD_POSITION "position:%f"

#ifdef __cplusplus
extern "C" {
#endif

void handle_command(ledomaticd * const lomd, char *buf);

#ifdef __cplusplus
}
#endif

#endif // __LEDOMATICD_COMMAND_H__




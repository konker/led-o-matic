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

#ifndef __LEDOMATICD_MATRIX_H__
#define __LEDOMATICD_MATRIX_H__

#include "led-o-maticd.h"

#define LEDOMATIC_DEFAULT_MATRIX_WIDTH 32
#define LEDOMATIC_DEFAULT_MATRIX_HEIGHT 16

#ifdef __cplusplus
extern "C" {
#endif

bool ledomatic_matrix_init(ledomaticd * const lomd);
void ledomatic_matrix_exit(ledomaticd * const lomd);
void *ledomatic_matrix_scanner_thread(void *arg);

#ifdef __cplusplus
}
#endif

#endif // __LEDOMATICD_MATRIX_H__


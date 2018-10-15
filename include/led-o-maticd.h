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

#ifndef __LEDOMATICD_H__
#define __LEDOMATICD_H__

#include <stdio.h>
#include <errno.h>

#include <klm_matrix.h>
#include "led-o-maticd_config.h"
#include "led-o-maticd_args.h"

#define LEDOMATIC_LOG(l, ...) fprintf((l).logfp, __VA_ARGS__); \
                              fflush((l).logfp);

#define LEDOMATIC_ONE_MILLION 1000000
#define LEDOMATIC_ONE_THOUSAND 1000
#define LEDOMATIC_NOW_MICROSECS(var, time_spec_var) \
        clock_gettime(CLOCK_REALTIME, &time_spec_var); \
        var =  time_spec_var.tv_sec * LEDOMATIC_ONE_MILLION; \
        var += time_spec_var.tv_nsec / LEDOMATIC_ONE_THOUSAND; \

#define LEDOMATIC_MAXBUFLEN 1024
#define LEDOMATIC_MAIN_LOOP_PERIOD_NANOS 10000000

#define LEDOMATIC_STRINGIFY(x) #x
#define LEDOMATIC_TOSTRING(x) LEDOMATIC_STRINGIFY(x)

// 1 thousand => 1 millisecond
#define LEDOMATIC_TICK_PERIOD_MICROS 400


#ifdef __cplusplus
extern "C" {
#endif

typedef struct ledomaticd {
    ledomatic_args args;
    ledomatic_config config;
    FILE *logfp;
    int sockfd;
    volatile bool running;
    klm_matrix *matrix;

    struct timespec now_t;
    int64_t micros_0;
    int64_t micros_1;

} ledomaticd;

#ifdef __cplusplus
}
#endif

#endif // __LEDOMATICD_H__



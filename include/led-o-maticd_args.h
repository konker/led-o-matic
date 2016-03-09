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

#ifndef __LEDOMATICD_ARGS_H__
#define __LEDOMATICD_ARGS_H__

#define LEDOMATIC_USAGE "led-o-maticd -c <config file> [-l <log file>] [-p <pid file>] [-h]\n"

#define LEDOMATIC_DEFAULT_CONFIG_FILE "led-o-matic.ini"
#define LEDOMATIC_DEFAULT_LOG_FILE "/var/log/led-o-maticd.log"
#define LEDOMATIC_DEFAULT_PID_FILE "/var/run/led-o-maticd.pid"

#ifdef __cplusplus
extern "C" {
#endif

// Forward declare ledomaticd
typedef struct ledomaticd ledomaticd;

typedef struct {
    char *config_file;
    char *log_file;
    char *pid_file;

} ledomatic_args;


int ledomatic_args_parse(int argc, char *argv[], ledomaticd * const lomd);

#ifdef __cplusplus
}
#endif

#endif // __LEDOMATICD_ARGS_H__





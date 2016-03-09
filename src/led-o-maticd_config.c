#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#include <ini.h>

#include "led-o-maticd.h"
#include "led-o-maticd_config.h"

/**
  // ----------------------------------------------------------------------
  Config file handler
*/
int ledomatic_config_handler(void* user, const char* section,
                             const char* name, const char* value)
{
    ledomatic_config* config = (ledomatic_config*)user;

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("matrix", "width")) {
        config->matrix_width = atoi(value);
    }
    else if (MATCH("matrix", "height")) {
        config->matrix_height = atoi(value);
    }
    else {
        //[FIXME] LEDOMATIC_LOG("Warning: Unknown config item: %s, %s\n", section, name);
    }
    return 1;
}

bool ledomatic_config_parse(ledomaticd * const lomd) {
    if (ini_parse(lomd->args.config_file, &ledomatic_config_handler, &lomd->config) < 0) {
        return false;
    }
    return true;
}

#ifndef C_CONFIGFILE_H_H
#define C_CONFIGFILE_H_H

#include <libconfig.h>
#include "logger.h"

int initConfigFile(config_t *cfg, const char *cfgfilename);
int readConfigFile(config_t *cfg, const char *cfgfilename);

#endif

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "configfile.h"


int initConfigFile(config_t *cfg, const char *cfgfilename){
    if(access(cfgfilename, F_OK)==-1){
        SLOGL_vprint(SLOGL_LVL_ERROR,
        "Le fichier de configuration %s \
        nexiste pas", cfgfilename);
        return -1;
    }
    if (access(cfgfilename, R_OK)==-1){
        SLOGL_vprint(SLOGL_LVL_ERROR,
        "Le fichier de configuration %s \
        nest pas accessible en lecture", cfgfilename);
        return -1;
    }  

    config_init(cfg);
    return 1;
}

int readConfigFile(config_t *cfg, const char *cfgfilename){ 
    if(! config_read_file(cfg, cfgfilename)){ 
        SLOGL_vprint(SLOGL_LVL_ERROR,
            "Erreur de syntaxe dans %s (ligne %d) -> %s\n",
            config_error_file(cfg), config_error_line(cfg),
            config_error_text(cfg));
        return -1;
    }
    SLOGL_vprint(SLOGL_LVL_INFO,
        "Utilisation du fichier de configuration %s", cfgfilename);
    return 1;
}

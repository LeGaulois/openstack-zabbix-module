#include <stdio.h>
#include <unistd.h>
#include <libconfig.h>
#include <json-c/json_object.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "logger.h"
#include "configfile.h"
#include "curl-request.h"
#include "zbx-instance.h"
#include "sender.h"
#include "list.h"
#include "zbx-hosts.h"
#include "zbx-interface.h"
#include "myjson.h"
#include "tools.h"

SLOGL_level programLevel = SLOGL_LVL_DEBUG;

int main(){
    json_object* all = NULL;
    fprintf(stderr,"%d\n",SLOGL_init("test"));
    zbxinstance *zbx = NULL;
    zbxhost *host = NULL;
    zbxinterface *eth = NULL;
    
    
    
    list_t *groups = list_new();
    list_t *interfaces = list_new();
    
    prepare_zbx_instance(&zbx,"default.cfg");
    //fprintf(stderr,"%s\n", zbxinstance_get_url(zbx));
    
    
    zbxinterface_create(&eth, NULL, "192.168.0.2", 1, 10050, 1, 1);
    list_rpush(interfaces, list_node_new(eth));
    list_rpush(groups, list_node_new(4));
    
    zbxhost_create(&host, "test","test", "test", NULL, 1, 1, 1, NULL, NULL, NULL, NULL, groups, interfaces,NULL);
    
    if(zbx_connection(zbx)==1){
        //fprintf(stdout,"Authentification success: %s\n", zbx->auth);
        //zbxhost_add(zbx, host);
        //fprintf(stdout,"Add success: %s\n", host->hostid);
        //all = json_object_new_array();
        //all = getAllHosts(zbx);
        //fprintf(stderr,"%s\n", json_object_get_string(all));
        //json_object_free(&all);
        //zbxhost_delete(zbx, host);
    }    
    
    zbxhost_destroy(&host);
    zbxinterface_destroy(&eth);
    
    destroy_zbxinstance(&zbx);
    
    SLOGL_quit();
    return EXIT_SUCCESS;
}

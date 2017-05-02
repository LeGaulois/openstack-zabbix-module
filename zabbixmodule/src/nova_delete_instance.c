#include <stdlib.h>
#include <stdio.h>
#include <json-c/json.h>
#include <json-c/json_object.h>
#include <json-c/json_object_private.h>
#include <assert.h>
#include <string.h>

#include "configfile.h"
#include "zbx-hosts.h"
#include "zbx-interface.h"
#include "myjson.h"
#include "tools.h"
#include "amqp_listen.h"



void nova_delete_instance(zbxinstance *zbx, json_object *data){
    
    json_object *jtmp, *result;
    const char *hostname, *hostid;
    
    /*On verifie le status*/
    if((json_object_object_get_ex(data,"state", &jtmp)==1)&&
        (strcmp(json_object_get_string(jtmp),"deleted")!=0))
    {
            fprintf(stderr,"OUPS\n");
            return;
    }
    
    json_object_object_get_ex(data,"host_name", &jtmp);
    hostname = json_object_get_string(jtmp);
    
    result = searchHostIDByHostname(zbx, hostname);
    
    if(result==NULL){
        return ;
    }
    fprintf(stderr,"Result: \n%s\n", json_object_to_json_string(result));
    if(json_object_array_length(result)==0){
        //TODO: logger
    }
    
    else{
        jtmp = json_object_array_get_idx(result,0);
        json_object_object_get_ex(jtmp, "hostid", &jtmp);
        hostid = json_object_get_string(jtmp);
        zbxhost_delete_byid(zbx, hostid);
    }
}

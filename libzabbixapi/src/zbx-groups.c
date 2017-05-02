#include <stdio.h>
#include <string.h>
#include <json-c/json_object.h>
#include <json-c/json_object_private.h>
#include <libconfig.h>
#include "logger.h"
#include "list.h"
#include "zbx-instance.h"
#include "zbx-interface.h"
#include "curl-request.h"
#include "sender.h"
#include "zbx-groups.h"

json_object* getAllGroups(zbxinstance *zbx){
    int ret;
    json_object *jret = NULL;
    curlrequest *cr = NULL;
    json_object *params = NULL,*output = NULL, *tmp = NULL;
    
    
    params = json_object_new_object();
    /*OUTPUT*/
    output = json_object_new_array();
    tmp = json_object_new_string("groupid");
    json_object_array_add(output, tmp);
    tmp = json_object_new_string("name");
    json_object_array_add(output, tmp);
    json_object_object_add(params, "output", output);
    
    prepareJsonData(params, "hostgroup.get", zbx, &cr);
    ret = sendPost(zbx, cr);

    if(ret==1){
        json_object_object_get_ex(cr->response,"result", &tmp);
        jret = json_object_get(tmp);
    }

    curl_request_destroy(&cr);
    return jret;
}

int checkGroupHostByName(zbxinstance *zbx, char *groupname){
    int ret;
    curlrequest *cr = NULL;
    json_object *params = NULL,*output = NULL,
        *filter = NULL, *name = NULL, *tmp = NULL;
    
    
    params = json_object_new_object();
    /*OUTPUT*/
    output = json_object_new_string("extend");
    json_object_object_add(params, "output", output);
    
    /*FILTER*/
    filter = json_object_new_object();
    name = json_object_new_array();
    tmp = json_object_new_string(groupname);
    json_object_array_add(name, tmp);
    json_object_object_add(filter, "name", name);
    json_object_object_add(params, "filter", filter);
    
    prepareJsonData(params, "hostgroup.get", zbx, &cr);
    ret = sendPost(zbx, cr);

    if(ret==1){
        json_object_object_get_ex(cr->response,"result", &tmp);
        
        if(json_object_array_length(cr->response)==0) ret=0;
    }
    
    else{
        //TODO: message erreur
    }

    curl_request_destroy(&cr);
    return ret;
} 

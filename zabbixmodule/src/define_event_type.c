#include "rabbit_functions.h"

int define_nova_event_type(json_object *event){
    int ret = -1;
    const char *type;
    
    type = json_object_get_string(event);
    
    if(strcmp(type, "instance.create.end")==0){
        ret = INSTANCE_CREATE_END;
    }
    
    else if(strcmp(type, "instance.delete.end")==0){
        ret = INSTANCE_DELETE_END;
    }
    
    return ret;
}


int define_zabbix_event_type(json_object *event){
    int ret = -1;
    const char *type;
    
    type = json_object_get_string(event);
    
    if(strcmp(type, "zabbix.list.groups")==0){
        ret = ZABBIX_LIST_GROUPS;
    }
    
    else if(strcmp(type, "zabbix.list.templates")==0){
        ret = ZABBIX_LIST_TEMPLATES;
    }
    
    else if(strcmp(type, "zabbix.create.host")==0){
        ret = ZABBIX_CREATE_HOST;
    }
    
    return ret;
}

int define_zabbix_message_type(json_object *messagetype){
    int ret = -1;
    const char *type;
    
    type = json_object_get_string(messagetype);
    
    if(strcmp(type, "zabbix.request")==0){
        ret = ZABBIX_REQUEST;
    }
    
    else if(strcmp(type, "zabbix.response")==0){
        ret = ZABBIX_RESPONSE;
    }
    
    return ret;
}

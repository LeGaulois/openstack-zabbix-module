#include <stdio.h>
#include <string.h>
#include <json-c/json_object.h>
#include "logger.h"
#include "list.h"
#include "zbx-interface.h"



int zbxinterface_create(zbxinterface **eth, char *dns, char *ip, unsigned int main, unsigned int port, 
    unsigned int type, unsigned int useip)
{
    int ret = 1;
    if(zbxinterface_init(eth) == -1) return -1;
    
    if((useip == VIA_DNS)||(useip == VIA_IP)){
        (*eth)->useip = useip;
    }
    if((type == TYPE_AGENT)||(type == TYPE_SNMP)||
        (type == TYPE_IPMI)||(type == TYPE_JMX)){
        (*eth)->type = type;
    }
    
    if((port>=1)&&(port<=65535)){
        (*eth)->port = port;
    }
    if((main==1)||(main==0))(*eth)->main = main;
    
    
    if((*eth)->useip == VIA_DNS){
        if((!dns)&&(!ip)){
            SLOGL_vprint(SLOGL_LVL_ERROR,"La connexion a ete definie\
             via DNS mais aucun nom DNS na ete fixe.");
             ret = -2;
        }
        else if((!dns)&&(ip)){
            SLOGL_vprint(SLOGL_LVL_ERROR,"La connexion a ete definie\
             via DNS mais aucun nom DNS na ete fixe. Utilisation\
              de l'adresse IP %s pour la connexion", ip);
            (*eth)->useip = VIA_IP;
            ret = -3; 
        }
        else{
            free((*eth)->dns);
            (*eth)->dns = strndup(dns, strlen(dns));
        }
    }
    if((*eth)->useip == VIA_IP){
        if((!ip)&&(!dns)){
            SLOGL_vprint(SLOGL_LVL_ERROR,"La connexion a ete definie\
             via IP mais aucune adresse na ete fixee.");
             ret = -3;
        }
        else if((!ip)&&(dns)){
            SLOGL_vprint(SLOGL_LVL_ERROR,"La connexion a ete definie\
             via IP mais aucune adresse na ete fixee. Utilisation\
              du hostname %s pour la connexion", dns);
            (*eth)->useip = VIA_DNS;
            ret = -4; 
        }
        else{
            free((*eth)->ip);
            (*eth)->ip = strndup(ip, strlen(ip));
        }
    }
    return ret;
}


int zbxinterface_init(zbxinterface **eth){
    *eth = calloc(1, sizeof(zbxinterface));
    if(! *eth) return -1;
    
    (*eth)->dns    = strndup("",1);
    (*eth)->hostid = NULL;
    (*eth)->ip     = strndup("", 1);
    (*eth)->main   = 1;
    (*eth)->port   = 10050;
    (*eth)->type   = TYPE_AGENT;
    (*eth)->useip  = VIA_IP;

    return 1;
}

void zbxinterface_destroy(zbxinterface **eth){
    if(*eth == NULL) return;
    if((*eth)->dns) free((*eth)->dns);
    if((*eth)->hostid) free((*eth)->hostid);
    if((*eth)->ip) free((*eth)->ip);
    free(*eth);
}


json_object* zbxinterface_to_json(zbxinterface *eth){
    json_object *jeth = NULL, *dns = NULL,
        *ip=NULL, *principal=NULL,
        *port=NULL, *type=NULL, *useip=NULL;
    
    jeth = json_object_new_object();
    
    if (eth->dns){
        dns = json_object_new_string(eth->dns);
        json_object_object_add(jeth,"dns", dns);
    }
    if (eth->ip){
        ip = json_object_new_string(eth->ip);
        json_object_object_add(jeth,"ip", ip);
    }
    
    principal = json_object_new_int(eth->main);
    json_object_object_add(jeth,"main", principal);

    port = json_object_new_int(eth->port);
    json_object_object_add(jeth,"port", port);
    
    type = json_object_new_int(eth->type);
    json_object_object_add(jeth,"type", type);
    useip = json_object_new_int(eth->useip);
    json_object_object_add(jeth,"useip", useip);
    
    return jeth;
}


json_object* zbxinterface_list_to_json(list_t *leth){
    json_object *jeths = json_object_new_array();
    list_iterator_t *it = list_iterator_new(leth, LIST_HEAD);
    list_node_t *node = NULL;
    json_object *jeth = NULL;
    
    while ((node = list_iterator_next(it))) {
        jeth = zbxinterface_to_json(node->val);
        json_object_array_add(jeths,jeth);
    }
    list_iterator_destroy(it);
    return jeths;
}

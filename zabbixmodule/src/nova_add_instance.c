#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
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



void add_from_zabbix_queue(sharet *share, json_object *data){
    /*
    Cette fonction permet de créer un host ZABBIX et de l'ajouter
    à la liste des hosts en cours de création
    */
    char *hostname;
    int nb_group = 0, nb_templates=0, i=0, groupid=0, templateid=0;
    json_object *jtmp, *jgroup, *jtemplate;
    zbxhost *host = NULL;
    list_t *groups = NULL, *templates = NULL;
    
    groups = list_new();
    templates = list_new();
    
    /*Verification du JSON recu*/
    if(data==NULL){
        SLOGL_vprint(SLOGL_LVL_ERROR,
        "%s, le JSON recu est vide ou est mal forme (%s)",__FUNCTION__, json_object_get_string(data));
        return;
    }
    
    /*Recuperation du hostname*/
    json_object_object_get_ex(data,"name", &jtmp);
    if(json_object_get_type(jtmp)!=json_type_string){
        SLOGL_vprint(SLOGL_LVL_ERROR,"%s, le hostname n'est pas valide (doit etre un string): %s",__FUNCTION__, json_object_to_json_string(jtmp));
        return;
    }
    
    hostname = json_object_get_string(jtmp);
    SLOGL_vprint(SLOGL_LVL_INFO,"%s, hostname= %s",__FUNCTION__, hostname);
    
    
    /*Récuperation de la liste des groupsid*/
    json_object_object_get_ex(data,"groupid", &jtmp);
    
    if(json_object_get_type(jtmp)!=json_type_array){
        SLOGL_vprint(SLOGL_LVL_ERROR,"%s, la liste des groupes n'est pas valide (doit etre un array): %s",__FUNCTION__, json_object_to_json_string(jtmp));
        return;
    }
    
    nb_group = json_object_array_length(jtmp);
    
    for(i=0; i<nb_group;i++){
        jgroup = json_object_array_get_idx(jtmp, i);
        
        if(json_object_get_type(jgroup)!=json_type_int){
            SLOGL_vprint(SLOGL_LVL_ERROR,"%s, le groupid doit etre un int: %s",__FUNCTION__, json_object_to_json_string(jgroup));
        }
        else{
            groupid = (int)(json_object_get_int(jgroup));
            list_rpush(groups, list_node_new(groupid));
        }
    }
    
    /*Récuperation de la liste des templates*/
    json_object_object_get_ex(data,"templateid", &jtmp);
    
    if(json_object_get_type(jtmp)!=json_type_array){
        SLOGL_vprint(SLOGL_LVL_ERROR,"%s, la liste des templates n'est pas valide (doit etre un array): %s",__FUNCTION__, json_object_to_json_string(jtmp));
        return;
    }
    
    nb_templates = json_object_array_length(jtmp);
    
    for(i=0; i<nb_templates;i++){
        jtemplate = json_object_array_get_idx(jtmp, i);
        
        if(json_object_get_type(jtemplate)!=json_type_int){
            SLOGL_vprint(SLOGL_LVL_ERROR,"%s, le templateid doit etre un int: %s",__FUNCTION__, json_object_to_json_string(jgroup));
        }
        else{
            templateid = (int)(json_object_get_int(jtemplate));
            list_rpush(templates, list_node_new(templateid));
        }
    }
    
    zbxhost_create(&host, hostname, hostname, hostname, NULL, 1, 1, 1, NULL, NULL, NULL, NULL, groups, NULL,templates);
    
    /*On ajoute le host zabbix a la 
    fin de la liste
    => acceleration des futures recherches
    (FIFO)
    */
    pthread_mutex_lock(share->mutex_list);
    list_rpush(share->list_hosts, list_node_new(host));
    pthread_mutex_unlock(share->mutex_list);

}

void nova_add_instance(sharet *share, json_object *data){
    json_object *jtmp;
    list_node_t *node;
    list_iterator_t *it = NULL;
    char *instance_name, *instance_ip, *instance_status, *hostname=NULL;
    zbxhost *host = NULL;
    zbxinterface *eth = NULL;
    list_t *groups = NULL;
    list_t *interfaces = NULL;
    int find=0;
  
    /*On verifie que la creation de host sur nova est OK*/
    if((json_object_object_get_ex(data,"power_state", &jtmp)==1)&&
        (strcmp(json_object_get_string(jtmp),"running")!=0))
    {
            return;
    }
            
    instance_status = json_object_get_string(jtmp);
    json_object_object_get_ex(data,"host_name", &jtmp);
    instance_name = json_object_get_string(jtmp);   
    json_object_object_get_ex(data, "ip_addresses", &jtmp);
            
    jtmp = json_object_array_get_idx(jtmp,0);       
    json_object_object_get_ex(jtmp, "nova_object.data", &jtmp);
    json_object_object_get_ex(jtmp, "address", &jtmp);
    instance_ip = json_object_get_string(jtmp);
            
    /*On cherche dans la liste des hosts en cours
    si il y a une correspondance
    */
    pthread_mutex_lock(share->mutex_list);
    it = list_iterator_new(share->list_hosts, LIST_HEAD);
    
    while ((node = list_iterator_next(it))) {
        hostname = ((zbxhost*)(node->val))->host;

        if(strcmp(hostname, instance_name)==0){
            host = node->val;
            find = 1;
            break;
        }
    }
    list_iterator_destroy(it);
    
    /*On cree une interface pour le host*/
    interfaces = list_new();
    zbxinterface_create(&eth, NULL, instance_ip, 1, 10050, 1, 1);
    list_rpush(interfaces, list_node_new(eth));
    
    
    if(host==NULL){
        groups = list_new();
        list_rpush(groups, list_node_new(4));
        zbxhost_create(&host, instance_name,instance_name, instance_name, NULL, 1, 1, 1, NULL, NULL, NULL, NULL, groups, interfaces,NULL);
    }
    else{
        host->interfaces = interfaces;
    }
    
    zbxhost_add(share->zbx, host);
    zbxhost_destroy(&host);
    zbxinterface_destroy(&eth);
    
    if(find==1){
        list_remove(share->list_hosts, node);
    }
    pthread_mutex_unlock(share->mutex_list);
}

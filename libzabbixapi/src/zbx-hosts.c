#include <stdio.h>
#include <string.h>
#include <json-c/json_object.h>
#include <json-c/json_object_private.h>
#include <libconfig.h>
#include "logger.h"
#include "zbx-hosts.h"


/*
Création d'un hôte
Valeurs de retour:
    -4 -> tls_psk ou tls_psk_identity non defini
    -3 -> tls_issuer ou tls_subject non  defini
    -2 -> aucun host defini
    -1 -> erreur d'allocation memoire
     1 -> OK
*/
int zbxhost_create(zbxhost **zbxh, char *host, char *description, char *name, char *proxy_host_id,unsigned int status,unsigned int tls_connect, unsigned int tls_accept,char * tls_issuer, char *tls_subject,char *tls_psk_identity, char *tls_psk,list_t *groups, list_t *interfaces, list_t *templates){
    
    int ret = 1;
    
    if(zbxhost_init(zbxh) == -1) return -1;
    
    if(host) (*zbxh)->host = strndup(host, strlen(host));
    else{
        SLOGL_vprint(SLOGL_LVL_ERROR,"Le host na pas ete defini\
            (parametre  obligatoire)");
        return -2;
    }
    if(description) (*zbxh)->description=strndup(description, strlen(description));
    if(name) (*zbxh)->name = strndup(name, strlen(name));
    if(proxy_host_id) (*zbxh)->proxy_host_id = strndup(proxy_host_id, strlen(proxy_host_id));
    
    if((status == MONITORED)||(status == UNMONITORED)) (*zbxh)->status=status;
    if((tls_connect == NOENCRYPTION)||(tls_connect == PSK)||(tls_connect==CERTIFICATE)){ 
        (*zbxh)->tls_connect = tls_connect;
    }
    if((tls_accept == NOENCRYPTION)||(tls_accept == PSK)||(tls_accept==CERTIFICATE)){ 
        (*zbxh)->tls_accept = tls_accept;
    }    
    
    if(tls_accept==CERTIFICATE){
        if((!tls_issuer)||(!tls_subject)){
            SLOGL_vprint(SLOGL_LVL_ERROR,
                "Le host %s accepte les connections via certificat,\
                 mais les parametres tls_issuer ou tls_subject n'ont\
                  pas ete defini. Le parametre 'accept' a ete redefini\
                 a UNENCRYPTED");
            (*zbxh)->tls_accept = NOENCRYPTION;
            ret = -3;
        }
        else{
            (*zbxh)->tls_issuer = strndup(tls_issuer, strlen(tls_issuer));
            (*zbxh)->tls_subject = strndup(tls_subject, strlen(tls_subject));
        }
    }
    else if(tls_accept==PSK){
         if((!tls_psk)||(!tls_psk_identity)){
            SLOGL_vprint(SLOGL_LVL_ERROR,
                "Le host %s accepte les connections via PSK,\
                 mais les parametres tls_psk et/ou tls_psk_identity\
                  n'ont pas ete defini.Le parametre 'accept'\
                   a ete redefini\
                 a UNENCRYPTED");
            (*zbxh)->tls_accept = NOENCRYPTION;
            ret = -4;
        }
        else{
            (*zbxh)->tls_psk_identity = strndup(tls_psk_identity, strlen(tls_psk_identity));
            (*zbxh)->tls_psk = strndup(tls_psk, strlen(tls_psk));
        }
    }
    
    (*zbxh)->groups = groups;
    (*zbxh)->interfaces = interfaces;
    (*zbxh)->templates = templates;
    return ret;
}


int zbxhost_init(zbxhost **host){
    *host = calloc(1, sizeof(zbxhost));
    if(! *host) return -1;
    
    (*host)->hostid           = NULL;
    (*host)->host             = NULL;
    (*host)->description      = NULL;
    (*host)->name             = NULL;
    (*host)->proxy_host_id    = NULL;
    (*host)->status           = 0;
    (*host)->tls_connect      = 1;
    (*host)->tls_accept       = 1;
    (*host)->tls_issuer       = NULL;
    (*host)->tls_subject      = NULL;
    (*host)->tls_psk_identity = NULL;
    (*host)->tls_psk          = NULL;
    (*host)->groups           = NULL;
    (*host)->interfaces       = NULL;
    (*host)->templates        = NULL;
    return 1;
}

void zbxhost_destroy(zbxhost **host){
    if(*host == NULL) return;
    if((*host)->hostid) free((*host)->hostid);
    if((*host)->host) free((*host)->host);
    if((*host)->description) free((*host)->description);
    if((*host)->name) free((*host)->name);
    if((*host)->proxy_host_id) free((*host)->proxy_host_id);
    if((*host)->tls_issuer) free((*host)->tls_issuer);
    if((*host)->tls_subject) free((*host)->tls_subject);
    if((*host)->tls_psk_identity) free((*host)->tls_psk_identity);
    if((*host)->tls_psk) free((*host)->tls_psk);
    if((*host)->groups) list_destroy((*host)->groups);
    if((*host)->interfaces) list_destroy((*host)->interfaces);
    if((*host)->templates) list_destroy((*host)->templates);
    free(*host);
}


json_object* zbxhost_to_json(zbxhost* zhost){
    json_object *jhost = NULL, *host = NULL,
        *description=NULL, *name=NULL,
        *status=NULL,
        *tls_connect=NULL, *tls_accept=NULL,
        *proxy=NULL,
        *tls_issuer=NULL, *tls_subject=NULL,
        *tls_psk_identity=NULL, *tls_psk=NULL;
    
    jhost = json_object_new_object();
    
    host = json_object_new_string(zhost->host);
    json_object_object_add(jhost,"host", host);
    
    if (zhost->description){
        description = json_object_new_string(zhost->description);
        json_object_object_add(jhost,"description", description);
    }
    if(zhost->name){
        name = json_object_new_string(zhost->name);
        json_object_object_add(jhost,"name", name);
    }
    if(zhost->proxy_host_id){
        proxy = json_object_new_string(zhost->proxy_host_id);
        json_object_object_add(jhost,"proxy_host_id", proxy);
    }
    
    status = json_object_new_int(zhost->status);
    json_object_object_add(jhost,"status", status);
    
    tls_connect = json_object_new_int(zhost->tls_connect);
    json_object_object_add(jhost,"tls_connect", tls_connect);
    
    tls_accept = json_object_new_int(zhost->tls_accept);
    json_object_object_add(jhost,"tls_accept", tls_accept);
    
    if(zhost->tls_issuer){
        tls_issuer = json_object_new_string(zhost->tls_issuer);
        json_object_object_add(jhost,"tls_issuer", tls_issuer);
    }
    if(zhost->tls_subject){
        tls_subject = json_object_new_string(zhost->tls_subject);
        json_object_object_add(jhost,"tls_subject", tls_subject);
    }
    if(zhost->tls_psk_identity){
        tls_psk_identity = json_object_new_string(zhost->tls_psk_identity);
        json_object_object_add(jhost,"tls_psk_identity", tls_psk_identity);
    }
    if(zhost->tls_psk){
        tls_psk = json_object_new_string(zhost->tls_psk);
        json_object_object_add(jhost,"tls_psk", tls_psk);
    }
    if(zhost->groups){
        json_object_object_add(jhost,"groups", zbxhost_listgroups_to_json(zhost));
    }
    
    if(zhost->templates){
        json_object_object_add(jhost,"templates", zbxhost_listtemplates_to_json(zhost));
    }
    json_object_object_add(jhost,"interfaces",zbxinterface_list_to_json(zhost->interfaces));
    
    return jhost;
}


json_object* zbxhost_listgroups_to_json(zbxhost *host){
    json_object *jgroups = json_object_new_array();
    list_iterator_t *it = list_iterator_new(host->groups, LIST_HEAD);
    list_node_t *node = NULL;
    json_object *group = NULL, *groupid=NULL;
    
    while ((node = list_iterator_next(it))) {
        group = json_object_new_object();
        groupid = json_object_new_int(node->val);
        json_object_object_add(group,"groupid", groupid);
        json_object_array_add(jgroups,group);
    }
    list_iterator_destroy(it);
    return jgroups;   
}

json_object* zbxhost_listtemplates_to_json(zbxhost *host){
    json_object *jtemplates = json_object_new_array();
    list_iterator_t *it = list_iterator_new(host->templates, LIST_HEAD);
    list_node_t *node = NULL;
    json_object *template = NULL, *templateid=NULL;
    
    while ((node = list_iterator_next(it))) {
        template = json_object_new_object();
        templateid = json_object_new_int(node->val);
        json_object_object_add(template,"templateid", templateid);
        json_object_array_add(jtemplates,template);
    }
    list_iterator_destroy(it);
    return jtemplates;   
}


int zbxhost_add(zbxinstance *zbx, zbxhost *host){
    int ret;
    curlrequest *cr = NULL;
    json_object *params = NULL, *jhostid = NULL;
    const char *hostid;
    size_t len_hostid;
    
    /*Construction du json a envoyer*/
    params = zbxhost_to_json(host);

    prepareJsonData(params, "host.create", zbx, &cr);
    ret = sendPost(zbx, cr);
    
    if(ret==1){
        json_object_object_get_ex(cr->response, "result", &jhostid);
        json_object_object_get_ex(jhostid, "hostids", &jhostid);
        hostid = json_object_to_json_string(json_object_array_get_idx(jhostid,0));
        len_hostid = strlen(hostid)+1;
        host->hostid = calloc(len_hostid-2, sizeof(char));
        strncpy(host->hostid, hostid+1, len_hostid-3);
        SLOGL_vprint(SLOGL_LVL_INFO,
                "Le hote %s a bien ete ajoute au Zabbix", host->host);
    }
    else{
        SLOGL_vprint(SLOGL_LVL_ERROR,
                "Erreur lors de l'ajout de le hote %s: %s"
                , host->host,json_object_to_json_string(cr->response));
    }
    
    ret=-1;
    curl_request_destroy(&cr);
    return ret;  
}

int zbxhost_delete(zbxinstance *zbx,zbxhost *host){
    int ret;
    curlrequest *cr = NULL;
    json_object *params = NULL, *hostid = NULL;
    
    /*Construction du json a envoyer*/
    params = json_object_new_array();
    hostid = json_object_new_string(host->hostid);
    json_object_array_add(params, hostid);
    
    prepareJsonData(params, "host.delete", zbx, &cr);
    ret = sendPost(zbx, cr);

    if(ret==1){
        SLOGL_vprint(SLOGL_LVL_INFO,
                "Le hote %s a bien ete supprime de zabbix'", host->host);
    }
    else{
        SLOGL_vprint(SLOGL_LVL_ERROR,
                "Erreur lors de la suppresion de %s'", host->host);
    }

    curl_request_destroy(&cr);
    return ret;  
}

json_object* getAllHosts(zbxinstance *zbx){
    int ret;
    json_object *jret = NULL;
    curlrequest *cr = NULL;
    json_object *params = NULL,*output = NULL, *select=NULL, *tmp = NULL;
    
    
    params = json_object_new_object();
    /*OUTPUT*/
    output = json_object_new_array();
    tmp = json_object_new_string("host");
    json_object_array_add(output, tmp);
    tmp = json_object_new_string("error");
    json_object_array_add(output, tmp);
    tmp = json_object_new_string("available");
    json_object_array_add(output, tmp);
    tmp = json_object_new_string("status");
    json_object_array_add(output, tmp);
    json_object_object_add(params, "output", output);
    
    /*SELECT*/
    select = json_object_new_array();
    tmp = json_object_new_string("ip");
    json_object_array_add(select, tmp);
    json_object_object_add(params, "selectInterfaces", select);
    
    prepareJsonData(params, "host.get", zbx, &cr);
    ret = sendPost(zbx, cr);

    if(ret==1){
        json_object_object_get_ex(cr->response,"result", &tmp);
        jret = json_object_get(tmp);
    }

    curl_request_destroy(&cr);
    return jret;
}


json_object* searchHostIDByHostname(zbxinstance *zbx, const char *hostname){
    int ret;
    json_object *jret = NULL;
    curlrequest *cr = NULL;
    json_object *params = NULL,*output = NULL, *host=NULL, \
                *filter=NULL, *tmp = NULL;
    
    
    params = json_object_new_object();
    
    /*OUTPUT*/
    output = json_object_new_array();
    tmp = json_object_new_string("hostid");
    json_object_array_add(output, tmp);
    json_object_object_add(params, "output", output);
    
    /*Filter*/
    filter = json_object_new_object();
    host = json_object_new_array();
    tmp = json_object_new_string(hostname);
    json_object_array_add(host, tmp);
    json_object_object_add(filter, "host", host);
    json_object_object_add(params, "filter", filter);
    
    prepareJsonData(params, "host.get", zbx, &cr);
    ret = sendPost(zbx, cr);

    if(ret==1){
        json_object_object_get_ex(cr->response,"result", &tmp);
        jret = json_object_get(tmp);
    }

    curl_request_destroy(&cr);
    return jret;
}


int zbxhost_delete_byid(zbxinstance *zbx,const char *hostid){
    int ret;
    curlrequest *cr = NULL;
    json_object *params = NULL, *host = NULL;
    
    /*Construction du json a envoyer*/
    params = json_object_new_array();
    host = json_object_new_string(hostid);
    json_object_array_add(params, host);
    
    prepareJsonData(params, "host.delete", zbx, &cr);
    ret = sendPost(zbx, cr);

    if(ret==1){
        SLOGL_vprint(SLOGL_LVL_INFO,
                "Le hote %s a bien ete supprime de zabbix'", hostid);
    }
    else{
        SLOGL_vprint(SLOGL_LVL_ERROR,
                "Erreur lors de la suppresion de %s'", hostid);
    }

    curl_request_destroy(&cr);
    return ret;  
}

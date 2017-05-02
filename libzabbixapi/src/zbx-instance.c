#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <libconfig.h>
#include <string.h>
#include <json-c/json_object.h>
#include "logger.h"
#include "curl-request.h"
#include "zbx-instance.h"
#include "configfile.h"
#include "tools.h"
#include "sender.h"
#include "myjson.h"

void new_zbxinstance(zbxinstance **zbx){
    *zbx = calloc(1, sizeof(zbxinstance));
    (*zbx)->id         = 1;
    (*zbx)->auth       = NULL;
    (*zbx)->address    = NULL;
    (*zbx)->port       = 443;
    (*zbx)->usessl     = 1;
    (*zbx)->checkssl   = 0;
    (*zbx)->username   = NULL;
    (*zbx)->password   = NULL;
    (*zbx)->cafilename = NULL; 
}


void destroy_zbxinstance(zbxinstance **zbx){
    if((*zbx)->address) free((*zbx)->address);
    if((*zbx)->auth) free((*zbx)->auth);
    if((*zbx)->username) free((*zbx)->username);
    if((*zbx)->password) free((*zbx)->password);
    if((*zbx)->cafilename) free((*zbx)->cafilename);
    free(*zbx);
}


int prepare_zbx_instance(zbxinstance **zbx, const char *config_filename){

    config_t config;
    
    if(initConfigFile(&config, config_filename)==-1) goto error;
    if(readConfigFile(&config, config_filename)==-1) goto destroy;
    
    new_zbxinstance(zbx);    
    /*Parametres obligatoires*/
    if(zbxinstance_get_address(&config, *zbx)==-1) goto destroy;
    if(zbxinstance_get_username(&config, *zbx)==-1) goto destroy;
    if(zbxinstance_get_password(&config, *zbx)==-1) goto destroy;
    
    /*Parametres facultatifs*/
    zbxinstance_get_port(&config, *zbx);
    zbxinstance_get_usessl(&config, *zbx);
    
    if((*zbx)->usessl == 1){
        zbxinstance_get_checkssl(&config, *zbx);
        zbxinstance_get_cafilename(&config, *zbx);
        zbxinstance_get_checkhostname(&config, *zbx);
    }

    config_destroy(&config);
    return 1;
    
    destroy:
        config_destroy(&config);
        goto error;
    error:
        return -1;
}

int zbxinstance_get_address(config_t *cfg, zbxinstance *zbx){
    const char *str_tmp = NULL;
    size_t len_str =0;
    
    if(config_lookup_string(cfg, "ZABBIX.address", &str_tmp)){
        len_str = strlen(str_tmp)+1;
        zbx->address = calloc(len_str, sizeof(char));
        strncpy( zbx->address, str_tmp, len_str-1 );
        
        SLOGL_vprint(SLOGL_LVL_INFO,
        "%s: adresse -> %s", config_error_file(cfg), str_tmp);
        return 1;
    }
    SLOGL_vprint(SLOGL_LVL_ERROR,
        "%s: aucune adresse de connexion indique",
         config_error_file(cfg));
    return -1;
}

int zbxinstance_get_username(config_t *cfg, zbxinstance *zbx){
    const char *str_tmp = NULL;
    size_t len_str =0;
    
    if(config_lookup_string(cfg, "ZABBIX.username", &str_tmp)){
        len_str = strlen(str_tmp)+1;
        zbx->username = calloc(len_str, sizeof(char));
        strncpy( zbx->username, str_tmp, len_str-1 );
        
        SLOGL_vprint(SLOGL_LVL_INFO,
        "%s: username -> %s", config_error_file(cfg), str_tmp);
        return 1;
    }
    SLOGL_vprint(SLOGL_LVL_ERROR,
        "%s: aucun compte de connexion indique",
         config_error_file(cfg));
    return -1;
}

int zbxinstance_get_password(config_t *cfg, zbxinstance *zbx){
    const char *str_tmp = NULL;
    size_t len_str =0;
    
    if(config_lookup_string(cfg, "ZABBIX.password", &str_tmp)){
        len_str = strlen(str_tmp)+1;
        zbx->password = calloc(len_str, sizeof(char));
        strncpy( zbx->password, str_tmp, len_str-1 );
        
        SLOGL_vprint(SLOGL_LVL_DEBUG,
        "%s: password -> %s", config_error_file(cfg), str_tmp);
        return 1;
    }
    SLOGL_vprint(SLOGL_LVL_ERROR,
        "%s: aucun mot de passe indique",
         config_error_file(cfg));
    return -1;
}

void zbxinstance_get_port(config_t *cfg, zbxinstance *zbx){
    int port;
    
    if(config_lookup_int(cfg, "ZABBIX.port", &port)){
        if( (port<65535)&&(port>=1) ){
            zbx->port = (short unsigned int)(port);
            SLOGL_vprint(SLOGL_LVL_INFO,
        "%s: port -> %d", config_error_file(cfg), port);
        }
        else{
            SLOGL_vprint(SLOGL_LVL_ERROR,
        "%s: le port indique n'est pas valide, utilisation du port par defaut (443)", config_error_file(cfg));
        }
    }
    else {
    SLOGL_vprint(SLOGL_LVL_INFO,
        "%s: aucun port fixe, utilisation du port par default (443)",
         config_error_file(cfg));
    }
}

void zbxinstance_get_checkssl(config_t *cfg, zbxinstance *zbx){
    int checkssl;
    if(config_lookup_bool(cfg, "ZABBIX.checkssl", &checkssl)){
        zbx->checkssl = checkssl;
        SLOGL_vprint(SLOGL_LVL_INFO,
        "%s: checkssl -> %d", config_error_file(cfg), checkssl);
    }
    else{
    SLOGL_vprint(SLOGL_LVL_INFO,
        "%s: aucun parametre de verification du certificat TLS distant indique. Par defaut, si vous utilisez un certificat autosigne, la connection risque d'etre avorte",
         config_error_file(cfg));
    }
}

void zbxinstance_get_checkhostname(config_t *cfg, zbxinstance *zbx){
    int checkhostname;
    if(config_lookup_bool(cfg, "ZABBIX.checkhostname", &checkhostname)){
        zbx->checkhostname = checkhostname;
        SLOGL_vprint(SLOGL_LVL_INFO,
        "%s: checkhostname -> %d", config_error_file(cfg), checkhostname);
    }
    else{
    SLOGL_vprint(SLOGL_LVL_INFO,
        "%s: aucun parametre de verification du hostname distant indique. Par defaut cette valeur est fixe a TRUE",
         config_error_file(cfg));
    }
}

void zbxinstance_get_usessl(config_t *cfg, zbxinstance *zbx){
    int usessl;
    if(config_lookup_bool(cfg, "ZABBIX.usessl", &usessl)){
        zbx->usessl = usessl;
        SLOGL_vprint(SLOGL_LVL_INFO,
        "%s: usessl -> %d", config_error_file(cfg), usessl);
    }
    else{
    SLOGL_vprint(SLOGL_LVL_INFO,
        "%s: aucun parametre d'utilisation de SSL indique. Utilisation de la valeur par défault TRUE",
         config_error_file(cfg));
    }
}

void zbxinstance_get_cafilename(config_t *cfg, zbxinstance *zbx){
    /*
    TODO: verifiez si le fichier est un certificat
    */
    const char *cafilename = NULL;
    size_t len_str =0;
    
    if(config_lookup_string(cfg, "ZABBIX.cafile", &cafilename)){
        if(access(cafilename, F_OK)==-1){
            SLOGL_vprint(SLOGL_LVL_ERROR,
                "Le certificat %s nexiste pas", cafilename);
                return;
            }
        if (access(cafilename, R_OK)==-1){
            SLOGL_vprint(SLOGL_LVL_ERROR,
                "Le certificat %s \
                nest pas accessible en lecture", cafilename);
            return;
        }  
        len_str = strlen(cafilename)+1;
        zbx->cafilename = calloc(len_str, sizeof(char));
        strncpy( zbx->cafilename, cafilename, len_str-1 );
        SLOGL_vprint(SLOGL_LVL_DEBUG,
        "%s: cafile -> %s", config_error_file(cfg), cafilename);
    }
}

char * zbxinstance_get_url(zbxinstance *zbx){
    /*renvoie l'url de connexion au serveur 
    frontal zabbix
    https://zbx->address:zbx->port/api_jsonrpc.php
    */
    size_t len_str;
    char *ret = NULL;
    len_str = strlen(zbx->address)+countDigits(zbx->port)+26;
    ret = calloc(len_str, sizeof(char));
    
    if(zbx->usessl==1){
        snprintf(ret,len_str,"https://%s:%d/api_jsonrpc.php",
            zbx->address, zbx->port);
    }
    else {
        snprintf(ret,len_str,"http://%s:%d/api_jsonrpc.php",
            zbx->address, zbx->port);
    }
    return ret;
}


int zbx_connection(zbxinstance *zbx){
    json_object *params = NULL, 
        *user=NULL, *password=NULL;

    int ret=-1;
    const char *token ;
    size_t len_token;
    curlrequest *cr = NULL;
    json_object *authid = NULL;
    
    /*Construction du json a envoyer*/
    params   = json_object_new_object();
    user     = json_object_new_string(zbx->username);
    password = json_object_new_string(zbx->password);
    json_object_object_add(params,"user", user);
    json_object_object_add(params,"password", password);
    
    
    prepareJsonData(params, "user.login", zbx, &cr);
    json_object_object_del(cr->postdata, "auth");
    ret = sendPost(zbx, cr);
    
    /*On recupere le token d'authentification*/
    if(ret==1){
        json_object_object_get_ex(cr->response, "result", &authid);
        token = json_object_to_json_string(authid);
        len_token= strlen(token)+1;
        if(zbx->auth != NULL){
            free(zbx->auth);
        }
        zbx->auth = calloc(len_token-2, sizeof(char));
        strncpy(zbx->auth, token+1, len_token-3);
        SLOGL_vprint(SLOGL_LVL_INFO,
                "Authentification reussi sur %s", zbx->address);
    }
    else{
    SLOGL_vprint(SLOGL_LVL_ERROR,
                "Echec d'authentification sur %s", zbx->address);
    }
    curl_request_destroy(&cr);
    return ret;
}


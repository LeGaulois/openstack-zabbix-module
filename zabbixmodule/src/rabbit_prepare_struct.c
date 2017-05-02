#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "configfile.h"
#include "rabbit_prepare_struct.h"

void new_rabbitinstance(rabbitinstance **rmq){
    *rmq = calloc(1, sizeof(rabbitinstance));
    (*rmq)->hostname   = NULL;
    (*rmq)->port       = 5672;
    (*rmq)->username   = NULL;
    (*rmq)->password   = NULL;
    (*rmq)->exchange   = NULL;
    (*rmq)->bindingkey = NULL;
}


void destroy_rabbitinstance(rabbitinstance **rmq){
    if((*rmq)->hostname) free((*rmq)->hostname);
    if((*rmq)->username) free((*rmq)->username);
    if((*rmq)->password) free((*rmq)->password);
    if((*rmq)->exchange) free((*rmq)->exchange);
    if((*rmq)->bindingkey) free((*rmq)->bindingkey);
    free(*rmq);
}

int prepare_rabbit_instance(rabbitinstance **rmq, const char *config_filename, char *partname){

    config_t config;
    SLOGL_vprint(SLOGL_LVL_INFO,"*** Initialisation de instance RABBITMQ pour %s ***", partname);
    
    if(initConfigFile(&config, config_filename)==-1) goto error;
    if(readConfigFile(&config, config_filename)==-1) goto destroy;
    
    new_rabbitinstance(rmq);    
    /*Parametres obligatoires*/
    if(rabbitinstance_get_hostname(&config, partname,*rmq)==-1) goto destroy;
    if(rabbitinstance_get_exchange(&config, partname,*rmq)==-1) goto destroy;
    if(rabbitinstance_get_bindingkey(&config, partname,*rmq)==-1) goto destroy;
    /*Parametres facultatifs*/
    rabbitinstance_get_port(&config, partname,*rmq);
    rabbitinstance_get_username(&config, partname,*rmq);
    rabbitinstance_get_password(&config, partname,*rmq);

    config_destroy(&config);
    return 1;
    
    destroy:
        config_destroy(&config);
        goto error;
    error:
        return -1;
}

char* get_item_name(char *partname, const char *item_name){
    size_t len_partname, len_item_name, len_total;
    char *item = NULL;
    
    len_partname = strlen(partname);
    len_item_name = strlen(item_name);
    len_total = len_partname+ len_item_name + 1;
    item = calloc( len_total +1, sizeof(char));
    snprintf(item, len_total+1, "%s.%s", partname, item_name);
    return item;
}

int rabbitinstance_get_hostname(config_t *cfg, char *partname,rabbitinstance *rmq){
    const char *str_tmp = NULL;
    size_t len_str =0;
    char *item = NULL;
    
    item = get_item_name(partname, "address");
    
    if(config_lookup_string(cfg, item, &str_tmp)){
        len_str = strlen(str_tmp)+1;
        rmq->hostname = calloc(len_str, sizeof(char));
        strncpy( rmq->hostname, str_tmp, len_str-1 );
        
        SLOGL_vprint(SLOGL_LVL_INFO,
        "%s: adresse -> %s", config_error_file(cfg), str_tmp);
        free(item);
        return 1;
    }
    SLOGL_vprint(SLOGL_LVL_ERROR,
        "%s: aucune adresse de connexion indique",
         config_error_file(cfg));
    free(item);
    return -1;
}

void rabbitinstance_get_username(config_t *cfg, char *partname, rabbitinstance *rmq){
    const char *str_tmp = NULL;
    size_t len_str =0;
    char *item = NULL;
    
    item = get_item_name(partname, "username");
    
    if(config_lookup_string(cfg, item, &str_tmp)){
        len_str = strlen(str_tmp)+1;
        rmq->username = calloc(len_str, sizeof(char));
        strncpy( rmq->username, str_tmp, len_str-1 );
    }
    else {
        rmq->username = strndup("guest", 5);
        SLOGL_vprint(SLOGL_LVL_ERROR,
            "%s: aucun username de connexion au serveur RABBITMQ specifie, utilisation du compte 'guest'",
            config_error_file(cfg));
    }
    
    free(item);
}

void rabbitinstance_get_password(config_t *cfg, char *partname,rabbitinstance *rmq){
    const char *str_tmp = NULL;
    size_t len_str =0;
    char *item = NULL;
    
    item = get_item_name(partname, "password");
    
    if(config_lookup_string(cfg, item, &str_tmp)){
        len_str = strlen(str_tmp)+1;
        rmq->password = calloc(len_str, sizeof(char));
        strncpy( rmq->password, str_tmp, len_str-1 );
    }
    else {
        rmq->password = strndup("guest", 5);
        SLOGL_vprint(SLOGL_LVL_ERROR,
            "%s: aucun password de connexion au serveur RABBITMQ specifie, utilisation du compte 'guest'",
            config_error_file(cfg));
    }
    free(item);
}


int rabbitinstance_get_exchange(config_t *cfg, char *partname,rabbitinstance *rmq){
    const char *str_tmp = NULL;
    size_t len_str =0;
    char *item = NULL;
    
    item = get_item_name(partname, "exchange");
    
    if(config_lookup_string(cfg, item, &str_tmp)){
        len_str = strlen(str_tmp)+1;
        rmq->exchange = calloc(len_str, sizeof(char));
        strncpy( rmq->exchange, str_tmp, len_str-1 );
        
        SLOGL_vprint(SLOGL_LVL_INFO,
        "%s: exchange -> %s", config_error_file(cfg), str_tmp);
        free(item);
        return 1;
    }
    SLOGL_vprint(SLOGL_LVL_ERROR,
        "%s: aucun canal dechange specifie",
         config_error_file(cfg));
         
    free(item);
    return -1;
}

int rabbitinstance_get_bindingkey(config_t *cfg, char *partname, rabbitinstance *rmq){
    const char *str_tmp = NULL;
    size_t len_str =0;
    char *item = NULL;
    
    item = get_item_name(partname, "bindingkey");
    
    if(config_lookup_string(cfg, item, &str_tmp)){
        len_str = strlen(str_tmp)+1;
        rmq->bindingkey = calloc(len_str, sizeof(char));
        strncpy( rmq->bindingkey, str_tmp, len_str-1 );
        
        SLOGL_vprint(SLOGL_LVL_INFO,
        "%s: bindingkey -> %s", config_error_file(cfg), str_tmp);
        free(item);
        return 1;
    }
    SLOGL_vprint(SLOGL_LVL_ERROR,
        "%s: aucune bindingkey specifie",
         config_error_file(cfg));
    free(item);
    return -1;
}


void rabbitinstance_get_port(config_t *cfg, char *partname,rabbitinstance *rmq){
    int port;
    char *item = NULL;
    
    item = get_item_name(partname, "port");
    
    if(config_lookup_int(cfg, item, &port)){
        if( (port<65535)&&(port>=1) ){
            rmq->port = (short unsigned int)(port);
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
        "%s: aucun port fixe, utilisation du port par default (5672)",
         config_error_file(cfg));
    }
    free(item);
}

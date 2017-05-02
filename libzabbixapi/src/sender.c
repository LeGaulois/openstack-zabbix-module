#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>
#include <json-c/json_object.h>
#include <libconfig.h>
#include "curl-request.h"
#include "zbx-instance.h"
#include "logger.h"



void prepareJsonData(json_object *param,
    const char *methode, zbxinstance *zbx, curlrequest **cr){
    json_object *jobj = NULL, *jsonrpc = NULL,
    *method = NULL,*id = NULL,*auth = NULL;
    
    curl_request_init(cr);
    jobj    = json_object_new_object();
    jsonrpc = json_object_new_string("2.0");
    method  = json_object_new_string(methode);
    id      = json_object_new_int(zbx->id);
    if(zbx->auth)auth = json_object_new_string(zbx->auth);
    
    json_object_object_add(jobj,"jsonrpc", jsonrpc);
    json_object_object_add(jobj,"method", method);
    json_object_object_add(jobj,"params", param);
    json_object_object_add(jobj,"id", id);
    if(zbx->auth) json_object_object_add(jobj,"auth", auth);
    
    (*cr)->postdata = jobj;
}



int sendPost(zbxinstance *zbx, const curlrequest *r){
    /*
    Permet l'envoi d'eune requete HTTP
    Assurez vous que la fonction curl_global_init
    a ete appele dans la fonction principale 
    ex: curl_global_init(CURL_GLOBAL_NOTHING);
    
    Attention, curl_global_init(CURL_GLOBAL_ALL)
    provoque une petite fuite de memoire :(
    */
    CURL *curl;
    CURLcode res;
    int ret = 1;
    char *url = NULL;
    
    curl = curl_easy_init();
    struct curl_slist *chunk = NULL;
    
    if(curl) {
        url = zbxinstance_get_url(zbx);
        chunk = curl_slist_append(chunk, "Content-Type:application/json");
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_object_to_json_string(r->postdata));
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "zabbix_api");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_curl_response);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)r);
     
        /*
        if(zbx->checkssl==1) curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
        else  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        if(zbx->checkhostname==1)curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 1L);
        else curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        
        if(zbx->cafilename)curl_easy_setopt(curl,CURLOPT_CAPATH , zbx->cafilename);
        */
        res = curl_easy_perform(curl);
     
        if(res != CURLE_OK){
            SLOGL_vprint(SLOGL_LVL_ERROR,
                "Erreur lors de l'envoie de la requete %s sur %s: %s", json_object_get_string(r->postdata),url,curl_easy_strerror(res));
            ret=-1;
        }
        /*On verifie si le serveur a rmeonte une erreur*/
        else{
            if(json_object_object_get_ex(r->response,"error", NULL)==1){
                ret=-1;
            }
        }
    
        curl_easy_cleanup(curl);
        curl_slist_free_all(chunk);
        free(url);
    }
    zbx->id +=1;
    curl_global_cleanup();
    return ret;
}

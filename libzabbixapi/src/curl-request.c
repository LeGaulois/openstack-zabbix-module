#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>
#include <json-c/json_object.h>
#include "myjson.h"
#include "curl-request.h"
#include "logger.h"

int curl_request_init(curlrequest **cr){
    *cr = calloc(1, sizeof(curlrequest));
    
    if (*cr == NULL) return -1;
    (*cr)->postdata = NULL;
    (*cr)->response = NULL;
    return 1;
}


void curl_request_destroy(curlrequest **cr){
    if(*cr==NULL) return;
    if((*cr)->postdata != NULL) json_object_free(&((*cr)->postdata));
    if((*cr)->response != NULL) json_object_free(&((*cr)->response));
    free(*cr);
}



size_t get_curl_response(char *contents, size_t size,size_t nmemb, void *cr)
{
    size_t new_len = size*nmemb;
    ((curlrequest *)(cr))->response = json_tokener_parse(contents);
    return new_len;
}

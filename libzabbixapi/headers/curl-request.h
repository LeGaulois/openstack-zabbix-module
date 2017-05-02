#ifndef CURL_REQUEST_H_INCLUDED
#define CURL_REQUEST_H_INCLUDED

typedef struct curlrequest curlrequest;


struct curlrequest{
    json_object *postdata;
    json_object *response;
};

int curl_request_init(curlrequest **cr);
void curl_request_destroy(curlrequest **cr);
size_t get_curl_response(char *contents, size_t size,size_t nmemb, void *cr);
#endif

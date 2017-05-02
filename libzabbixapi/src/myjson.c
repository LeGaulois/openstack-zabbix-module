#include <json-c/json.h>
#include <json-c/json_object.h>
#include <json-c/json_object_private.h>
#include <stdio.h>
#include <string.h>


int json_get_value(json_object *value, void **ret){
    /*
    Cette fonction permet de récupérer la valeur d'une clé JSON
    de type: int, double ou string
    */
    int v_int;
    double v_double;
    size_t len_str;
    json_type type;
    
    if(value==NULL) return -1;
    
    type = json_object_get_type(value);
    
    switch (type) {
        case json_type_null: 
            break;
        case json_type_array:
            break;
        case json_type_object:
            *ret = calloc(1, sizeof(json_object));
            memcpy(*ret,value, sizeof(json_object));
            break;
        case json_type_double:
            v_double = json_object_get_double(value);
            *ret     = calloc(1, sizeof(double));
            memcpy(*ret,&v_double, sizeof(double));
            break;
        case json_type_int:
            *ret  = calloc(1, sizeof(int));
            v_int = json_object_get_int(value);
            memcpy(*ret,&v_int, sizeof(int)); ;
            break;
        case json_type_boolean:
            *ret = calloc(1, sizeof(short unsigned int));
            v_int = json_object_get_boolean(value);
            memcpy(*ret,&v_int, sizeof(short unsigned int));
            break;
        case json_type_string:
            len_str = strlen(json_object_get_string(value))+1;
            *ret = calloc( len_str, sizeof(int));
            strncpy(*ret, json_object_get_string(value), len_str);
            break;
    }
    return type;
}


void json_object_free(json_object **jobj){
    if(*jobj == NULL) return;
    while (json_object_put(*jobj)!=1){
    }
}

int json_get_value_from_key(json_object *jobj, const char *key, void **ret){
    int type;
    json_object *tmp = NULL;
    json_object_object_get_ex(jobj, key, &tmp);
    type = json_get_value(tmp, ret);
    return type;
}


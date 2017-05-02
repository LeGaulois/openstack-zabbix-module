#ifndef MYJSON_H_INCLUDED
#define MYJSON_H_INCLUDED

int json_get_value(json_object *value, void **ret);
void json_object_free(json_object **jobj);
int json_get_value_from_key(json_object *jobj, const char *key, void **ret);

#endif

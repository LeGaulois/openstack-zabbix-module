#ifndef rabbit_prepare_struct_H_H
#define rabbit_prepare_struct_H_H

typedef struct rabbitinstance rabbitinstance;

struct rabbitinstance {
    char *hostname;
    int  port;
    char *username;
    char *password;
    char *exchange;
    char *bindingkey;
};

int prepare_rabbit_instance(rabbitinstance **rmq, const char *config_filename, char *partname);
char* get_item_name(char *partname, const char *item_name);
void new_rabbitinstance(rabbitinstance **rmq);
void destroy_rabbitinstance(rabbitinstance **rmq);
int rabbitinstance_get_hostname(config_t *cfg, char *partname, rabbitinstance *rmq);
void rabbitinstance_get_username(config_t *cfg, char *partname, rabbitinstance *rmq);
void rabbitinstance_get_password(config_t *cfg, char *partname, rabbitinstance *rmq);
int rabbitinstance_get_exchange(config_t *cfg, char *partname, rabbitinstance *rmq);
int rabbitinstance_get_bindingkey(config_t *cfg, char *partname, rabbitinstance *rmq);
void rabbitinstance_get_port(config_t *cfg, char *partname, rabbitinstance *rmq);

#endif

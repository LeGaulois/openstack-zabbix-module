#ifndef C_ZBX_INSTANCE_H_H
#define C_ZBX_INSTANCE_H_H


typedef struct zbxinstance zbxinstance;

struct zbxinstance{
    int             id;
    char            *auth;
    char            *address;
    unsigned int    port:16;
    unsigned int    usessl:1;
    unsigned int    checkssl:1;
    unsigned int    checkhostname:1;
    char            *cafilename;
    char            *username;
    char            *password;
};

int prepare_zbx_instance(zbxinstance **zbx, const char *config_filename);
void new_zbxinstance(zbxinstance **zbx);
void destroy_zbxinstance(zbxinstance **zbx);
int zbxinstance_get_address(config_t *cfg, zbxinstance *zbx);
int zbxinstance_get_username(config_t *cfg, zbxinstance *zbx);
int zbxinstance_get_password(config_t *cfg, zbxinstance *zbx);
void zbxinstance_get_port(config_t *cfg, zbxinstance *zbx);
void zbxinstance_get_usessl(config_t *cfg, zbxinstance *zbx);
void zbxinstance_get_checkssl(config_t *cfg, zbxinstance *zbx);
char * zbxinstance_get_url(zbxinstance *zbx);
void zbxinstance_get_cafilename(config_t *cfg, zbxinstance *zbx);
void zbxinstance_get_checkhostname(config_t *cfg, zbxinstance *zbx);
int zbx_connection(zbxinstance *zbx);
#endif

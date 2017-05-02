#ifndef ZBX_HOSTS_H_INCLUDED
#define ZBX_HOSTS_H_INCLUDED

#include "list.h"
#include "logger.h"
#include "zbx-instance.h"
#include "zbx-interface.h"
#include "curl-request.h"
#include "sender.h"


#define MONITORED    0
#define UNMONITORED  1
#define NOENCRYPTION 1
#define PSK          2
#define CERTIFICATE  4


typedef struct zbxhost zbxhost;

struct zbxhost {
    char            *hostid;
    char            *host;
    char            *description;
    char            *name;
    char            *proxy_host_id;
    
    unsigned int    status:1;
    unsigned int    tls_connect:3;
    unsigned int    tls_accept:3;
    char            *tls_issuer;
    char            *tls_subject;
    char            *tls_psk_identity;
    char            *tls_psk;
    
    list_t          *groups;
    list_t          *interfaces;
    list_t          *templates;     
};

int zbxhost_init(zbxhost **host);
void zbxhost_destroy(zbxhost **host);
int zbxhost_create(zbxhost **zbxh, char *host, char *description, char *name, char *proxy_host_id,unsigned int status,unsigned int tls_connect, unsigned int tls_accept,char * tls_issuer, char *tls_subject,char *tls_psk_identity, char *tls_psk,list_t *groups, list_t *interfaces, list_t *templates);

json_object* zbxhost_to_json(zbxhost* zhost);
json_object* zbxhost_listgroups_to_json(zbxhost *host);
json_object* zbxhost_listtemplates_to_json(zbxhost *host);
int zbxhost_add(zbxinstance *zbx, zbxhost *host);
int zbxhost_delete(zbxinstance *zbx,zbxhost *host);
json_object* getAllHosts(zbxinstance *zbx);
json_object* searchHostIDByHostname(zbxinstance *zbx, const char *hostname);
int zbxhost_delete_byid(zbxinstance *zbx,const char *hostid);

#endif

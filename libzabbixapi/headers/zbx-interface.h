#ifndef ZBX_INTERFACE_H_INCLUDED
#define ZBX_INTERFACE_H_INCLUDED

#define TYPE_AGENT 1
#define TYPE_SNMP  2
#define TYPE_IPMI  3
#define TYPE_JMX   4

#define VIA_DNS 0
#define VIA_IP  1

typedef struct zbxinterface zbxinterface;

struct zbxinterface{
    int     interfaceid;
    char    *dns;
    char    *hostid;
    char    *ip;
    unsigned int main:1;
    unsigned int port:16;
    unsigned int type:3;
    unsigned int useip:1;
};

int zbxinterface_init(zbxinterface **eth);
void zbxinterface_destroy(zbxinterface **eth);
json_object* zbxinterface_to_json(zbxinterface *eth);
json_object* zbxinterface_list_to_json(list_t *leth);
int zbxinterface_create(zbxinterface **eth, char *dns,
    char *ip, unsigned int main, unsigned int port, 
    unsigned int type, unsigned int useip);
    
#endif

#ifndef nova_add_instance_H_H
#define nova_add_instance_H_H

void nova_add_instance(zbxinstance *zbx, json_object *data);
void add_from_zabbix_queue(sharet *share, json_object *data);

#endif

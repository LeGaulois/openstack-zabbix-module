#ifndef rabbit_functions_H_H
#define rabbit_functions_H_H

#include <assert.h>
#include <amqp_tcp_socket.h>
#include <amqp.h>
#include <amqp_framing.h>
#include "configfile.h"
#include "list.h"
#include "rabbit_prepare_struct.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <stdint.h>
#include <amqp_tcp_socket.h>
#include <amqp.h>
#include <amqp_framing.h>

#include <json-c/json.h>
#include <json-c/json_object.h>
#include <json-c/json_object_private.h>
#include <assert.h>

#include "utils.h"
#include "logger.h"
#include "configfile.h"
#include "zbx-instance.h"
#include "tools.h"
#include "amqp_listen.h"
#include "rabbit_functions.h"


#define INSTANCE_CREATE_END 1
#define INSTANCE_DELETE_END 2

#define ZABBIX_LIST_GROUPS      1
#define ZABBIX_LIST_TEMPLATES   2
#define ZABBIX_CREATE_HOST      3

#define ZABBIX_REQUEST          0
#define ZABBIX_RESPONSE         1


void rabbit_sendstring(amqp_connection_state_t *conn, rabbitinstance *rmq, char *messagebody);
void nova_add_instance(zbxinstance *zbx, json_object *data);
void *rabbit_listen_nova(void *zbx);
void *rabbit_listen_zabbix(void *zbx);
int define_nova_event_type(json_object *event);
int define_zabbix_event_type(json_object *event);
int define_zabbix_message_type(json_object *messagetype);
void rabbitmq_bind_queue(amqp_connection_state_t *conn,\
    amqp_bytes_t *queuename, rabbitinstance *rmq);
void rabbitmq_authentification(amqp_connection_state_t *conn,\
     rabbitinstance *rmq);
void rabbitmq_connection(amqp_connection_state_t *conn,\
         amqp_socket_t **socket, int *status, rabbitinstance *rmq);
int rabbitmq_open_channel(amqp_connection_state_t *conn,\
        amqp_queue_declare_ok_t **r, amqp_bytes_t *queuename);
void rabbitmq_bind_queue(amqp_connection_state_t *conn,\
    amqp_bytes_t *queuename, rabbitinstance *rmq);
    
#endif

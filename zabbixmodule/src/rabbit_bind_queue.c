#include <stdlib.h>
#include <stdio.h>
#include "list.h"
#include "rabbit_functions.h"


void rabbitmq_bind_queue(amqp_connection_state_t *conn,\
    amqp_bytes_t *queuename, rabbitinstance *rmq){

    amqp_queue_bind(*conn, 1, *queuename, amqp_cstring_bytes(rmq->exchange),\
     amqp_cstring_bytes(rmq->bindingkey),amqp_empty_table);
     
    die_on_amqp_error(amqp_get_rpc_reply(*conn), "Binding queue");
    amqp_basic_consume(*conn, 1, *queuename, amqp_empty_bytes,\
        0, 1, 0, amqp_empty_table);
    die_on_amqp_error(amqp_get_rpc_reply(*conn), "Consuming");
}

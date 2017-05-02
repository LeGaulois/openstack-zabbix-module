#include <stdlib.h>
#include <stdio.h>
#include "rabbit_functions.h"

int rabbitmq_open_channel(amqp_connection_state_t *conn,\
        amqp_queue_declare_ok_t **r, amqp_bytes_t *queuename)
{
    amqp_channel_open(*conn, 1);
    die_on_amqp_error(amqp_get_rpc_reply(*conn), "Opening channel");
    {
        *r = amqp_queue_declare(*conn, 1,amqp_empty_bytes,\
                 0, 0, 0, 1,amqp_empty_table);
        die_on_amqp_error(amqp_get_rpc_reply(*conn), "Declaring queue");
        *queuename = amqp_bytes_malloc_dup((*r)->queue);
        
        if (queuename->bytes == NULL) {
            fprintf(stderr, "Out of memory while copying queue name");
            return -1;
        }
    }
    return 1;
}

#include <stdlib.h>
#include <stdio.h>
#include "rabbit_functions.h"


void rabbitmq_connection(amqp_connection_state_t *conn,\
         amqp_socket_t **socket, int *status, rabbitinstance *rmq)
{
    *conn = amqp_new_connection();

    *socket = amqp_tcp_socket_new(*conn);
    if (!*socket) {
        die("creating TCP socket");
    }
  
    *status = amqp_socket_open(*socket, rmq->hostname, rmq->port);
  
    if(*status) {
        SLOGL_vprint(SLOGL_LVL_ERROR,"Impossible d'ouvrir une connexion RABBITMQ");
        die("opening TCP socket");
    }
}

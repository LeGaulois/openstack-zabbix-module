#include <stdlib.h>
#include <stdio.h>
#include "rabbit_functions.h"

void rabbitmq_authentification(amqp_connection_state_t *conn,\
     rabbitinstance *rmq)
{
    die_on_amqp_error(
        amqp_login(*conn, "/", 0, 131072, 0,AMQP_SASL_METHOD_PLAIN,\
         rmq->username, rmq->password),"Logging in");
}

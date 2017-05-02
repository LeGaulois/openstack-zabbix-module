#include "rabbit_functions.h"

void rabbit_sendstring(amqp_connection_state_t *conn, rabbitinstance *rmq, char *messagebody){
    amqp_basic_properties_t props;
    
    //fprintf(stderr,"%s\n", messagebody);
    props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
    props.content_type = amqp_cstring_bytes("application/json");
    props.delivery_mode = 2;
    
    die_on_error(amqp_basic_publish(*conn,
                                    1,
                                    amqp_cstring_bytes(rmq->exchange),
                                    amqp_cstring_bytes(rmq->bindingkey),
                                    0,
                                    0,
                                    &props,
                                    amqp_cstring_bytes(messagebody)),
                 "Publishing");
}

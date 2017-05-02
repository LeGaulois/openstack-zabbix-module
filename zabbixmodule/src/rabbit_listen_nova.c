#include "rabbit_functions.h"
#include "nova_delete_instance.h"
#include "myjson.h"
#include "amqp_listen.h"
#include <pthread.h>

void *rabbit_listen_nova(void *arg){
    int status;
    amqp_socket_t *socket = NULL;
    amqp_connection_state_t conn;
    amqp_queue_declare_ok_t *r;
    amqp_bytes_t queuename;
    json_object *message = NULL, *jtmp = NULL, *data = NULL;
    struct json_tokener *tokener = NULL;
    const char *oslo_message;
    char *initial_message = NULL;
    int event_type=-1;
    rabbitinstance *rmq = NULL;
    sharet *share = (sharet*)(arg);
    zbxinstance *zbx =  share->zbx;
    
    
    new_rabbitinstance(&rmq);
    
     if( prepare_rabbit_instance(&rmq, "default.cfg", "RABBIT_NOVA")==-1){
        SLOGL_vprint(SLOGL_LVL_ERROR,
        "Echec de l'initalisation de l'instance d'ecoute pour la file NOVA");
        destroy_rabbitinstance(&rmq);
        pthread_exit(NULL);
     }
     else {
        SLOGL_vprint(SLOGL_LVL_INFO,
        "Initialisation instance d'ecoute pour la file NOVA reussi");
     }
    
    rabbitmq_connection(&conn, &socket, &status, rmq);
    rabbitmq_authentification(&conn, rmq);
    
    if(rabbitmq_open_channel(&conn, &r, &queuename)!=1){
        pthread_exit(NULL);
    }

    rabbitmq_bind_queue(&conn, &queuename, rmq);
    
    for (;;) {
      amqp_rpc_reply_t res;
      amqp_envelope_t envelope;

      amqp_maybe_release_buffers(conn);

      res = amqp_consume_message(conn, &envelope, NULL, 0);

      if (AMQP_RESPONSE_NORMAL != res.reply_type) {
        break;
      }

    /*
    On duplique le contenue du message amqp dans un nouveau string
    Le json recu par la librairie amqp est formé bizarrement:
    le contenue de oslo message est un string, on doit donc le reparser
    */
             
    initial_message = calloc(envelope.message.body.len+1, sizeof(char));
    strncpy(initial_message, envelope.message.body.bytes, envelope.message.body.len);

    tokener = json_tokener_new();
    message = json_tokener_parse_ex(tokener, initial_message, envelope.message.body.len);
    
    json_tokener_free(tokener); 	
    free(initial_message);
    amqp_destroy_envelope(&envelope);
    
    if(json_object_object_get_ex(message,"oslo.message", &jtmp)!=1){
        json_object_free(&message);
        continue;
    }
    
    oslo_message = json_object_get_string(jtmp);
    message = json_tokener_parse(oslo_message);
      
      
    if(json_object_object_get_ex(message,"event_type", &jtmp)==1){
        
        event_type = define_nova_event_type(jtmp);
        json_object_object_get_ex(message,"payload",&jtmp);
        json_object_object_get_ex(jtmp,"nova_object.data",&data);

        
        switch(event_type){
            case INSTANCE_CREATE_END:
                nova_add_instance(share, data);
                break;
            case INSTANCE_DELETE_END:
                nova_delete_instance(zbx, data);
                break;
            default:
                fprintf(stderr,"event type inconnu\n");
        }
      }
      json_object_put(message);
      free(oslo_message);
      message = NULL;
      jtmp=NULL;
      
      if(event_type==INSTANCE_CREATE_END) break;
    }
    
    
    fprintf(stderr,"NOVA QUIT\n");
    die_on_amqp_error(amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS), "Closing channel");
  die_on_amqp_error(amqp_connection_close(conn, AMQP_REPLY_SUCCESS), "Closing connection");
  die_on_error(amqp_destroy_connection(conn), "Ending connection");
  destroy_rabbitinstance(&rmq);
  pthread_exit(NULL);
}

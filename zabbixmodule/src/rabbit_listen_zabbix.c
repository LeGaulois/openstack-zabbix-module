#include "rabbit_functions.h"
#include "zbx-groups.h"
#include "zbx-templates.h"
#include "amqp_listen.h"
#include "list.h"
#include "myjson.h"
#include <pthread.h>
#include "nova_add_instance.h"


extern list_t *list_hosts;

void *rabbit_listen_zabbix(void *arg){
    json_object *message = NULL, *jtmp = NULL;
    int event_type, status;
    amqp_socket_t *socket = NULL;
    amqp_connection_state_t conn;
    amqp_queue_declare_ok_t *r;
    amqp_bytes_t queuename;
    rabbitinstance *rmq = NULL;
    char *initial_message = NULL;
  
    sharet *share = (sharet*)(arg);
    new_rabbitinstance(&rmq);
    
    if( prepare_rabbit_instance(&rmq, "default.cfg", "RABBIT_ZABBIX")==-1){
        SLOGL_vprint(SLOGL_LVL_ERROR,
        "Echec de l'initalisation de l'instance d'ecoute pour la file zabbix");
        destroy_rabbitinstance(&rmq);
        pthread_exit(NULL);
     }
     else {
        SLOGL_vprint(SLOGL_LVL_INFO,
        "Initialisation instance d'ecoute pour la file ZABBIX reussi");
     }
    
    rabbitmq_connection(&conn, &socket, &status, rmq);
    rabbitmq_authentification(&conn, rmq);
    
    if(rabbitmq_open_channel(&conn, &r, &queuename)!=1){
        destroy_rabbitinstance(&rmq);
        pthread_exit(NULL);
    }

    rabbitmq_bind_queue(&conn, &queuename, rmq);
    
    for (;;) {
        amqp_rpc_reply_t res;
        amqp_envelope_t envelope;
  
        res = amqp_consume_message(conn, &envelope, NULL, 0);
        amqp_maybe_release_buffers(conn);
      
        if (AMQP_RESPONSE_NORMAL != res.reply_type) {
            break;
        }
    
        /*
        On duplique le contenue du message amqp dans un nouveau string
        Le json recu par la librairie amqp est formé bizarrement:
        le contenue de oslo message est un string, on doit donc le
        reparser
        */
        initial_message = strndup(envelope.message.body.bytes,
        envelope.message.body.len);
        message = json_tokener_parse(initial_message);
        free(initial_message);
        amqp_destroy_envelope(&envelope);
      
        if(json_object_object_get_ex(message,"event_type", &jtmp)==1){
            
            event_type = define_zabbix_event_type(jtmp);

            /*
            On verifie qu'il ne s'agit pas d'une requete
             que nous avons envoyé 
            en tant que reponse, sinon risque de bloucle 
            */
            if (json_object_object_get_ex(message,"message_type",
             &jtmp)&&define_zabbix_message_type(jtmp)==ZABBIX_RESPONSE){
                json_object_put(message);
                message = NULL;
                continue;
            }
       
        
            switch(event_type){
                case ZABBIX_LIST_GROUPS:
                    jtmp = getAllGroups(share->zbx);
                    json_object_object_add(message, "payload", jtmp);
                    jtmp = json_object_new_string("zabbix.response");
                    json_object_object_add(message,"message_type",
                     jtmp);
                    rabbit_sendstring(&conn, rmq,
                     (char*)(json_object_to_json_string(message)));
                    break;
                
                case ZABBIX_LIST_TEMPLATES:
                    jtmp = getAllTemplates(share->zbx);
                    json_object_object_add(message, "payload", jtmp);
                    jtmp = json_object_new_string("zabbix.response");
                    json_object_object_add(message,"message_type",
                     jtmp);
                    rabbit_sendstring(&conn, rmq,
                    (char*)(json_object_to_json_string(message)));
                    break;
                
                case ZABBIX_CREATE_HOST:
                    json_object_object_get_ex(message,"payload", &jtmp);
                    add_from_zabbix_queue(share, jtmp);
                    break;
                default:
                    fprintf(stderr,"Event type %d inconnu \n",
                     event_type);
            }
        }
      
        json_object_put(message);
        message = NULL;
        if(event_type==ZABBIX_CREATE_HOST)break;
    }
    
    
    fprintf(stderr,"ZABBIX QUIT\n");
    die_on_amqp_error(amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS),
     "Closing channel");
    amqp_maybe_release_buffers(conn);
    die_on_amqp_error(amqp_connection_close(conn, AMQP_REPLY_SUCCESS),
   "Closing connection");
    die_on_error(amqp_destroy_connection(conn), "Ending connection");
    destroy_rabbitinstance(&rmq);
    pthread_exit(NULL);
}

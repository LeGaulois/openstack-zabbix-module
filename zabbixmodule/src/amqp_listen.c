#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <amqp_tcp_socket.h>
#include <amqp.h>
#include <amqp_framing.h>
#include <pthread.h>
#include <curl/curl.h>
#include <json-c/json.h>
#include <json-c/json_object.h>
#include <json-c/json_object_private.h>
#include <assert.h>

#include "utils.h"
#include "logger.h"
#include "configfile.h"
#include "zbx-instance.h"
#include "tools.h"
#include "rabbit_functions.h"
#include "list.h"
#include "amqp_listen.h"

SLOGL_level programLevel = SLOGL_LVL_DEBUG;



int main(int argc, char const *const *argv)
{
    sharet *share = NULL;
    pthread_t thr_nova, thr_zabbix;

    share = calloc(1, sizeof(sharet));
    share->zbx = NULL;
    share->mutex_list = malloc(1*sizeof(pthread_mutex_t));
    pthread_mutex_init(share->mutex_list,NULL);
    share->list_hosts = list_new();
    
    SLOGL_init("openstack_zabbixmodule");
    
    curl_global_init(CURL_GLOBAL_ALL);
    prepare_zbx_instance(&(share->zbx),"default.cfg");

    if(zbx_connection(share->zbx)!=1){
        return EXIT_FAILURE;
    }

    
    if (pthread_create(&thr_nova, NULL, rabbit_listen_nova, (void*)(share)) != 0)
    {
	    fprintf(stderr, "Erreur dans pthread_create\n");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_create(&thr_zabbix, NULL, rabbit_listen_zabbix, (void*)(share))!= 0)
    {
	    fprintf(stderr, "Erreur dans pthread_create\n");
		exit(EXIT_FAILURE);
	}
	
	pthread_join(thr_nova, NULL);
	pthread_join(thr_zabbix, NULL);   
  //rabbit_listen_nova(zbx);


    destroy_zbxinstance(&(share->zbx));
    free(share->mutex_list);
    list_destroy(share->list_hosts);
    free(share);
    SLOGL_quit();
  

  return 0;
}

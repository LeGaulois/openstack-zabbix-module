#ifndef AMQP_LISTEN_H_H
#define AMQP_LISTEN_H_H


#define INSTANCE_CREATE_END 1
typedef struct sharet sharet;

struct sharet {
    zbxinstance *zbx;
    list_t *list_hosts;
    pthread_mutex_t *mutex_list;
};

int define_event_type(json_object *event);


#endif

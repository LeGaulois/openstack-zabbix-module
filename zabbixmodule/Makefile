CC=gcc
CFLAGS= -Wall  -g -fstack-protector-all -Wstack-protector -I ./headers -I ../libzabbixapi/headers/


all: list.o list-iterator.o utils.o rabbit_bind_queue.o rabbit_connection.o rabbit_authentification.o rabbit_open_channel.o rabbit_prepare_struct.o rabbit_listen_nova.o rabbit_listen_zabbix.o rabbit_send.o \
	define_event_type.o nova_delete_instance.o nova_add_instance.o
	$(CC) $(CFLAGS) list.o list_iterator.o rabbit_bind_queue.o rabbit_connection.o rabbit_authentification.o rabbit_open_channel.o rabbit_prepare_struct.o rabbit_listen_nova.o define_event_type.o nova_delete_instance.o utils.o nova_add_instance.o rabbit_listen_zabbix.o rabbit_send.o src/amqp_listen.c -o zabxapi -ljson-c -ljson-c -lcurl -lconfig -lzbxapi -lrabbitmq -lpthread
	

list.o:
	$(CC) $(CFLAGS) -c src/list.c
	
list-iterator.o:
	$(CC) $(CFLAGS) -c src/list_iterator.c

rabbit_listen_nova.o: nova_delete_instance.o nova_add_instance.o rabbit_connection.o
	$(CC) $(CFLAGS) -c src/rabbit_listen_nova.c

rabbit_listen_zabbix.o: rabbit_connection.o
	$(CC) $(CFLAGS) -c src/rabbit_listen_zabbix.c
	
rabbit_send.o:
	$(CC) $(CFLAGS) -c src/rabbit_send.c

define_event_type.o:
	$(CC) $(CFLAGS) -c src/define_event_type.c

rabbit_bind_queue.o:
	$(CC) $(CFLAGS) -c src/rabbit_bind_queue.c

rabbit_connection.o:
	$(CC) $(CFLAGS) -c src/rabbit_connection.c

rabbit_authentification.o:
	$(CC) $(CFLAGS) -c src/rabbit_authentification.c

rabbit_open_channel.o:
	$(CC) $(CFLAGS) -c src/rabbit_open_channel.c

rabbit_prepare_struct.o:
	$(CC) $(CFLAGS) -c src/rabbit_prepare_struct.c

nova_add_instance.o:
	$(CC) $(CFLAGS) -c src/nova_add_instance.c

nova_delete_instance.o:
	$(CC) $(CFLAGS) -c src/nova_delete_instance.c

utils.o:
	$(CC) $(CFLAGS) -c src/utils.c

clean:
	rm *.o

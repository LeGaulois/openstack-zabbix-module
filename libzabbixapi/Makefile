CC=gcc
CFLAGS= -Wall -fPIC -g -fstack-protector-all -Wstack-protector -I ./headers 


all: logger.o tools.o curl-request.o myjson.o configfile.o zbx-instance.o sender.o list.o list-iterator.o list_node.o zbx-hosts.o zbx-interface.o
	$(CC) $(CFLAGS) logger.o tools.o curl-request.o myjson.o configfile.o zbx-instance.o sender.o list.o list_iterator.o list_node.o zbx-hosts.o zbx-interface.o src/test.c -o zabxapi -ljson-c -lcurl -lconfig
	
shared: logger.o tools.o curl-request.o myjson.o configfile.o zbx-instance.o sender.o list.o list-iterator.o list_node.o zbx-hosts.o zbx-groups.o zbx-interface.o zbx-templates.o
	$(CC) $(CFLAGS) -shared logger.o tools.o curl-request.o myjson.o configfile.o zbx-instance.o sender.o list.o list_iterator.o list_node.o zbx-hosts.o zbx-interface.o  zbx-groups.o zbx-templates.o -o libzbxapi.so -ljson-c -lcurl -lconfig

test:
	$(CC) $(CFLAGS) src/test.c -o zabxapi -ljson-c -lcurl -lconfig -lzbxapi


logger.o:
	$(CC) $(CFLAGS) -c src/logger.c

tools.o:
	$(CC) $(CFLAGS) -c src/tools.c
	
curl-request.o: myjson.o
	$(CC) $(CFLAGS) -c src/curl-request.c
	
myjson.o:
	$(CC) $(CFLAGS) -c src/myjson.c
	
configfile.o: logger.o
	$(CC) $(CFLAGS) -c src/configfile.c
	
zbx-instance.o: logger.o tools.o configfile.o
	$(CC) $(CFLAGS) -c src/zbx-instance.c
	
sender.o: curl-request.o zbx-instance.o
	$(CC) $(CFLAGS) -c src/sender.c

list.o:
	$(CC) $(CFLAGS) -c src/list.c
	
list-iterator.o:
	$(CC) $(CFLAGS) -c src/list_iterator.c

zbx-groups.o: list.o zbx-instance.o zbx-interface.o logger.o curl-request.o
	$(CC) $(CFLAGS) -c src/zbx-groups.c

zbx-templates.o: list.o zbx-instance.o zbx-interface.o logger.o curl-request.o
	$(CC) $(CFLAGS) -c src/zbx-templates.c

list_node.o:
	$(CC) $(CFLAGS) -c src/list_node.c
	
zbx-hosts.o: list.o zbx-instance.o zbx-interface.o logger.o curl-request.o
	$(CC) $(CFLAGS) -c src/zbx-hosts.c
	
zbx-interface.o: logger.o list.o
	$(CC) $(CFLAGS) -c src/zbx-interface.c
clean:
	rm *.o

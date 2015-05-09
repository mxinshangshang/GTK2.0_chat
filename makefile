all:server client

OBJ=client.o cl_sock.o
OBJ1=server.o se_sock.o

# Option for development
CFLAGS= -g

# Option for release
#CFLAGS= -o 
 
server:server.o se_sock.o server.h
	gcc -Wall $(OBJ1) $(CFLAGS) -o server `pkg-config gtk+-2.0 --libs` -l pthread
server.o:server.c server.h
	gcc -Wall $(CFLAGS) -c $< `pkg-config gtk+-2.0 --cflags`
server_sock.o:se_sock.c server.h
	gcc -Wall $(CFLAGS) c $<
 
client:client.o cl_sock.o client.h
	gcc -Wall $(OBJ) $(CFLAGS) -o client `pkg-config gtk+-2.0 --libs` -l pthread
client.o:client.c client.h
	gcc -Wall $(CFLAGS) -c $< `pkg-config gtk+-2.0 --cflags`
client_sock.o:cl_sock.c client.h
	gcc -Wall $(CFLAGS) -c $<
 
clean:
	rm *.o

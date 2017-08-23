#ifndef CLIENT_H
#define CLIENT_H
#include <arpa/inet.h>

#define DOMAIN 			AF_INET
#define SERVER_ADDR 		(inet_addr("127.0.0.1")) 
#define SERVER_PORT 		(htons(8888))

#define TYPE 			SOCK_STREAM
#define PROTOCOL 		0
#define MAX_CLIENT_MES		10



void client_loop(int *status);

#endif

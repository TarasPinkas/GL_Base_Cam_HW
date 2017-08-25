#ifndef COMMON_H
#define COMMON_H

#include <arpa/inet.h>

enum {
	NO_ERROR = 0,
	ERROR_CANT_CREATE = 1,
	ERROR_CANT_BIND = 2,
	ERROR_CANT_LISTEN = 3,
	ERROR_CANT_ACCEPT = 4,
	ERROR_CANT_READ = 5,
	ERROR_CANT_CONNECT = 6,
	ERROR_CANT_SEND = 7,
	ERROR_CANT_GET_HOST_ADDR = 8,
	ERROR_INTERRUPTED = 100
};

#define FINAL_MES 		"End connection\n"

#define DOMAIN		AF_INET
#define SERVER_ADDR	(inet_addr("127.0.0.1"))
#define SERVER_PORT	(htons(8888))
#define TYPE		SOCK_STREAM
#define PROTOCOL	0

const int MAX_CLIENTS;
const int MAX_CLIENT_MES;
const int MAX_MESSAGE_SIZE;

#endif /* COMMON_H */

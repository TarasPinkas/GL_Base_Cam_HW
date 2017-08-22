#ifndef SERVER_H
#define SERVER_H

static inline void print_error(int error_code);

/*
 * socket_fd	- server`s socket file descriptor
 * *sa			- pointer to struct sockaddr
 * domain		- AF_UNIX, AF_INET, ...
 * type			- SOCK_STREAM, SOCK_DGRAM, ...
 * max_client	- defined on common.h
*/
int init_server_socket(int *socket_fd, struct sockaddr_in *sa, int domain, int type, int protocol);


void server_loop(int server_socket_fd, int *status);

#endif /* SERVER_H */

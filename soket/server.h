#ifndef SERVER_H
#define SERVER_H


/*
 * socket_fd		- server`s socket file descriptor
 * *sa			- pointer to struct sockaddr
 * domain		- AF_UNIX, AF_INET, ...
 * type			- SOCK_STREAM, SOCK_DGRAM, ...
 * max_client		- defined on common.h
 * *status 		- if 0 stop loop
*/

void server_loop(int *status, struct sockaddr *sa, int domain, int type, int protocol);

#endif /* SERVER_H */

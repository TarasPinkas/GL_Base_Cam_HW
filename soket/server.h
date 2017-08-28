#ifndef SERVER_H
#define SERVER_H


/*
 * domain		- AF_UNIX, AF_INET, ...
 * type			- SOCK_STREAM, SOCK_DGRAM, ...
 * protocol 		- 0
*/

int server_loop(int domain, int type, int protocol);

#endif /* SERVER_H */

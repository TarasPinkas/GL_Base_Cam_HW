#ifndef CLIENT_H
#define CLIENT_H

void init_client_sock(int *client_fd, struct sockaddr_in *sa);

void client_loop(int *client_fd, struct sockaddr_in *sa);

#endif

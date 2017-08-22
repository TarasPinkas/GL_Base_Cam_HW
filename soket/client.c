#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

void init_client_sock(int *client_fd, struct sockaddr_in *sa)
{
	*client_fd = socket(
}

void client_loop(int *client_fd, struct sockaddr_int *sa)
{
}

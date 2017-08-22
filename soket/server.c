#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <pthread.h>

#include "server.h"
#include "common.h"


int init_server_socket(int *socket_fd, struct sockaddr_in *sa, int domain, int type, int protocol)
{
	int retval = 0;
	errno = 0;
	*socket_fd = socket(domain, type, protocol);

	if ( !*socket_fd)
	{
		retval = ERROR_CANT_CREATE;
		goto ERROR;
	}

	errno = 0;
	if (!bind(*socket_fd, (struct sockaddr *) sa, sizeof(*sa)))
	{
		retval = ERROR_CANT_BIND;
		goto ERROR;
	}

	errno = 0;
	if (!listen(*socket_fd, MAX_CLIENTS))
	{
		retval = ERROR_CANT_LISTEN;
		goto ERROR;
	}

	return retval;

ERROR:
	print_error(errno);
	close(*socket_fd);
	return retval;
}


void print_error(int error_code)
{
	fprintf(stderr, "Error.  %s\n", strerror(error_code));
}

static void *thread_func(void *arg)
{
	int client_fd = *(int *)arg;
	char client_message[MAX_MESSAGE_SIZE] = { 0 };
	int read_size = 0;

	while(!read_size)
	{
		read_size = recv(client_fd, client_message,
				sizeof(client_message), 0);
		switch(read_size)
		{
			case 0:
				perror("CLient disconected\n");
				break;

			case -1:
				perror("recv failed\n");
				break;

			default:
				printf("Client say:\n\t%s\n", client_message);
				write(client_fd, client_message, strlen(client_message));
				break;
		}
	}

	close(client_fd);

	return 0;
}

void server_loop(int server_fd, int *status)
{
	int client_fd[MAX_CLIENTS];
	struct sockaddr_in client;
	socklen_t client_socklen = sizeof(struct sockaddr_in);

	pthread_t threads[MAX_CLIENTS];
	int i = 0;						/* tread counter */


	while(*status)
	{
		client_fd[i] = accept(server_fd, (struct sockaddr *)&client, &client_socklen);

		if (!client_fd[i])
		{
			sleep(1);
			continue;
		}

		if(!pthread_create(&threads[i], NULL, thread_func, (int *)&client_fd[i]))
		{
			sleep(1);
			continue;
		}
		i++;

		if( i == MAX_CLIENTS)
		{
			for (i = 0; i < MAX_CLIENTS; i++)
			{
				pthread_join(threads[i], NULL);
			}

			memset(client_fd, 0, sizeof(client_fd));
			i = 0;
		}
	}

	for (i = 0; i < MAX_CLIENTS; i++)
	{
		pthread_join(threads[i], NULL);
	}
}

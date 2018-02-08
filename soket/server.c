#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <pthread.h>

#include "server.h"
#include "common.h"

#define DEBUG


static void *thread_func(void *arg);


static int status_property(int s)
{
	static int status;

	if (s)
	{
		status = s;
	}
	return status;
}

/*
 * socket_fd 	- server`s socket descriptor
 * *sa 		- pointer to strucrt sockaddr
 * domaint	- AF_UNIX, AF_INET, ...
 * type		- SOCK_STREAM, SOCK_DGRAM, ...
 * max_client	- defined on common.h
 * 
*/
static int init_server_socket(int *socket_fd, struct sockaddr *sa, int domain, int type, int protocol)
{
	int retval = 0;
	*socket_fd = socket(domain, type, protocol);

	do {

		errno = 0;
		if (*socket_fd < 0)
		{
			fprintf(stderr, "Server create error: %s\r\n", strerror(errno));
			retval = errno;
			break;
		}
	
		errno = 0;
		if (bind(*socket_fd, (struct sockaddr *) sa, sizeof(*sa)) < 0)
		{
			fprintf(stderr, "Server bind error: %s\r\n", strerror(errno));
			retval = errno;
			break;
		}
	
		errno = 0;
		if (listen(*socket_fd, MAX_CLIENTS) < 0)
		{
			fprintf(stderr, "Server listen error: %s\r\n", strerror(errno));
			retval = errno;
			break;
		}
	} while(0);

	if(retval)
	{
		close(*socket_fd);
	}
	return retval;
}





/*
 * socket_fd 	- server`s socket descriptor
 * *sa 		- pointer to strucrt sockaddr
 * domaint	- AF_UNIX, AF_INET, ...
 * type		- SOCK_STREAM, SOCK_DGRAM, ...
 * max_client	- defined on common.h
 * *status 	- if 0 stop loop 
*/

void server_loop(int *status, struct sockaddr *sa, int domain, int type, int protocol)
{
	/* Server */
	int server_fd;

	/* Cleit */
	int client_fd[MAX_CLIENTS];
	struct sockaddr_in client;
	socklen_t client_socklen = sizeof(struct sockaddr_in);

	pthread_t threads[MAX_CLIENTS];
	int i = 0, s = 0;				/* tread counter */

	memset(client_fd, 0, sizeof(client_fd));


	status_property(*status);

	if (init_server_socket(&server_fd, sa, domain, type, protocol))
	{
		fprintf(stderr, "Init faild\n");
		close(server_fd);
		exit(1);
	}

	while(status_property(0) && i < MAX_CLIENTS)
	{
		client_fd[i] = accept(server_fd, (struct sockaddr *)&client, &client_socklen);

		if (client_fd[i] < 0)
		{
			sleep(1);
			client_fd[i] = 0;
			continue;
		}

		s = 0;
		while(pthread_create(&threads[i], NULL, thread_func, (void*)((intptr_t)client_fd[i])) < 0)
		{
			if (s == 5)
			{
				break;
			}
			s++;
			sleep(1);
		}

		i++;
	}

	for (i = 0; i < MAX_CLIENTS; i++)
	{
		pthread_join(threads[i], NULL);
	}

	close(server_fd);
}




static void *thread_func(void *arg)
{
	int client_fd = (int) ((intptr_t)arg);
	char client_message[MAX_MESSAGE_SIZE];
	int read_size = 1;

	memset(client_message, '\0', sizeof(client_message));

	while(read_size && status_property(0))
	{
		errno = 0;
		read_size = recv(client_fd, client_message,
				sizeof(client_message), 0);

		switch(read_size)
		{
			case 0:
				fprintf(stderr, "CLient disconected\n");
				pthread_exit(NULL);
				break;

			case -1:
				fprintf(stderr, "Server receiv error: %s\r\n\n", strerror(errno));
				pthread_exit(NULL);
				break;

			default:
				printf("Server got:\n\t%s", client_message);

				if(!strcmp(client_message, FINAL_MES))
				{
					printf("Server get end message\r\n");
					close(client_fd);
					pthread_exit(NULL);
				}

				write(client_fd, client_message, strlen(client_message));
				break;
		}
	}

	close(client_fd);
	pthread_exit(NULL);
}



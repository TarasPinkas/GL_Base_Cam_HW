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


static inline void print_error(int error_code)
{
	char *str;

	switch(error_code)
	{
		case NO_ERROR:
			str = "";
			break;
		case ERROR_CANT_CREATE:
			str = "Can`t create socket\n";
			break;
		case ERROR_CANT_BIND:
			str = "Bind faild\n";
			break;
		case ERROR_CANT_LISTEN:
			str = "Listen failture\n";
			break;
		default:
			str = strerror(error_code);
	}


	fprintf(stderr, "Error server. %s\n", str);
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

	if (*socket_fd < 0)
	{
		retval = ERROR_CANT_CREATE;
		goto ERROR;
	}
	errno = 0;
	if (bind(*socket_fd, (struct sockaddr *) sa, sizeof(*sa)) < 0)
	{
		fprintf(stderr, "Bind error. %s\n", strerror(errno));
		retval = ERROR_CANT_BIND;
		goto ERROR;
	}

	if (listen(*socket_fd, MAX_CLIENTS) < 0)
	{
		retval = ERROR_CANT_LISTEN;
		goto ERROR;
	}

	return retval;

ERROR:
	print_error(retval);
	close(*socket_fd);
	return retval;
}



static void *thread_func(void *arg)
{
	int client_fd = (int) ((intptr_t)arg);
	char client_message[MAX_MESSAGE_SIZE] = { 0 };
	int read_size = 1;

	while(read_size)
	{
		read_size = recv(client_fd, client_message,
				sizeof(client_message), 0);
		switch(read_size)
		{
			case 0:
				perror("CLient disconected\n");
				read_size = 1;
				break;

			case -1:
				perror("recv failed\n");
				read_size = 1;
				break;

			default:
				printf("Client say:\n\t%s\n", client_message);

				if(!strcmp(client_message, FINAL_MES))
				{
					close(client_fd);
					return NO_ERROR;
				}

				write(client_fd, client_message, strlen(client_message));
				break;
		}
	}

	close(client_fd);

	return NO_ERROR;
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
	int i = 0;				/* tread counter */
	
	if (init_server_socket(&server_fd, sa, domain, type, protocol))
	{
		fprintf(stderr, "Debag. Init faild\n");
		close(server_fd);
		exit(1);	
	}	

	while(*status)
	{
		client_fd[i] = accept(server_fd, (struct sockaddr *)&client, &client_socklen);

		if (!client_fd[i])
		{
			sleep(1);
			continue;
		}
		
		int s = 0;
		while(!pthread_create(&threads[i], NULL, thread_func, (void*)((intptr_t)client_fd[i]) ))
		{
			if (s == 5)
			{
				continue;
			}
			s++;
			sleep(1);
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
	close(server_fd);
}

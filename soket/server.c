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


static void *thread_func(void *arg);

static int init_server_socket(int *socket_fd, struct sockaddr *sa, 
		int domain, int type, int protocol)
{
	errno = 0;
	*socket_fd = socket(domain, type, protocol);
	
	if (*socket_fd < 0)
	{
		fprintf(stderr, "Server create error: %s\r\n", strerror(errno));
		return  ERROR_CANT_CREATE;
	}

	errno = 0;
	if (bind(*socket_fd, (struct sockaddr *) sa, sizeof(*sa)) < 0)
	{
		fprintf(stderr, "Server bind error: %s\r\n", strerror(errno));
		return ERROR_CANT_BIND;
	}

	errno = 0;
	if (listen(*socket_fd, MAX_CLIENTS) < 0)
	{
		fprintf(stderr, "Server listen error: %s\r\n", strerror(errno));
		return ERROR_CANT_LISTEN;
	}
	
	return NO_ERROR;
}



int server_loop(int domain, int type, int protocol)
{
	int retval = 0;
	/* Server */
	int server_fd;
	struct sockaddr_in sa =
	{
		.sin_family		= domain,
		.sin_addr.s_addr	= INADDR_ANY,
		.sin_port		= SERVER_PORT
	};

	/* Client */
	int client_fd[SERVER_BUF] = {0};
	struct sockaddr_in client;
	socklen_t client_socklen = sizeof(struct sockaddr_in);
	
	/* Threads */
	pthread_t threads[SERVER_BUF];
	int thread_index = 0, try_counter = 0;
	
	/* Init server socket */
	if ((retval = init_server_socket(&server_fd, (struct sockaddr *)&sa, 
				domain, type, protocol)))
	{
		fprintf(stderr, "Init faild\n");
		close(server_fd);
		return retval;	
	}	

	//while(thread_index < SERVER_BUF)
	while(1)
	{
		/* check free thread_index */
		while(client_fd[thread_index] != 0)
		{
			if (pthread_tryjoin_np(threads[thread_index], NULL))
			{
				thread_index = (thread_index + 1) % SERVER_BUF;
			}	
			else
			{
				client_fd[thread_index] 	= 0;
				threads[thread_index] 		= 0;
			}
		}

		client_fd[thread_index] = accept(server_fd,
			       	(struct sockaddr *)&client, &client_socklen);

		if (client_fd[thread_index] < 0)
		{
			sleep(1);
			client_fd[thread_index] = 0;
			continue;
		}
		
		/* creadte new thread from new client */
		try_counter = 0;
		while(pthread_create(&threads[thread_index], NULL, thread_func, 
				(void*)((intptr_t)client_fd[thread_index])) < 0)
		{
			if (try_counter == 5)
			{
				break;
			}
			try_counter++;
			sleep(1);
		}
		
		thread_index = (thread_index + 1) % SERVER_BUF;
	}

	/* wait while all threads closed */
	for (thread_index = 0; thread_index < SERVER_BUF; thread_index++)
	{
		pthread_join(threads[thread_index], NULL);

	}

	printf("\nServer closed\r\n\n");
	close(server_fd);
	return retval;
}




static void *thread_func(void *arg)
{
	int client_fd = (int) ((intptr_t)arg);
	char client_message[MAX_MESSAGE_SIZE];
	int read_size = 1;

	memset(client_message, '\0', sizeof(client_message));
	
	while(read_size)
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
				fprintf(stderr, "Server receiv error: %s\r\n\n",
					       	strerror(errno));
				pthread_exit(NULL);
				break;

			default:
				printf("Server got:\n\t%s", client_message);

				if(!strcmp(client_message, FINAL_MES))
				{
					close(client_fd);
					pthread_exit(NULL);
				}

				write(client_fd, client_message, 
						strlen(client_message));
				break;
		}
	}

	close(client_fd);
	pthread_exit(NULL);
}



#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include <errno.h>


#include "common.h"
#include "client.h"


static void* thread_func(void *);



void client_loop()
{
	int client_count = 0;
	int i = 0;
	pthread_t threads[MAX_CLIENTS];


	while (client_count < MAX_CLIENTS)
	{
		pthread_create(&threads[client_count], NULL, thread_func,
				(void *)((intptr_t)client_count));
		client_count++;
	}


	for (i = 0; i < MAX_CLIENTS; i++)
	{
		pthread_join(threads[i], NULL);
	}
}


/*
 * return:
 *	NO_ERROR			if succses
 *	ERROR_CANT_CREATE	if cant`n create socket
 *	ERROR_CANT_CONNECT	if cant`n connect to server
 */
static void* thread_func(void *arg)
{
	int thread_id = (int) ((intptr_t) arg);
	int sock_fd = 0;
	int mes_counter = 0;

	char messag[MAX_MESSAGE_SIZE] = {0};

	struct sockaddr_in server =
	{
		.sin_family			= DOMAIN,
		.sin_addr.s_addr	= SERVER_ADDR,
		.sin_port			= SERVER_PORT
	};

	sock_fd = socket(DOMAIN, TYPE, PROTOCOL);

	if(sock_fd == -1)
	{
		fprintf(stderr, "Could not create client(%d) socket\r\n", thread_id);
		close(sock_fd);
		return (void *)((intptr_t)ERROR_CANT_CREATE);
	}

	if (connect(sock_fd, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		fprintf(stderr, "Client(%d) can`t connect\r\n", thread_id);
		close(sock_fd);
		return (void *)((intptr_t)ERROR_CANT_CONNECT);
	}

	printf("Client %d started\r\n", thread_id);

	while(mes_counter < MAX_CLIENT_MES)
	{
		snprintf(messag, MAX_MESSAGE_SIZE - 1, "%s %d\r\n", "Thread: ", thread_id);

		if (send(sock_fd, messag, strlen(messag), 0) < 0)
		{
			fprintf(stderr, "Client(%d) can`t send the message\r\n", thread_id);
			continue;
		}

		if (recv(sock_fd, messag, MAX_MESSAGE_SIZE - 1, 0) < 0)
		{
			fprintf(stderr, "Client(%d) recv failed\r\nErrno: %s\r\n",
					thread_id, strerror(errno));
			continue;
		}

		print("Client(%d) got: \n\t%.*s\r\n", thread_id, (int)strlen(messag), messag);
		mes_counter++;
	 }

	 send(sock_fd, FINAL_MES, strlen(FINAL_MES), 0);

	 close(sock_fd);

	 return (void *)((intptr_t) NO_ERROR);
}

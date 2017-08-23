#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>

#include "common.h"
#include "client.h"

static void* thread_func(void *);

void client_loop(int *status)
{
	int client_count = 0;
	int i = 0;
	pthread_t threads[MAX_CLIENTS];

	while (*status && client_count < MAX_CLIENTS)
	{
		pthread_create(&threads[client_count], NULL, thread_func, (void *)((intptr_t)client_count));
		client_count++;
		sleep(1);
	}

	for (i = client_count; i >= 0; i--)
	{
		pthread_join(threads[i], NULL);
	}
}



static void* thread_func(void *arg)
{
	int thread_id = (int) ((intptr_t) arg);
	int sock_fd = 0;
	int mes_counter = 0;

	char messag[MAX_MESSAGE_SIZE] = { 0 };

	struct sockaddr_in server = 
	{ 
		.sin_family 		= DOMAIN,
		.sin_addr.s_addr 	= SERVER_ADDR,
		.sin_port 		= SERVER_PORT
	};

	sock_fd = socket(DOMAIN, TYPE, PROTOCOL);
	
	if(sock_fd == -1)
	{
		perror("Could not create client socket\n");
		return (int *)ERROR_CANT_CREATE;
	}	
	
	if (connect(sock_fd, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		perror("Client can`t connect\n");
		return (int *)ERROR_CANT_CONNECT;
	}

	 while(mes_counter < MAX_CLIENT_MES)
	 {
		sprintf(messag, "%s %d\n", "Thread: ", thread_id);
		if(send(sock_fd, messag, strlen(messag), 0) < 0)
	 	{
			perror("Client can`t send the message\n");
			continue;
		}	

		if (recv(sock_fd, messag, MAX_MESSAGE_SIZE, 0) < 0)
		{
			perror("Client`s recv failed\n");
			continue;
		}

		printf("Got: %.*s\n", (int)strlen(messag), messag);
		mes_counter++;
	 }
	
	 recv(sock_fd, FINAL_MES, sizeof(FINAL_MES), 0);

	 close(sock_fd);
	 return NO_ERROR;
}

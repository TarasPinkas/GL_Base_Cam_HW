#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <pthread.h>


#include "server.h"
#include "client.h"
#include "../common.h"

void client_process();		/* create a lot of clients */
void *thread_func(void *);

int main(void)
{
	errno = 0;
	switch(fork())
	{
		case -1:
			perror("Can`t create new procces\r\n");
			break;
		case 0:	/* child process */
			if (server_loop(DOMAIN, TYPE, PROTOCOL))
			{
				perror("Server can`t start\r\n");
				exit(1);
			}
			break;

		default:
			client_process();
			printf("clients succses.\r\n");
			wait(NULL);
			break;
	}
	return 0;
}

void client_process()
{
	int i = 0;

	pthread_t thread[MAX_CLIENTS] = {0};

	for (i = 0; i < MAX_CLIENTS; i++)
	{
		if (pthread_create(&thread[i], NULL, thread_func, NULL))
		{
			perror("Thread create faild\r\n");
			continue;
		}
	}

	for (i = 0; i < MAX_CLIENTS; i++)
	{
		pthread_join(thread[i], NULL);
	}
}

void *thread_func(void * i)
{
	client_loop();
	pthread_exit(NULL);
}

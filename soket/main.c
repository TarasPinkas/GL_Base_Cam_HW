#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>



#include "server.h"
#include "client.h"
#include "common.h"

const int MAX_CLIENTS		= 7;
const int MAX_CLIENT_MES	= 3;
const int MAX_MESSAGE_SIZE	= 2000;

int status = 1;

void sig_handler(int sig)
{
	printf("\rGet SIGINT(%d). Application stoped...\n", sig);
	status = 0;
}


int main(void)
{
	struct sockaddr_in server =
	{
		.sin_family			= DOMAIN,
		.sin_addr.s_addr	= INADDR_ANY,
		.sin_port			= SERVER_PORT
	};


//	signal(SIGINT, sig_handler);

	switch (fork())
	{
		case 0:		/* child procces. TCP server */
			server_loop(&status, (struct sockaddr *)&server, DOMAIN, TYPE, PROTOCOL);
			break;

		case -1:	/* Error */
			perror("Can`t create new process\n");
			break;

		default:	/* parent procces. TCP client */
			client_loop(&status);
			wait(NULL);
			break;
	}

	return 0;
}

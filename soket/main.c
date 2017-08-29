#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <stdlib.h>


#include "server.h"
#include "client.h"
#include "common.h"


int main(void)
{
	int retval = 0;


	switch (fork())
	{
		case 0:		/* child procces. TCP server */
			if ((retval = server_loop(DOMAIN, TYPE, PROTOCOL)))
			{
				fprintf(stderr, "Error. error code = %d\r\n", retval);
				exit(retval);
			}
			break;

		case -1:	/* Error */
			perror("Can`t create new process\n");
			break;

		default:	/* parent procces. TCP client */
			client_loop();
			print("Client loop closed\r\n");
			wait(NULL);
			break;
	}

	return retval;
}

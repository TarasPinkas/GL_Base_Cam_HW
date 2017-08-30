#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#include "../common.h"

#define MES_TO_SERV		"Hi server"

int client_loop()
{
	char message[MAX_MESSAGE_SIZE] = {0};
	int read_size = 0;
	int mes_counter = 0;
	int sock_fd = 0;

	struct sockaddr_in server_sa =
	{
		.sin_family		= DOMAIN,
		.sin_addr.s_addr	= SERVER_ADDR,
		.sin_port		= SERVER_PORT
	};

	socklen_t sockaddr_len = sizeof(struct sockaddr_in);

	sock_fd = socket(DOMAIN, TYPE, PROTOCOL);
	if (sock_fd < 0)
	{
		perror("Client socket faild\r\n");
		close(sock_fd);
		return ERROR_CANT_CREATE;
	}

	print("Client started\r\n");
	while(mes_counter < MAX_CLIENT_MES)
	{
		while(sendto(sock_fd, MES_TO_SERV, strlen(MES_TO_SERV), 0, (const struct sockaddr_in *)&server_sa,
				sizeof(server_sa)) < 0)
		{
			fprintf(stderr, "Client can`t send the message\r\n");
			sleep (1);
		}

		read_size = recvfrom(sock_fd, message, MAX_MESSAGE_SIZE, 0,
					(struct sockaddr_in *)&server_sa, &sockaddr_len);

		switch(read_size)
		{
			case -1:

			case 0:
				perror("Client recv failed\r\n");
				break;
		default:
				print("Client got mes: %.*s from %s:%d\r\n", read_size, message,
					       inet_ntoa(server_sa.sin_addr), ntohs(server_sa.sin_port));

				break;
		}
		mes_counter++;
	}

	close(sock_fd);
	return NO_ERROR;
}



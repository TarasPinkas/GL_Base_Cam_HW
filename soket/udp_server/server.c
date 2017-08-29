#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "../common.h"

static int server_init(int *server_fd, int domain, int type, int protocol)
{
	struct sockaddr_in sa =
	{
		.sin_family			= domain,
		.sin_addr.s_addr	= INADDR_ANY,
		.sin_port			= SERVER_PORT
	};

	errno = 0;
	*server_fd = socket(domain, type, protocol);

	if(*server_fd < 0)
	{
		perror("Server create failed");
		return  ERROR_CANT_CREATE;
	}

	errno = 0;

	if(bind(*server_fd, (struct sockaddr *)&sa, sizeof(sa)) < 0)
	{
		perror("Server bind failed");
		return  ERROR_CANT_CREATE;
	}

	return NO_ERROR;
}


int server_loop(int domain, int type, int protocol)
{
	int server_fd = 0;

	struct sockaddr_in client_sa;
	socklen_t client_socklen = sizeof(struct sockaddr_in);

	int read_size = 0;
	char message[MAX_MESSAGE_SIZE] = {0};
	char str[] = "Hi client";

	errno = 0;
	if (server_init(&server_fd, domain, type, protocol))
	{
		perror("Server init failed");
		close(server_fd);
		return ERROR_CANT_INIT;
	}

	print("Server started\r\n");

	while(1)
	{
		errno = 0;
		read_size = recvfrom(server_fd, message, sizeof(message), 0,
						(struct sockaddr *)&client_sa, &client_socklen);
		switch(read_size)
		{
			case 0:
				fprintf(stderr, "Server notheng to recv\r\n");
				break;

			case -1:
				perror("Server recv failed\r\n");
				break;

			default:
				print("Server got message: %.*s from %s, port %d\r\n",
					read_size, message, inet_ntoa(client_sa.sin_addr),
					ntohs(client_sa.sin_port));

				snprintf(message, MAX_MESSAGE_SIZE, "%s %s:%d\r\n", str,
						inet_ntoa(client_sa.sin_addr), ntohs(client_sa.sin_port));

				if ((read_size = sendto(server_fd, message, sizeof(message), 0,
							(struct sockaddr *)&client_sa,
								sizeof(client_sa))) <= 0)
				{
					fprintf(stderr, "Server can`t send data to %s:%d\r\n",
							inet_ntoa(client_sa.sin_addr),
							ntohs(client_sa.sin_port));
				}
				break;

		}
	}
	close(server_fd);
}




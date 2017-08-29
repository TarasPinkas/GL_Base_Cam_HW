#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <linux/if_ether.h>
#include <linux/ip.h>


#define BUF_SIZE	65536

void print_data(const char *, const int);	/* print buffer */


struct sockaddr_in sa_in;

char *get_protocol_name(int protocol)
{
	switch(protocol)
	{
		case 6:		/* TCP protocol */
			return "TCP";
		case 17:	/* UDP protocol */
			return "UDP";
		case 1:		/* ICMP protocol */
			return "ICMP";
		default:
			return "OTHER";
	}
}

char *get_eth_type(int type)
{
	switch(type)
	{
		case 0x0800:
			return "IP";
		case 0x86dd:
			return "IPv6";
		default:
			return "OTHER";
	}
}
void print_ip_hdr(struct iphdr *iph)
{
	struct sockaddr_in s, d;

	s.sin_addr.s_addr = iph->saddr;
	d.sin_addr.s_addr = iph->daddr;

	printf("\nIP header:\r\n");
	printf("\tTotal len: %d\r\n", iph->tot_len);
	printf("\tId: %d\r\n", iph->id);
	printf("\tTTL: %d\r\n", iph->ttl);
	printf("\tProtocol: %d(%s)\r\n", iph->protocol,
			get_protocol_name(iph->protocol));
	printf("\t%-15s%s\r\n\t%-15s%s\r\n","Sourse: ", inet_ntoa(s.sin_addr),
			"Destination:", inet_ntoa(d.sin_addr));
}
void print_eth_hdr(struct ethhdr *eth)
{
	int i = 0;


	printf("\nEthernet header:\r\n\t%-15s", "Source: ");
	for (i = 0; i < ETH_ALEN; i++)
	{
		printf("%02X ", eth->h_source[i]);
	}
	printf("\r\n\t%-15s", "Destination: ");
	for (i = 0; i < ETH_ALEN; i++)
	{
		printf("%02X ", eth->h_dest[i]);
	}
	printf("\r\n\t");
	printf("%-14s %04X(%s)\r\n", "Type: ", ntohs(eth->h_proto), get_eth_type(ntohs(eth->h_proto)));
}

void print_info(char *buffer, int size)
{
	struct iphdr *iph = ((struct iphdr *)(buffer + sizeof(struct ethhdr)));
	struct ethhdr *eth = (struct ethhdr *) buffer;


	print_eth_hdr(eth);
	print_ip_hdr(iph);

	print_data(buffer, size);
}

void print_data(const char *buf, const int read_size)
{
	int i = 0;

	printf("\nData:\r\n\t");
	for (i = 0; i < read_size; i++)
	{
		printf("%X ", (unsigned char)buf[i]);
	}
	printf("\r\n");

}
int main(void)
{
	int sock_fd;
	struct sockaddr_in sa;
	int sa_size = sizeof(struct sockaddr_in), read_size = 0;
	char buffer[BUF_SIZE] = {0};

	memset(&sa, 0, sa_size);

	sock_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if(sock_fd < 0)
	{
		perror("Can`t create socket\r\n");
		exit(1);
	}

	while(1)
	{
		read_size = recvfrom(sock_fd, buffer, BUF_SIZE, 0,(struct sockaddr *) &sa, &sa_size);

		switch(read_size)
		{
			case -1: /* error */
				perror("Recv failed\r\n");
				break;

			case 0:
				break;

			default:
				print_info(buffer, read_size);

				break;
		}
	}
}

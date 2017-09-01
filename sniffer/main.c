#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>

#define TCP		6
#define UDP		17

#define BUF_SIZE	65536

void print_tcp(const char *, const int);
void print_udp(const char *, const int);


void print_data(const char *, const int);	/* print buffer */


struct sockaddr_in sa_in;

char *get_protocol_name(int protocol)
{
	switch(protocol)
	{
		case TCP:		/* TCP protocol */
			return "TCP";
		case UDP:	/* UDP protocol */
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

void print_tcp(const char *buf, const int size)
{
	int len = sizeof (struct ethhdr) + sizeof(struct iphdr);
	struct tcphdr *tcp = (struct tcphdr *)(buf + len);

	printf("TCP header:\r\n");

	printf("\t%s%u\r\n", "Source port: ", ntohs(tcp->source));
	printf("\t%s%u\r\n", "Destination port: ", ntohs(tcp->dest));
	printf("\t%s%u\r\n", "Header length: ", (unsigned int)tcp->doff * 4);
	printf("\r\n");
}

void print_udp(const char *buf, const int size)
{
	struct udphdr *udp = (struct udphdr *)(buf + sizeof (struct ethhdr) + sizeof(struct iphdr));

	printf("UDP header:\r\n");

	printf("\t%s%u\r\n", "Source port: ", ntohs(udp->source));
	printf("\t%s%u\r\n", "Destination port: ", ntohs(udp->dest));
	printf("\t%s%u\r\n", "Header length: ", ntohs(udp->len));
	printf("\r\n");
}


/* retrun type of inernet protocol */
int print_ip_hdr(const char *buf, const int size)
{
	struct iphdr *iph = (struct iphdr *)(buf + sizeof(struct ethhdr));
	struct sockaddr_in s, d;

	s.sin_addr.s_addr = iph->saddr;
	d.sin_addr.s_addr = iph->daddr;

	printf("\nIP header:\r\n");
	printf("\tTotal len: %d\r\n",	ntohs(iph->tot_len));
	printf("\tId: %d\r\n",			iph->id);
	printf("\tTTL: %d\r\n",			iph->ttl);
	printf("\tProtocol: %d(%s)\r\n", iph->protocol,
			get_protocol_name(iph->protocol));
	printf("\t%-15s%s\r\n\t%-15s%s\r\n","Sourse: ", inet_ntoa(s.sin_addr),
			"Destination:", inet_ntoa(d.sin_addr));


	return iph->protocol;
}
int print_eth_hdr(const char *buf, const int size)
{
	int i = 0;

	struct ethhdr *eth = (struct ethhdr *)buf;

	printf("\nEthernet header:\r\n\t%-15s", "Source: ");
	/* printf Source Mac */
	for (i = 0; i < ETH_ALEN; i++)
	{
		printf("%02X ", eth->h_source[i]);
	}

	printf("\r\n\t%-15s", "Destination: ");
	/* printf Des MAC */
	for (i = 0; i < ETH_ALEN; i++)
	{
		printf("%02X ", eth->h_dest[i]);
	}
	printf("\r\n\t");

	printf("%-14s %04X(%s)\r\n", "Type: ", ntohs(eth->h_proto), get_eth_type(ntohs(eth->h_proto)));

	return ntohs(eth->h_proto);
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


/*	*/
/* Print all info  */
/*	*/
void print_info(char *buffer, int size)
{
	int proto = 0;
	print_eth_hdr(buffer, size);	/* print Ethernet protocol */
	proto = print_ip_hdr(buffer, size);		/* print Ip protocol	*/

	switch(proto)
	{
		case UDP:
			print_udp(buffer, size);
			break;
		case TCP:
			print_tcp(buffer,size);
			break;
		default:
			printf("Don`t now this(%d) protocol\r\n", proto);
			break;
	}

	print_data(buffer, size);		/* print data	*/
}
/*	*/
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

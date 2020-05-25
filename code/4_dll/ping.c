#include "ping.h"

struct sockaddr_ll sll;

/*
unsigned char myip[4]={88,80,187,84};
unsigned char mymac[6]={0xf2, 0x3c, 0x91,};
unsigned char dest_ip={};
*/
int main(int argc, char** argv)
{
	int sd;
    socklen_t sll_len;
	unsigned char buffer[BUFFER_SIZE];
	size_t pkt_len;

	sd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	sll.sll_family = AF_PACKET;
	sll.sll_ifindex = if_nametoindex("eth0");
	
	sll_len = sizeof(sll);

	pkt_len = recvfrom(sd, buffer, BUFFER_SIZE, 0, (struct sockaddr*) &sll, &sll_len);
	 
	if(pkt_len == -1)
	{
		printf("Errno: %d\n", errno);
		perror("Error receiving the packet");
		exit(1);
	}
		
	print_packet(buffer, pkt_len);

	return 0;
}

void print_packet(unsigned char*buff, int len){
	int i=0;
	for(; i<len; i++)
	{
		if(i%4==0)
			printf("\n");

		printf("%.2x (%.3d)", buff[i], buff[i]);
	}
}

#include <stdio.h>
#include <arpa/inet.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h> /* the L2 protocols */
#include <net/if.h>
#include <string.h>
#include "utility.h"

unsigned char mymac[6]={0x4c,0xbb,0x58,0x5f,0xb4,0xdc};
unsigned char targetmac[6];
unsigned char buffer[1500];
int s;
struct sockaddr_ll sll;

struct eth_frame
{
    unsigned char dst[6];
    unsigned char src[6];
    unsigned short type;
    unsigned char payload[1460];
};

struct arp_packet
{
    unsigned short hw;
    unsigned short proto;
    unsigned char hlen;
    unsigned char plen;
    unsigned short op;
    unsigned char srcmac[6];
    unsigned char srcip[4];
    unsigned char dstmac[6];
    unsigned char dstip[4];
};

struct ip_datagram
{
    unsigned char ver_ihl;
    unsigned char tos;
    unsigned short len;
    unsigned short id;
    unsigned short flag_offs;
    unsigned char ttl;
    unsigned char proto;
    unsigned short  checksum;
    unsigned int src;
    unsigned int dst;
    unsigned char payload[1480];
};

struct icmp_packet 
{
    unsigned char type;
    unsigned char code;
    unsigned short checksum;
    unsigned int unused;
    unsigned char payload[84];
};

unsigned char packet[1500];

int main()
{
    int i,n,len ;
    unsigned char mac_addr[6];
    unsigned char ip_addr[4];
    struct eth_frame * eth;
    struct ip_datagram * ip;
    struct icmp_packet * icmp;

    s = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)); 
    if(s==-1){perror("socket failed");return 1;}

    sll.sll_family=AF_PACKET;
    sll.sll_ifindex = if_nametoindex("eth0");
    len=sizeof(sll);
    
    eth = (struct eth_frame *) packet;
    ip = (struct ip_datagram *) eth->payload; 
    icmp = (struct icmp_packet *) ip->payload;

    while( 1 )
    {
        len=sizeof(sll);
        n=recvfrom(s,packet,1500, 0,(struct sockaddr *)&sll,&len);
    
        if (n == -1) 
        {
            perror("Recvfrom failed"); 
            return 0;
        }

        if (eth->type == htons (0x0800)) //it is IP
        {
            if(ip->proto == 1) // it is ICMP 
            {
                if(icmp->type==8)
                {
                    unsigned short checksum_IP = ip->checksum;
                    unsigned short checksum_ICMP = icmp->checksum;
                    ip->checksum = 0;
                    icmp->checksum = 0;

                    unsigned int ip_HEADER_length = (ip->ver_ihl & 0x0F) * 4;
                    unsigned int ICMP_length = ntohs(ip->len) - ip_HEADER_length;
                    printf("\n%sIP HEADER length:%s %u   %sICMP length:%s %u\n", BOLD_RED, DEFAULT, ip_HEADER_length, BOLD_BLUE, DEFAULT, ICMP_length);
                    
                    if((ip->checksum = htons(checksum((unsigned char*) ip, ip_HEADER_length)))==checksum_IP &&
                       (icmp->checksum = htons(checksum((unsigned char*) icmp, ICMP_length)))==checksum_ICMP)
                    {
                        memcpy(mac_addr, eth->dst, 6);
                        memcpy(eth->dst, eth->src, 6);
                        memcpy(eth->src, mac_addr, 6);
                        
                        memcpy(ip_addr, (unsigned char*) &(ip->dst), 4);
                        memcpy((unsigned char*) &(ip->dst), (unsigned char*) &(ip->src), 4);
                        memcpy((unsigned char*) &(ip->src), ip_addr, 4);
                        
                        print_packet(packet,14+ip_HEADER_length+ICMP_length, BOLD_YELLOW);

                        icmp->type = 0;
                        icmp->checksum = 0;
                        icmp->checksum = htons(checksum((unsigned char*) icmp, ICMP_length));

                        for(i=0;i<sizeof(sll);i++) ((char *)&sll)[i]=0;

                        sll.sll_family=AF_PACKET;
                        sll.sll_ifindex = if_nametoindex("eth0");
                        len=sizeof(sll);

                        n=sendto(s,packet, n, 0,(struct sockaddr *)&sll,len);
                        
                        if (n == -1) 
                        {
                            perror("Recvfrom failed"); 
                            return 0;
                        }
                    }
                }
            }
        }
    }
    return 0;
}

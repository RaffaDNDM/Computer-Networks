#include <stdio.h>
#include <arpa/inet.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h> /* the L2 protocols */
#include <net/if.h>
#include <string.h>

#include "utility.h"
#define LINE "_______________________________"

unsigned char myip[4]={88,80,187,84};
unsigned char netmask[4]={255,255,255,0};
unsigned char mymac[6]={0xf2,0x3c,0x91,0xdb,0xc2,0x98};
unsigned char gateway[4]={88,80,187,1};

//unsigned char targetip[4]={88,80,187,50};
unsigned char targetip[4]={212,71,253,5};
unsigned char targetmac[6];
unsigned char buffer[1500];
int s;
struct sockaddr_ll sll;

int printpacket(unsigned char *b,int l){
int i;
 for(i=0;i<l;i++){
	printf("%.2x(%.3d) ",b[i],b[i]);  
	if(i%4 == 3) printf("\n");
	}
	printf("\n%s\n", LINE);
}	
	
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
    unsigned short checksum;
    unsigned int src;
    unsigned int dst;
    unsigned char option[40];
    unsigned char payload[1441];
};

int forge_ip(struct ip_datagram *ip, unsigned char * dst, int payloadlen,unsigned char proto) 
{
    ip->ver_ihl=0x4F;
    ip->tos=0;
    ip->len=htons(payloadlen+20+40);
    ip->id=htons(0xABCD);
    ip->flag_offs=htons(0);
    ip->ttl=128;
    ip->proto=proto;
    ip->checksum=htons(0);
    ip->src= *(unsigned int*)myip;
    ip->dst= *(unsigned int*)dst;
    ip->checksum =htons(checksum((unsigned char *)ip,60));
};

struct icmp_packet 
{
    unsigned char type;
    unsigned char code;
    unsigned short checksum;
    unsigned short id;
    unsigned short seq;
    unsigned char payload[1400];
};

struct record_route
{
    unsigned char type;
    unsigned char length;
    unsigned char pointer;
    unsigned char route_data[37];
};

int forge_icmp(struct icmp_packet * icmp, int payloadsize)
{
    int i;
    icmp->type=8;
    icmp->code=0;
    icmp->checksum=htons(0);
    icmp->id = htons(1);
    icmp->seq = htons(1);
    for(i=0;i<payloadsize;i++)icmp->payload[i]=i&0xFF;
    icmp->checksum=htons(checksum((unsigned char*)icmp,8 + payloadsize));
} 

int arp_resolve(unsigned char* destip, unsigned char * destmac)
{
    int len,n,i;
    unsigned char pkt[1500];
    struct eth_frame *eth;
    struct arp_packet *arp;

    printf("\n%s\n%sARP REQUEST%s\n", LINE, BOLD_RED, DEFAULT);
    eth = (struct eth_frame *) pkt;
    arp = (struct arp_packet *) eth->payload; 
    
    for(i=0;i<6;i++) 
        eth->dst[i]=0xff;
    
    for(i=0;i<6;i++) 
        eth->src[i]=mymac[i];
    
    eth->type=htons(0x0806);
    
    arp->hw=htons(1);
    arp->proto=htons(0x0800);
    arp->hlen=6;
    arp->plen=4;
    arp->op=htons(1);
    
    for(i=0;i<6;i++) 
        arp->srcmac[i]=mymac[i];
    
    for(i=0;i<4;i++) 
        arp->srcip[i]=myip[i];
    
    for(i=0;i<6;i++) 
        arp->dstmac[i]=0;
    
    for(i=0;i<4;i++) 
        arp->dstip[i]=destip[i];
    
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = if_nametoindex("eth0");
    len = sizeof(sll);
    
    n=sendto(s,pkt,14+sizeof(struct arp_packet), 0,(struct sockaddr *)&sll,len);
    printpacket(pkt,14+sizeof(struct arp_packet));

    if (n == -1) 
    {
        perror("Recvfrom failed"); 
        return 0;
    }
    
    while( 1 )
    {
        n=recvfrom(s,pkt,1500, 0,(struct sockaddr *)&sll,&len);
        
        if (n == -1) 
        {
            perror("Recvfrom failed"); 
            return 0;
        }

        if (eth->type == htons (0x0806)) //ARP packet
        {
            if(arp->op == htons(2)) //ARP reply
            {
                if(!memcmp(destip,arp->srcip,4)) //From my target
                {
                    printf("%sARP REPLY%s\n", BOLD_RED, DEFAULT);
                    memcpy(destmac,arp->srcmac,6);
                    printpacket(pkt,14+sizeof(struct arp_packet));
                    return 0;
                }
            }
        }
    }
}

unsigned char packet[1500];

int main()
{
    int i,n,len, num_IPs, j;
    unsigned char dstmac[6];

    struct eth_frame* eth;
    struct ip_datagram* ip;
    struct icmp_packet* icmp;
    struct record_route* rr; 

    s = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)); 
    if(s==-1){perror("socket failed");return 1;}

    /**** HOST ROUTING ****/
    if( (*(unsigned int*)&myip) & (*(unsigned int*)&netmask) == 
        (*(unsigned int*)&targetip) & (*(unsigned int*)&netmask))
        arp_resolve(targetip,dstmac);
    else
        arp_resolve(gateway,dstmac);

    printf("%sdestmac: %s", BOLD_BLUE, DEFAULT);
    printpacket(dstmac,6);

    eth = (struct eth_frame *) packet;
    ip = (struct ip_datagram *) eth->payload; 
    rr = (struct record_route*) ip->option;
    icmp = (struct icmp_packet *) ip->payload;

    rr->type = 7;
    rr->length = 40;
    rr->pointer = 4;

    for(i=0;i<6;i++) eth->dst[i]=dstmac[i];
    for(i=0;i<6;i++) eth->src[i]=mymac[i];
    eth->type=htons(0x0800);
    forge_icmp(icmp, 20);
    forge_ip(ip,targetip, 20+8, 1); 
    
    printf("%sECHO REQUEST%s\n", BOLD_RED, DEFAULT);
    printpacket(packet,14+60+8+20);

    for(i=0;i<sizeof(sll);i++) ((char *)&sll)[i]=0;

    sll.sll_family=AF_PACKET;
    sll.sll_ifindex = if_nametoindex("eth0");
    len=sizeof(sll);
    n=sendto(s,packet,14+60+8+20, 0,(struct sockaddr *)&sll,len);
    
    if (n == -1) 
    {
        perror("Recvfrom failed"); 
        return 0;
    }

    while( 1 )
    {
        len=sizeof(sll);
        n=recvfrom(s,packet,1500, 0,(struct sockaddr *)&sll,&len);
        if (n == -1) {perror("Recvfrom failed"); return 0;}

        if (eth->type == htons (0x0800)) //IP datagram
        {
            if(ip->proto == 1) //ICMP packet 
            {
                if(icmp->type==0) //ECHO reply
                {
                    printf("%sECHO REPLY%s\n", BOLD_RED, DEFAULT);
                    if(rr->type==7)
                    {
                        printpacket(packet,14+60+8+20);
                        num_IPs=(rr->pointer-4)/4;
                        
                        printf("%sRoute%s\n", BOLD_BLUE, DEFAULT);
                        
                        for(j=0; j<num_IPs; j++)
                        {
                            printf("%s%d:%s", BOLD_YELLOW, j+1, DEFAULT);
                            for(i=0; i<3; i++)
                                printf("%u.",rr->route_data[j*4+i]);

                            printf("%u\n",rr->route_data[j*4+i]);
                        }

                        break;
                    }
                }
                else if(icmp->type==12) //Wrong IP option format
                {
                    printf("%sPROBLEM%s", BOLD_RED, DEFAULT);
                    printpacket(packet, n);
                    break;
                }
            }
        }
    }

    printf("%s\n\n", LINE);
    return 0;
}

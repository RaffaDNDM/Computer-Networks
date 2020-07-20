#include <stdio.h>
#include <arpa/inet.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h> /* the L2 protocols */
#include <net/if.h>
#include <string.h>
#include "utility.h"

int s;
struct sockaddr_ll sll;

struct eth_frame {
unsigned char dst[6];
unsigned char src[6];
unsigned short type;
unsigned char payload[1460];
};

struct ip_datagram {
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

unsigned char packet[1500];

int main(){
    int i,n,len, num_pkts=0;
    
    //Ethernet statistics
    int count_IP=0, count_ARP=0, count_3_level=0;
    //IP statistics
    int count_UDP=0, count_TCP=0, count_ICMP=0, count_other=0;

    unsigned char dstmac[6];

    struct eth_frame * eth;
    struct ip_datagram * ip;
    struct icmp_packet * icmp;

    s = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)); 
    if(s==-1){perror("socket failed");return 1;}

    eth = (struct eth_frame *) packet;
    ip = (struct ip_datagram *) eth->payload; 

    for(i=0;i<sizeof(sll);i++) ((char *)&sll)[i]=0;

    sll.sll_family=AF_PACKET;
    sll.sll_ifindex = if_nametoindex("wlp6s0");
    len=sizeof(sll);

    while(num_pkts<1000)
    {
        len=sizeof(sll);
        n=recvfrom(s,packet,1500, 0,(struct sockaddr *)&sll,&len);
        if (n == -1) {perror("Recvfrom failed"); return 0;}
        num_pkts++;

        if (eth->type == htons (0x0800)) //IP datagram
        {
            count_IP++;

            switch(ip->proto) // it is ICMP 
            {
                case 1: //ICMP packet
                {    
                    count_ICMP++;
                    break;
                }

                case 6:
                {
                    count_TCP++;
                    break;
                }

                case 17:
                {
                    count_UDP++;
                    break;
                }

                default:
                    count_other++;
            }
        }
        else if(eth->type == htons(0x0806)) //ARP packet
            count_ARP++;
        else //Neither IP nor ARP packet
            count_3_level++;
    }

    
    printf("%s___________________________________________________________________%s\n", 
            BOLD_GREEN, DEFAULT);
    
    printf("%s Ethernet statistics        IP statistics%s\n",
            BOLD_RED, DEFAULT);
    printf("%s   IP packets:%s %6.2lf%%       %sICMP packets:%s %6.2lf%%\n",
            BOLD_GREEN, DEFAULT, ((double) count_IP*100.0)/1000.0, BOLD_YELLOW, DEFAULT, ((double) count_ICMP*100.0)/count_IP);
    printf("%s  ARP packets:%s %6.2lf%%        %sTCP packets:%s %6.2lf%%\n",
            BOLD_GREEN, DEFAULT, ((double) count_ARP*100.0)/1000.0, BOLD_YELLOW, DEFAULT, ((double) count_TCP*100.0)/count_IP);
    printf("%sOther packets:%s %6.2lf%%        %sUDP packets:%s %6.2lf%%\n",
            BOLD_GREEN, DEFAULT, ((double) count_3_level*100.0)/1000.0, BOLD_YELLOW, DEFAULT, ((double) count_UDP*100.0)/count_IP);
    printf("                            %sOther packets:%s %6.2lf%%\n",
            BOLD_YELLOW, DEFAULT, ((double) count_other*100.0)/count_IP);
    
    printf("%s___________________________________________________________________%s\n\n",
            BOLD_GREEN, DEFAULT);
 
    return 0;
}

#include "utility.h"
#include "arp.h"
#include <sys/socket.h> //Socket for packet (see man packet)
#include <linux/if_packet.h> //..
#include <net/ethernet.h> //..
#include <string.h> //memcmp, memvpy and string elaboration
#include <stdio.h> //print and reading functions
#include <stdlib.h> //exit()
#include <net/if.h> //if_nametoindex
#include <arpa/inet.h> //htons()

void arp_resolution(int sd, host* src, host* dst, char* interface,
                    unsigned char* gateway, int verbose)
{
    unsigned char packet[PACKET_SIZE];
    struct sockaddr_ll sll;
    eth_frame *eth;
    arp_pkt *arp;
    int i;
    int found = 0;
    socklen_t len;
    int n;

    //Ethernet header
    eth = (eth_frame*) packet;

    for(i=0; i<6; i++)
        eth->dst[i]=0xff; //Broadcast request

    memcpy(eth->src, src->mac, 6);
    eth->type = htons(0x0806);

    //ARP packet
    arp = (arp_pkt *) (eth->payload);
    arp->hw = htons(0x0001);
    arp->protocol = htons(0x0800);
    arp->hw_len = 6;
    arp->prot_len = 4;
    arp->op = htons(0x0001);
    memcpy(arp->src_MAC, src->mac, 6);
    memcpy(arp->src_IP, src->ip, 4);

    for(i=0; i<6; i++)
        arp->dst_MAC[i] = 0;

    int local = ((*(unsigned int*) gateway)==0)? 1 : 0;

    if(local)
    {
        printf("        The remote host is in the same LAN\n");
        memcpy(arp->dst_IP, dst->ip, 4);
    }
    else
    {
        printf("      The remote host is outside the network\n");
        memcpy(arp->dst_IP, gateway, 4);
    }

    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = if_nametoindex(interface);

    len = sizeof(sll);

    if(verbose>50)
    {
        printf("\n%s                   ARP request\n%s", BOLD_BLUE, DEFAULT);
        print_packet(packet, ETH_HEADER_SIZE+sizeof(arp_pkt), BOLD_BLUE);
    }

    n = sendto(sd, packet, ETH_HEADER_SIZE+sizeof(arp_pkt), 0, (struct sockaddr*) &sll, sizeof(sll));
    control(n, "ARP sendto ERROR");

    while(!found)
    {
        int n = recvfrom(sd, packet, ETH_HEADER_SIZE+sizeof(arp_pkt), 0, (struct sockaddr*) &sll, &len);

        control(n, "ARP recvfrom ERROR");

        if(eth->type == htons(0x0806) && //it's ARP
           arp->op == htons(0x0002) && //it's ARP reply
           ((!memcmp(arp->src_IP, dst->ip, 4) && local) ||
           (!memcmp(arp->src_IP, gateway, 4) && !local))) //dst of ARP request = src of ARP reply
        {
            memcpy(dst->mac, arp->src_MAC, 6);

            if(verbose>50)
            {
                printf("\n%s                     ARP reply\n%s", BOLD_BLUE, DEFAULT);
                print_packet(packet, ETH_HEADER_SIZE+sizeof(arp_pkt), BOLD_BLUE);
            }

            found = 1;
        }
    }
}

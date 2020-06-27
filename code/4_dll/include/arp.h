#ifndef ARP
#define ARP

#include "utility.h"

typedef struct
{
    unsigned short hw; //code for HW protocol (e.g. Ethernet)
    unsigned short protocol; //code for upper layer protocol (e.g. IP)
    unsigned char hw_len; //length of HW address (6 for MAC)
    unsigned char prot_len; // length of protocol address (4 for IP)
    unsigned short op; //operation we want to do (e.g. ARP request or reply, rARP request or reply, ...)
    unsigned char src_MAC[6]; //src HW address
    unsigned char src_IP[4]; //src protocol address
    unsigned char dst_MAC[6]; //dst HW address
    unsigned char dst_IP[4]; //dst protocol address
}arp_pkt;

/**
 * @brief Send ARP request and analyze ARP reply, obtaining MAC address of remote machine.
 * @param sd socket of ethernet level
 * @param src IP address and MAC address of the source (mine)
 * @param dst IP address and MAC address of the destination
 * @param interface ethernet interface used by source (e.g. eth0)
 * @param gateway (default gateway if src and dst not in the same net or 0.0.0.0)
 * @param verbose level of verbose
 */
void arp_resolution(int sd, host* src, host* dst, char* interface, 
                    unsigned char* gateway, int verbose);

#endif

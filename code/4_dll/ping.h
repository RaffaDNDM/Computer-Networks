#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <netdb.h>

#define MAC_DEFAULT_FILE "/sys/class/net/%s/address"
#define LINE_SIZE 100
#define ROUTE_NETWORK_INDEX 0
#define ROUTE_GATEWAY_INDEX 1
#define ROUTE_MASK_INDEX 2
#define ROUTE_INTERFACE_INDEX 7
#define LINE_32_BITS "-----------------------------------------------------\n"
#define PACKET_SIZE 1500
#define ETH_HEADER_SIZE 14 //src_MAC + dst_MAC + type

typedef struct
{
    unsigned char mac[6];
    unsigned char ip[4];
}host;


typedef struct
{
    unsigned char dst[6];
    unsigned char src[6];
    unsigned short int type;
    unsigned char payload[1500];
}eth_frame;

typedef struct
{
    unsigned short hw;
    unsigned short protocol;
    unsigned char hw_len;
    unsigned char prot_len;
    unsigned short op;
    unsigned char src_MAC[6];
    unsigned char src_IP[4];
    unsigned char dst_MAC[6];
    unsigned char dst_IP[4];
}arp_pkt;

typedef struct
{
    unsigned char ver_IHL;
    unsigned char type_service;
    unsigned short length;
    unsigned short id;
    unsigned short flag_offs;
    unsigned char ttl;
    unsigned char proto;
    unsigned short checksum;
    unsigned char protocol;
    unsigned long src_IP;
    unsigned long dst_IP;
    unsigned char payload[1500];
}ip_datagram;

typedef struct
{
    unsigned char type;
    unsigned char code;
    unsigned short checksum;
    unsigned short id;
    unsigned short seq;
    unsigned char payload[1500];
}icmp_pkt;

/**
 *  @brief Print a packet on stdin.
 *  @param pkt packet to print
 *  @param size size of packet @pkt
 */
void print_packet(unsigned char* pkt, int size);

/**
 * @brief Send ARP request and analyze ARP reply, obtaining MAC address of remote machine.
 * @param sd socket of ethernet level
 * @param src IP address and MAC address of the source (mine)
 * @param dst IP address and MAC address of the destination
 * @param interface ethernet interface used by source (e.g. eth0)
 * @param gateway (default gateway if src and dst not in the same net or 0.0.0.0)
 */
void arp_resolution(int sd, host* src, host* dst, char* interface, unsigned char* gateway);

/**
 * @brief Create Ethernet packet.
 * @param eth pointer to Ethernet packet to fullfill
 * @param dst_MAC destination MAC address to which we send the message
 * @param src_MAC source MAC address (my MAC address)
 */
void create_eth(eth_frame* eth, unsigned char* dst_MAC, unsigned char* src_MAC);

/**
 * @brief Create IP packet.
 * @param ip pointer to IP packet to fullfill
 * @param dst_IP destination IP address to which we send the message
 * @param payload_size size of the IP payload
 * @param protocol protocol to be used in IP (ICMP)
 */
void create_IP(ip_datagram* ip, unsigned char* dst_IP, int payload_size, unsigned char protocol);


/**
 * @brief Creation of the ICMP packet for echo request (PING).
 * @param icmp pointer to ICMP packet to fullfill
 * @param payload_size size of the ICMP payload
 */
void echo_request(icmp_pkt* icmp, int payload_size);

/**
 * @brief Ping routine.
 * @param src_IP my IP address
 * @param src_MAC my MAC address
 * @param dst_IP IP address of remote host
 * @param dst_MAC IP address of remote host
 */
void ping_application(unsigned char* src_IP, unsigned char* src_MAC, 
                      unsigned char* dst_IP, unsigned char* dst_MAC);

/**
 * @brief Computation of checksum of a set of bytes.
 * @param buf set of bytes of which we want to compute the checksum
 * @param size number of bytes of @buf
 */
void checksum(char* buf, int size);

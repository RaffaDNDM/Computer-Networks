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
#include <time.h>
#include <float.h>

//Colors
#define DEFAULT "\033[0m" 

#define BLUE "\033[0;34m"
#define CYAN "\033[0;36m"
#define GREEN "\033[0;32m"
#define MAGENTA "\033[0;35m" 
#define RED "\033[0;31m"
#define YELLOW "\033[0;33m"

#define BOLD_BLUE "\033[1;34m"
#define BOLD_CYAN "\033[1;36m"
#define BOLD_GREEN "\033[1;32m"
#define BOLD_MAGENTA "\033[1;35m" 
#define BOLD_RED "\033[1;31m"
#define BOLD_YELLOW "\033[1;33m"


//Verbose variable to decide what program prints
#define MIN_VERBOSE 0
#define MAX_VERBOSE 100

//Utility macros
#define MAC_DEFAULT_FILE "/sys/class/net/%s/address"
#define LINE_SIZE 100
#define ROUTE_NETWORK_INDEX 0
#define ROUTE_GATEWAY_INDEX 1
#define ROUTE_MASK_INDEX 2
#define ROUTE_INTERFACE_INDEX 7

//Format of packet to print
#define LINE_32_BITS "-----------------------------------------------------\n"

//Packet characteristics
#define PACKET_SIZE 1500
#define IP_HEADER_SIZE 20
#define ECHO_HEADER_SIZE 8
#define ETH_HEADER_SIZE 14 //src_MAC + dst_MAC + type

//Default parameters of ping
#define DEFAULT_NUM 20 //Number of packets to be sent
#define DEFAULT_SIZE 10 //Number of bytes in ICMP packets
#define DEFAULT_TIMEOUT 1000.0 //Default timeout of a packet

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
    unsigned char protocol;
    unsigned short checksum;
    unsigned int src_IP;
    unsigned int dst_IP;
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
 *  @param color color of the table, representing the packet 
 */
void print_packet(unsigned char* pkt, int size, char* color);

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
 * @brief Ping single iteration.
 * @param src_IP my IP address
 * @param src_MAC my MAC address
 * @param dst_IP IP address of remote host
 * @param dst_MAC IP address of remote host
 */
int ping_iteration(int sd, int id_pkt, int size_pkts,
                    double timeout, char* interface, host src, host dst);

/**
 * @brief Computation of checksum of a set of bytes.
 * @param buf set of bytes of which we want to compute the checksum
 * @param size number of bytes of @buf
 */
unsigned short int checksum(unsigned char* buf, int size);


void ping(int sd, int num_pkts, int size_pkt, double timeout, char* interface, host src, host dst);


void print_ping(int id, int ttl, int size, double elapsed_time);

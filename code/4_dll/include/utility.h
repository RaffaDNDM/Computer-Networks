#ifndef UTILITY
#define UTILITY

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

//Unit of measurement of time
#define TIME_s "s"
#define TIME_ms "ms"
#define TIME_ns "ns"

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
#define PACKET_SIZE 1500 //Max size of ethernet packet
#define IP_HEADER_SIZE 20 //Size of IP header used for ping
#define ECHO_HEADER_SIZE 8 //Size of ICMP header used for ping
#define ETH_HEADER_SIZE 14 //src_MAC + dst_MAC + type

//Default parameters of ping
#define DEFAULT_NUM 20 //Number of packets to be sent
#define DEFAULT_SIZE 10 //Number of bytes in ICMP packets
#define DEFAULT_TIMEOUT 1000.0 //Default timeout of a packet

typedef struct
{
    unsigned char mac[6]; //MAC address of the host
    unsigned char ip[4]; //IP address of the host
}host;


typedef struct
{
    unsigned char dst[6]; //dst MAC address
    unsigned char src[6]; //src MAC address
    unsigned short int type; //type of upper layer protocol (e.g. IP, ARP,...)
    unsigned char payload[1500]; //payload
}eth_frame;

typedef struct
{
    unsigned char ver_IHL; //version (8 Bytes) = 4  +  IHL (8 Bytes) = number of 32 words used in header = 5
    unsigned char type_service; //type of service
    unsigned short length; // length of the entire IP datagram
    unsigned short id; //identifier of the packet
    unsigned short flag_offs; // flags (Don't fragment,...)
    unsigned char ttl; //Time to live
    unsigned char protocol; //upper layer protocol (e.g. ICMP)
    unsigned short checksum; //checksum of IP header
    unsigned int src_IP; //src IP address
    unsigned int dst_IP; //dst IP address
    unsigned char payload[1500];
}ip_datagram;

typedef struct
{
    unsigned char type; //type of ICMP packet (8=ECHO request,  0=ECHO reply)
    unsigned char code; //additional specifier of type
    unsigned short checksum; //checksum of entire ICMP packet (Header+Payload)
    unsigned short id; //identifier of the packet
    unsigned short seq; //usefull to identify packet together with id
    unsigned char payload[1500];
}icmp_pkt;

/**
 * @brief Computation of checksum of a set of bytes.
 * @param buf set of bytes of which we want to compute the checksum
 * @param size number of bytes of @buf
 */
unsigned short int checksum(unsigned char* buf, int size);

/**
 *  @brief Print a packet on stdin.
 *  @param pkt packet to print
 *  @param size size of packet @pkt
 *  @param color color of the table, representing the packet
 */
void print_packet(unsigned char* pkt, int size, char* color);

/**
 *  @brief Check if code is an error(-1)
 *  @param code code to check
 *  @param message message printed if error
*/
void control(int code, char* message);
#endif

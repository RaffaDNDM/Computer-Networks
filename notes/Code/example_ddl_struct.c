/*Host (IP address+port)*/
typedef struct
{
    unsigned char mac[6]; //MAC address of the host
    unsigned char ip[4]; //IP address of the host
}host;

/*Ethernet frame format*/
typedef struct
{
    unsigned char dst[6]; //dst MAC address
    unsigned char src[6]; //src MAC address
    unsigned short int type; //type of upper layer protocol (e.g. IP, ARP,...)
    unsigned char payload[1500]; //payload
}eth_frame;

/*ARP packet format*/
typedef struct
{
    unsigned short hw; //code for HW protocol (e.g. Ethernet)
    unsigned short protocol; //code for upper layer protocol (e.g. IP)
    unsigned char hw_len; //length of HW address (6 for MAC)
    unsigned char prot_len; // length of protocol address (4 for IP)
    unsigned short op; //operation to do (e.g. ARP request/reply, rARP request/reply, ...)
    unsigned char src_MAC[6]; //src HW address
    unsigned char src_IP[4]; //src protocol address
    unsigned char dst_MAC[6]; //dst HW address
    unsigned char dst_IP[4]; //dst protocol address
}arp_pkt;

/*IP datagram format*/
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

/*ICMP packet format*/
typedef struct
{
    unsigned char type; //type of ICMP packet (8=ECHO request,  0=ECHO reply)
    unsigned char code; //additional specifier of type
    unsigned short checksum; //checksum of entire ICMP packet (Header+Payload)
    unsigned short id; //identifier of the packet
    unsigned short seq; //usefull to identify packet together with id
    unsigned char payload[1500];
}icmp_pkt;

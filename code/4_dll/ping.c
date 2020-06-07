#include "ping.h"

int main(int argc, char** argv)
{
    int sd;
    int ret;
    int i;
    int j;
    unsigned int x;
    FILE* fd;
    char command[50];
    char* interface;
    char line[LINE_SIZE];
    unsigned char network[4];
    unsigned char gateway[4];
    unsigned char mask[4];
    char mac_file[30];
    char c;
    struct hostent* he;
    char* word;
    struct in_addr addr;
    char packet[PACKET_SIZE];

    host src; //me
    host dst; //remote host

    if(argc==1)
    {
        perror("You need to specify at least destination address, type --help for info");
        exit(1);
    }
    if(argc==2)
    {
        if(inet_aton(argv[1], &addr)==0) //input argument is not a valid IP address
        {
            he = gethostbyname(argv[1]);

            if(he == NULL)
            {
                perror("Error getting IP from hostname..");
                exit(1);
            }
            else
            {
                for(i=0; i<4; i++)
                    dst.ip[i] = (unsigned char) (he->h_addr[i]);
            }
            
        }
        else
        {
            unsigned char *p = (unsigned char*) &(addr.s_addr);
            
            for(i=0; i<4; i++)
                dst.ip[i] = p[i];
        }
    }
    
    
    printf("\n---------------Remote  analysis----------------------\n");
    printf("Destination address = ");
    for(i=0; i<3; i++)
    {
        printf("%u.", dst.ip[i]);
    }
    printf("%u\n", dst.ip[i]);
    
    
    //Evaluation of Ethernet interface name
    sprintf(command, "route -n | tac | head --lines=-2 ");
    fd = popen(command, "r");

    if(fd == NULL)
    {
        perror("error opening pipe..");
        return 1;
    }

    char* route_fields[8];
    
    while(fgets(line, LINE_SIZE, fd)!=NULL)
    {
        char* s = strtok(line, " ");
        i=0;
        
        if(s!=NULL)
        {
            if (inet_aton(s, &addr)!=0)
            { 
                unsigned char *p = (unsigned char*) &(addr.s_addr);
            
                for(j=0; j<4; j++)
                    network[j] = p[j];
            } 
            i++;
        }

        while((s=strtok(NULL," "))!=NULL && i<8)
        {
            switch(i)
            {
                case ROUTE_GATEWAY_INDEX:
                {
                    if (inet_aton(s, &addr)!=0)
                    { 
                        unsigned char *p = (unsigned char*) &(addr.s_addr);
                    
                        for(j=0; j<4; j++)
                            gateway[j] = p[j]; 
                    }
                    break;
                }

                case ROUTE_MASK_INDEX:
                {
                    if (inet_aton(s, &addr)!=0)
                    { 
                        unsigned char *p = (unsigned char*) &(addr.s_addr);
                    
                        for(j=0; j<4; j++)
                            mask[j] = p[j];
                    }
                    break;
                }

                case ROUTE_INTERFACE_INDEX:
                {
                    s[strlen(s)-1]=0;
                    interface = s;
                }
            }
        
            i++;
            /*
            printf("Gateway: "); 
            for(j=0; j<3; j++)
                printf("%u.", gateway[j]);
            printf("%u\n", gateway[j]);

            printf("Network: ");
            for(j=0; j<3; j++)
                printf("%u.", network[j]);
            printf("%u\n", network[j]);

            printf("Mask: ");
            for(j=0; j<3; j++)
                printf("%u.", mask[j]);
            printf("%u\n", mask[j]);

            printf("Interface: %s\n", interface);
            */
        }

        
        if((*(unsigned int*) &network)==((*((unsigned int*) &(dst.ip))) & (*((unsigned int*) &mask))))
        {        
            break;
        }
    }

    printf("\n");
    printf("Gateway: "); 
    for(i=0; i<3; i++)
        printf("%u.", gateway[i]);
    printf("%u\n", gateway[i]);

    printf("Network: ");
    for(i=0; i<3; i++)
        printf("%u.", network[i]);
    printf("%u\n", network[i]);

    printf("Mask: ");
    for(i=0; i<3; i++)
        printf("%u.", mask[i]);
    printf("%u\n", mask[i]);

    eth_frame* eth;
    ip_datagram* ip;
    icmp_pkt* icmp;
    
    //See the MAC address of eth0 looking to e.g. "/sys/class/net/eth0/address" content
    sprintf(mac_file, MAC_DEFAULT_FILE, interface);
   
    fd = fopen(mac_file, "r");

    for(i=0; i<5; i++)
    {
        fscanf(fd, "%x:", &x);
        src.mac[i]=(unsigned char) x;
    }
    
    fscanf(fd, "%x\n", &x);
    src.mac[i]=(unsigned char) x;

    fclose(fd);
    
    printf("\n");

    printf("Ethernet Interface: %s\n", interface);

    printf("Source MAC address: ");
    for(i=0; i<5; i++)
        printf("%x:", src.mac[i]);
    printf("%x\n", src.mac[i]);
    
    
    //Evaluation of IPv4 address of ethernet interface in input 
    sprintf(command, "ip -4 addr show %s | grep -oP '(?<=inet\\s)\\d+(\\.\\d+){3}'", interface);
    fd = popen(command, "r");

    for(i=0; i<3; i++)
    {
        fscanf(fd, "%u%c", &x, &c);
        src.ip[i]=x;
    }
    
    fscanf(fd, "%u", &x);
    src.ip[i]=x;

    pclose(fd);
    
    printf("Source IP address: ");
    for(i=0; i<3; i++)
        printf("%d.", src.ip[i]);
    printf("%d\n",src.ip[i]);


    //Creation of the socket
    sd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

    
    if(sd == -1)
    {
        printf("Socket failed\n");
        perror("ERROR");
        return 1;
    }

    
    //ARP resolution
    /*
        if(myip & mask == dstip & mask)
            arp_resolution(sd, &dst, 0.0.0.0);
        else
            arp_resolution(sd, &dst, gateway);
    */

    //arp_resolution(sd, &src, &dst, interface, gateway);
    

    //printf("\n\n---------------Packets  analysis---------------------\n");
    //unsigned char buff[]={1,2,3,4,5,6,7};
    //print_packet(buff, 7);



    //Ping application
    

    return 0;
}

void print_packet(unsigned char* pkt, int size)
{
    int i=0;
    int count = ((size%4)==0)? 4: (size%4);


    printf(LINE_32_BITS); 
    for(; i<size; i++)
    {
        printf("| 0x%02x (%03u) ", pkt[i], pkt[i]);
        
        if((i%4)==3 || i==(size-1))
        {
            printf("|\n");
       
            if(i!=(size-1))
                printf(LINE_32_BITS);
        }
    }
    
    for(i=0; i<count; i++)
    {
        printf("-------------");
    }

    printf("-\n\n");

}

void arp_resolution(int sd, host* src, host* dst, char* interface, unsigned char* gateway)
{
    unsigned char packet[PACKET_SIZE];
    struct sockaddr_ll sll;
    eth_frame *eth;
    arp_pkt *arp;
    int i=0;

    //Ethernet header
    eth = (eth_frame*) &packet;

    for(; i<6; i++)
        eth->src[i]=src->mac[i];

    for(; i<6; i++)
        eth->src[i]=0xff; //Broadcast request

    eth->type = htons(0x0800);

    //ARP packet
    arp = (arp_pkt*) &(eth->payload);

    arp->hw = htons(0x0001);
    arp->protocol = htons(0x0800);
    arp->hw_len = 6;
    arp->prot_len = 4;
    arp->op = htons(0x0001);
    
    for(i=0; i<6; i++)
        arp->src_MAC[i] = src->mac[i];

    for(i=0; i<4; i++)
        arp->src_IP[i] = src->ip[i];

    for(i=0; i<6; i++)
        arp->dst_MAC[i] = 0;

    
    int local = ((*(unsigned int*) gateway)==0)? 1 : 0;

    for(i=0; i<4; i++)
        arp->dst_IP[i] = (local)? dst->ip[i] : gateway[i];

    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = if_nametoindex(interface);

    //sendto(sd, packet, ETH_HEADER_SIZE+sizeof(arp_pkt), 0, (struct sockaddr*) &sll, sizeof(sll));


}

void create_eth(eth_frame* eth, unsigned char* dst_MAC, unsigned char* src_MAC)
{

}

void create_IP(ip_datagram* ip, unsigned char* dst_IP, int payload_size, unsigned char protocol)
{

}

void echo_request(icmp_pkt* icmp, int payload_size)
{

}

void ping_application(unsigned char* src_IP, unsigned char* src_MAC, 
                      unsigned char* dst_IP, unsigned char* dst_MAC);

void checksum(char* buf, int size);

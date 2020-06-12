#include "ping.h"

int verbose = MIN_VERBOSE;

int main(int argc, char** argv)
{
    int sd;
    int i;
    unsigned int x;
    FILE* fd;
    char command[60];
    char* interface;
    char line[LINE_SIZE];
    unsigned char network[4];
    unsigned char gateway[4];
    unsigned char mask[4];
    char mac_file[30];
    char c;
    struct hostent* he;
    struct in_addr addr;

    host src; //me
    host dst; //remote host
    int num_pkts = DEFAULT_NUM;
    int size_pkt = DEFAULT_SIZE;
    double timeout = DEFAULT_TIMEOUT; 

    if(argc==1)
    {
        printf("You need to specify at least destination address, type --help for info");
        exit(1);
    }
    else if(argc>=2)
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

        if(argc>2)
        {
            int i=2;
            for(; i<argc; i++)
            {
                if(!strncmp(argv[i], "-n", 2))
                    num_pkts = atoi(argv[++i]);
                else if(!strncmp(argv[i], "-s", 2))
                    size_pkt = atoi(argv[++i]);
                else if(!strncmp(argv[i], "-t", 2))
                    timeout  = atof(argv[++i]);
                else if(!strncmp(argv[i], "-v", 2))
                    verbose  = MAX_VERBOSE;
            }
        }
    }
    
    printf("\n%s---------------Remote  analysis----------------------\n%s", BOLD_RED, DEFAULT);
    printf("%sDestination address = %s",BOLD_GREEN, DEFAULT);
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

    while(fgets(line, LINE_SIZE, fd)!=NULL)
    {
        char* s = strtok(line, " ");
        i=0;
        
        if(s!=NULL)
        {
            if (inet_aton(s, &addr)!=0)
            { 
                unsigned char *p = (unsigned char*) &(addr.s_addr);
            
                memcpy(network, p, 4);
                //for(j=0; j<4; j++)
                //    network[j] = p[j];
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
                    
                        memcpy(gateway, p, 4);
                        //for(j=0; j<4; j++)
                        //    gateway[j] = p[j]; 
                    }
                    break;
                }

                case ROUTE_MASK_INDEX:
                {
                    if (inet_aton(s, &addr)!=0)
                    { 
                        unsigned char *p = (unsigned char*) &(addr.s_addr);
                    
                        memcpy(mask,p, 4);
                        //for(j=0; j<4; j++)
                        //  mask[j] = p[j];
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
        }

        
        if((*(unsigned int*) &network)==((*((unsigned int*) &(dst.ip))) & (*((unsigned int*) &mask))))
        {        
            break;
        }
    }

    printf("\n");
    printf("%sGateway: %s", BOLD_MAGENTA, DEFAULT); 
    for(i=0; i<3; i++)
        printf("%u.", gateway[i]);
    printf("%u\n", gateway[i]);

    printf("%sNetwork: %s", BOLD_MAGENTA, DEFAULT);
    for(i=0; i<3; i++)
        printf("%u.", network[i]);
    printf("%u\n", network[i]);

    printf("%s   Mask: %s", BOLD_MAGENTA, DEFAULT);
    for(i=0; i<3; i++)
        printf("%u.", mask[i]);
    printf("%u\n", mask[i]);

    
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

    printf("%sEthernet Interface:%s %s\n", BOLD_CYAN, DEFAULT, interface);

    printf("%sSource MAC address: %s", BOLD_CYAN, DEFAULT);
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
    
    printf("%sSource IP address: %s", BOLD_CYAN, DEFAULT);
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

    printf("\n%s-------------------ARP packets-----------------------\n%s", BOLD_RED, DEFAULT);
    arp_resolution(sd, &src, &dst, interface, gateway);
    
    printf("%sDestination MAC address: %s", BOLD_YELLOW, DEFAULT);
    for(i=0; i<5; i++)
        printf("%x:", dst.mac[i]);
    printf("%x\n", dst.mac[i]);
 
    printf("\n%s-----------------------------------Ping--------------------------------------\n%s", BOLD_RED, DEFAULT);


    //Ping application
    ping(sd, num_pkts, size_pkt, timeout, interface, src, dst); 

    printf("%s%s%s\n", BOLD_RED, LINE_32_BITS, DEFAULT);
    return 0;
}

void print_packet(unsigned char* pkt, int size, char* color)
{
    int i=0;
    int count = ((size%4)==0)? 4: (size%4);


    printf("%s%s%s", color, LINE_32_BITS, DEFAULT); 
    for(; i<size; i++)
    {
        printf("%s|%s 0x%02x (%s%03u%s)%s ", color, YELLOW, pkt[i], DEFAULT, pkt[i], YELLOW, DEFAULT);
        
        if((i%4)==3 || i==(size-1))
        {
            printf("%s|%s\n", color, DEFAULT);
       
            if(i!=(size-1))
                printf("%s%s%s", color, LINE_32_BITS, DEFAULT); 
        }
    }
    
    for(i=0; i<count; i++)
    {
        printf("%s-------------", color);
    }

    printf("-%s\n\n", DEFAULT);

}

void arp_resolution(int sd, host* src, host* dst, char* interface, unsigned char* gateway)
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
    eth = (eth_frame*) &packet;

    for(i=0; i<6; i++)
        eth->dst[i]=0xff; //Broadcast request

    memcpy(eth->src, src->mac, 6);
    //for(i=0; i<6; i++)
    //    eth->src[i]=src->mac[i];

    eth->type = htons(0x0806);

    //ARP packet
    arp = (arp_pkt*) &(eth->payload);

    arp->hw = htons(0x0001);
    arp->protocol = htons(0x0800);
    arp->hw_len = 6;
    arp->prot_len = 4;
    arp->op = htons(0x0001);
    
    memcpy(arp->src_MAC, src->mac, 6);
    //for(i=0; i<6; i++)
    //    arp->src_MAC[i] = src->mac[i];

    memcpy(arp->src_IP, src->ip, 4);
    //for(i=0; i<4; i++)
    //    arp->src_IP[i] = src->ip[i];

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
    //for(i=0; i<4; i++)
    //  arp->dst_IP[i] = (local)? dst->ip[i] : gateway[i];
        

    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = if_nametoindex(interface);

    len = sizeof(sll);

    if(verbose>50)
    {
        printf("\n%s                   ARP request\n%s", BOLD_BLUE, DEFAULT);
        print_packet(packet, ETH_HEADER_SIZE+sizeof(arp_pkt), BOLD_BLUE);
    }

    n = sendto(sd, packet, ETH_HEADER_SIZE+sizeof(arp_pkt), 0, (struct sockaddr*) &sll, sizeof(sll));

    if(n==-1)
    {
        perror("ARP sendto ERROR");
        exit(1);
    }

    while(!found)
    {
        int n = recvfrom(sd, packet, ETH_HEADER_SIZE+sizeof(arp_pkt), 0, (struct sockaddr*) &sll, &len);

        if(n==-1)
        {
            perror("ARP recvfrom ERROR");
            exit(1);
        }

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

int ping_iteration(int sd, int id_pkt, int size_pkt, 
                   double timeout, char* interface, host src, host dst)
{
    unsigned char packet[PACKET_SIZE];
    struct sockaddr_ll sll;
    eth_frame *eth;
    ip_datagram *ip;
    icmp_pkt *icmp;
    int i;
    int found = 0;
    socklen_t len;
    int n;

    eth = (eth_frame*) &packet;
    ip = (ip_datagram*) &(eth->payload);
    icmp = (icmp_pkt*) &(ip->payload);

    //Ethernet header
    memcpy(eth->src, src.mac, 6);
    memcpy(eth->dst, dst.mac, 6);
    eth->type = htons(0x0800);

    //IP packet
    ip->ver_IHL = 0x45;
    ip->type_service = 0;
    ip->length = htons(ECHO_HEADER_SIZE+size_pkt+IP_HEADER_SIZE);
    ip->id = htons(id_pkt);
    ip->flag_offs = htons(0);
    ip->ttl = 128;
    ip->protocol = 1; //ICMP
    ip->checksum = 0;
    memcpy((unsigned char*) &(ip->src_IP), src.ip, 6);
    memcpy((unsigned char*) &(ip->dst_IP), dst.ip, 6);
    ip->checksum = htons(checksum((unsigned char*) ip, IP_HEADER_SIZE)); //Checksum of ip header


    //Echo request (ICMP)
    icmp->type = 8;
    icmp->code = 0;
    icmp->checksum = htons(0);
    icmp->id = htons(id_pkt);
    icmp->seq = htons(1);
    
    for(i=0; i<size_pkt; i++)
        icmp->payload[i] = i&0xff;

    icmp->checksum = htons(checksum((unsigned char*) icmp, ECHO_HEADER_SIZE+size_pkt));
    //Checksum of the entire packet


    for(i=0; i<sizeof(sll); i++)
        ((char*) &sll)[i]=0;

    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = if_nametoindex(interface);
    len = sizeof(sll);

    if(verbose>50)
    {
        printf("\n%s                 ECHO request\n%s", BOLD_BLUE, DEFAULT);
        print_packet(packet, ETH_HEADER_SIZE+IP_HEADER_SIZE+ECHO_HEADER_SIZE+size_pkt, BOLD_BLUE);
    }

    n = sendto(sd, packet, ETH_HEADER_SIZE+IP_HEADER_SIZE+ECHO_HEADER_SIZE+size_pkt, 0, (struct sockaddr*) &sll, len); 

    if(n==-1)
    {
        perror("ECHO sendto ERROR");
        exit(1);
    }

    time_t start = clock();
    double remaining_time = timeout;
    
    while(!found && remaining_time > 0.0)
    {
        len = sizeof(sll);
        n = recvfrom(sd, packet, PACKET_SIZE, 0, (struct sockaddr*) &sll, &len);
        
        if(n==-1)
        {
            perror("ECHO recvfrom ERROR");
            exit(1);
        }
        
        time_t end = clock();
        remaining_time -= ((double) (end-start)/(double) CLOCKS_PER_SEC)*1000.0;

        
        if(remaining_time < 0)
        {
            printf("TIME EXCEEDED\n");
            return 0;
        }

        if(eth->type == htons(0x0800) && //IP datagram
           ip->protocol == 1 && //ICMP packet
           icmp->type == 0 &&
           icmp->id == htons(id_pkt)) //ECHO reply
        {
            if(verbose>50)
            {
                printf("\n%s                   ECHO reply\n%s", BOLD_BLUE, DEFAULT);
                print_packet(packet, ETH_HEADER_SIZE+IP_HEADER_SIZE+ECHO_HEADER_SIZE+size_pkt, BOLD_BLUE);
            }

            found = 1;
            print_ping(id_pkt, ip->ttl, size_pkt, timeout-remaining_time);
        }
    }

    return 1;
}

unsigned short checksum(unsigned char* buf, int size)
{
    int i;
    unsigned int sum=0;
    unsigned short* p = (unsigned short*) buf;

    for(i=0; i<size/2; i++)
    {
        sum += htons(p[i]);
        
        if(sum&0x10000) 
            sum = (sum&0xffff)+1;
    }

    return (unsigned short) ~sum;
}

void print_ping(int id, int ttl, int size, double elapsed_time)
{
    printf("%s[Packet %3d]%s ttl:%s %3d hops left     %ssize:%s %3d bytes    %selapsed_time:%s %.3lf ms\n", BOLD_CYAN, id, MAGENTA, DEFAULT, ttl, GREEN, DEFAULT, size, YELLOW, DEFAULT, elapsed_time);
}

void ping(int sd, int num_pkts, int size_pkt, double timeout,
          char* interface, host src, host dst)
{
    int i=0;
    int count_done = 0; 

    while(i<num_pkts)
    {
        count_done += ping_iteration(sd, i+1, size_pkt, timeout, interface, src, dst);
        i++;
    }
    
    printf("\n %sCOMPLETED:%s %d/%d\n", BOLD_YELLOW, DEFAULT, count_done, num_pkts);
}

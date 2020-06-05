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
    const char device[5] = "dev ";
    char mac_file[30];
    char c;
    struct hostent* he;
    char* word;
    struct in_addr addr;

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
            
            //Print the server address
            printf("[1]Destination address = ");
            for(i=0; i<3; i++)
            {
                printf("%u.", dst.ip[i]);
            }
            printf("%u\n", dst.ip[i]);
        }
        else
        {
            unsigned char *p = (unsigned char*) &(addr.s_addr);
            
            for(i=0; i<4; i++)
                dst.ip[i] = p[i];
        
            printf("[2]Destination address = ");
            for(i=0; i<3; i++)
            {
                printf("%u.", dst.ip[i]);
            }
            printf("%u\n", dst.ip[i]);
        }
    }
    
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
                    interface = s;
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
            printf("--------------------\nChecked\n------------------------\n");
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

            printf("Interface: %s\n", interface);

            break;
        }
    }

    //Find where is "dev interface" (e.g. dev eth0)
    eth_frame* eth;
    ip_datagram* ip;
    icmp_pkt* icmp;
    
    
    /*    
    //Evaluation of default Ethernet interface name
    sprintf(command, "ip route show to 0.0.0.0/0");
    fd = popen(command, "r");

    if(fd == NULL)
    {
        perror("error opening pipe..");
        return 1;
    }

    fgets(line, LINE_SIZE, fd);

    //Find where is "dev interface" (e.g. dev eth0)
    j=0;
    for(i=0; i<strlen(line) && j!=strlen(device); i++)
    {
        if(line[i]==device[j])
            j++;
        else if(j!=0)
            j=0;
    }

    pclose(fd);
    

    j=0;
    for(; line[i]!=' '; i++)
        interface[j++]=line[i];
    
    interface[j]=0;

    printf("%s size: %d\n", interface, (int) strlen(interface));
    
    //See the MAC address of eth0 looking to "/sys/class/net/eth0/address" content
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
    
    for(i=0; i<3; i++)
        printf("%d.", src.ip[i]);
    printf("%d\n",src.ip[i]);



    //Creation of the socket
    //sd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

    
    if(sd == -1)
    {
        printf("errno: %d", errno);
        perror("Socket failed");
        return 1;
    }
    */

    //ARP resolution

    //Ping application
    

    //return 0;
}

void print_packet(unsigned char* pkt, int size)
{

}

void arp_resolution(unsigned char* dest_IP, unsigned char* dest_MAC)
{

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

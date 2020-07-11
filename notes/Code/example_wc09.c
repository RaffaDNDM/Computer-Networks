#include "net_utility.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

struct sockaddr_in server;

int main(int argc, char ** argv)
{
    int sd;
    int t;
    int size;
    char request[100];
    char response[1000000];

    unsigned char ipaddr[4] = {216,58,211,163};

    if(argc>3)
        control(-1, "Too many arguments");

    //Initialization of TCP socket for IPv4 protocol
    sd = socket(AF_INET, SOCK_STREAM, 0);
    control(sd, "Socket failed \n");

    //Definition of IP address + Port of the server
    server.sin_family=AF_INET;
    server.sin_port = htons(80);

    if(argc>1)
    {
        server.sin_addr.s_addr=inet_addr(argv[1]);
        //or inet_aton(argv[1], &server.sin_addr);

        if(argc==3)
            server.sin_port = htons(atoi(argv[2]));
    }
    else
    {
        server.sin_addr.s_addr = *(uint32_t *) ipaddr;
        server.sin_port = htons(80);
    }

    //Connect to remote server
    t = connect(sd, (struct sockaddr *)&server, sizeof(server));
    control(t, "Connection failed \n");

    //Writing on socket (Sending request to server)
    sprintf(request, "GET /\r\n");
    size = my_strlen(request);
    t = write(sd, request, size);
    control(t, "Write failed\n");

    //Reading the response
    for(size=0; (t=read(sd, response+size, 1000000-size))>0; size=size+t);
    control(t, "Read failed \n");
    print_body(response, size, 0);

    return 0;
}

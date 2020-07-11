#include "wc10.h"
#include "net_utility.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

struct sockaddr_in server;
header h[30];

int main(int argc, char ** argv)
{
    int sd;
    int t;
    int i;
    int j;
    int k;
    int status_length;
    int size;
    int code;
    int body_length;
    char request[100];
    char response[1000000];
    char *website;
    char *status_tokens[3];
    unsigned char ipaddr[4] = {216, 58, 208, 131};

    if(argc>3)
    {
        control(-1,"Too many arguments");
    }

    //Initialization of TCP socket for IPv4 protocol
    sd = socket(AF_INET, SOCK_STREAM, 0);
    control(sd, "Socket failed \n");

    //Definition of IP address + Port of the server
    server.sin_family=AF_INET;
    server.sin_port = htons(80);

    if(argc>1)
    {
        server.sin_addr.s_addr=inet_addr(argv[1]);

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
    sprintf(request, "GET / HTTP/1.0\r\nConnection: keep-alive\r\n\r\n");
    size = my_strlen(request);
    t = write(sd, request, size);
    control(t, "Write failed\n");

    j = 0;
    k = 0;
    h[k].name= response;

    //Parser of response (HEADER*STATUS LINE)
    while(read(sd, response+j, 1))
    {
        if((response[j]=='\n') && (response[j-1]=='\r'))
        {
            response[j-1]=0;

            if(h[k].name[0]==0)
                break;

            h[++k].name = response+j+1;
        }

        if(response[j]==':' && h[k].value==0)
        {
            response[j]=0;
            h[k].value=response+j+1;
        }
        j++;
    }

    //Status line parser
    status_length = my_strlen(h[0].name);

    status_tokens[0]=h[0].name;
    i=1;
    k=1;
    for(i=0; i<status_length && k<3; i++)
    {
        if(h[0].name[i]==' ')
        {
            h[0].name[i]=0;
            status_tokens[k]=h[0].name+i+1;
            k++;
        }
    }


    printf(LINE);
    printf("Status line:\n");
    printf(LINE);

    printf("HTTP version: %30s\n", status_tokens[0]);
    code = atoi(status_tokens[1]);
    printf("HTTP code:    %30d\n", code);
    printf("HTTP version: %30s\n", status_tokens[2]);
    printf(LINE);

    //Analysis of header values
    website=NULL;
    for(i=1; h[i].name[0]; i++)
    {
        if(!strcmp(h[i].name, "Content-Length"))
            body_length = atoi(h[i].value);

        if(!strcmp(h[i].name, "Location") && code>300 && code<303)
            website=h[i].value;

        printf("Name=%s -----> Value=%s\n",h[i].name, h[i].value);
    }

    //Reading the response
    if(body_length)
        for(size=0; (t=read(sd, response+j+size, body_length-size))>0; size+=t);
    else
        for(size=0; (t=read(sd, response+j+size, 1000000-size))>0; size+=t);

    control(t, "Read failed");

    //Print the redirection
    if(website!=NULL)
    {
        printf(LINE);
        printf("\nRedirection:       %s \n\n", website);
    }

    //Print the response
    print_body(response, size, j);

    return 0;
}

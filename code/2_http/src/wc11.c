#include "net_utility.h"
#include "wc11.h"

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

struct sockaddr_in server;
header h[30];

int main(int argc, char ** argv)
{
    int sd;
    int t;
    int i;
    int k;
    int size;
    int header_size;
    int body_length=0;
    char request[100];
    char response[1000000];
    char entity[1000000];
    char *website=NULL;
    char *status_tokens[3];
    unsigned char ipaddr[4] = {192,168,1,81};

    if(argc>3)
    {
        perror("Too many arguments");
        return 1;
    }

    i=0;
    while(i<3)
    {
        //Initialization of TCP socket for IPv4 protocol
        sd = socket(AF_INET, SOCK_STREAM, 0);
        control(sd, "Socket failed\n");

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
            server.sin_port = htons(80);
            server.sin_addr.s_addr = *(uint32_t *) ipaddr;
        }

        //Connect to remote server
        t = connect(sd, (struct sockaddr *)&server, sizeof(server));
        control(t, "Connection failed \n");

        //Writing on socket (Sending request to server)
        sprintf(request, "GET /reflect HTTP/1.1\r\nHost: 192.168.1.81\r\n\r\n");
        size = my_strlen(request);
        t = write(sd, request, size);
        control(t, "Write failed \n");

        //Parsing the response (HEADER + STATUS LINE)
        parse_header(sd, response, status_tokens, &header_size);

        //Parsing header values
        analysis_headers(status_tokens, h, &body_length, website);

        //Read body of the response
        body_acquire(sd, body_length, entity, &size);
        print_body(entity, size, 0);
        i++;

         for(k=1; k<30 && h[k].name[0]; k++)
            h[k].value=0;
    }

    return 0;
}

void parse_header(int sd, char* response, char** status_tokens, int* header_size)
{
    //Parsing response (HEADER+STATUS LINE)
    int j = 0;
    int k = 0;
    h[k].name= response;

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

    //Parsing Status Line
    *header_size = k;
    status_tokens[0]=h[0].name;
    j=1;
    k=1;

    for(j=0; k<3; j++)
    {
        if(h[0].name[j]==' ')
        {
            h[0].name[j]=0;
            status_tokens[k++]=h[0].name+j+1;
        }
    }
}

void analysis_headers(char **status_tokens, header* h, int* body_length, char* website)
{
    int code;
    int i;

    printf("\n");
    printf(LINE);
    printf(LINE);
    printf("                    HEADERS\n");
    printf(LINE);
    printf("Status line\n");
    printf("HTTP version: %30s\n", status_tokens[0]);
    code = atoi(status_tokens[1]);
    printf("HTTP code:    %30d\n", code);
    printf("HTTP comment: %30s\n", status_tokens[2]);
    printf(LINE);

    website=NULL;
    for(i=1; h[i].name[0]; i++)
    {
        if(!strcmp(h[i].name, "Content-Length"))
            (*body_length) = atoi(h[i].value);

        if(!strcmp(h[i].name, "Location") && code>300 && code<303)
            website=h[i].value;

        if(!strcmp(h[i].name, "Transfer-Encoding") && !strcmp(h[i].value," chunked"))
            (*body_length)=-1;

        printf("Name= %s -----> Value= %s\n",h[i].name, h[i].value);
    }
    printf(LINE);
    printf("\n\n");
}


void body_acquire(int sd, int body_length, char* entity, int *size)
{
    char c;
    int t;
    int chunk_size;

    printf(LINE);
    printf(LINE);
    if(body_length>0)
    {
        printf("Reading of HTTP/1.0 (Content-length specified)\n");
        for((*size)=0; (t=read(sd, entity+(*size), body_length-(*size)))>0; (*size)+=t);
    }
    if(body_length<0)
    {
        printf("Reading of HTTP/1.1 (chunked read)\n");
        printf(LINE);
        body_length=0;

        do
        {
            chunk_size=0;
            printf("HEX chunck size: ");

            while((t=read(sd, &c, 1))>0)
            {
                if(c=='\n')
                    break;

                else if(c=='\r')
                    continue;

                else
                    c = hex2dec(c);

                chunk_size = chunk_size*16+c;
            }

            control(t, "Chunk body read failed");

            printf("\nChunk size: %d\n",chunk_size);
            for((*size)=0; (t=read(sd, entity+body_length+(*size), chunk_size-(*size)))>0; (*size)+=t);

            read(sd, &c, 1);
            read(sd, &c, 1);

            body_length+=chunk_size;
            printf(LINE);
        }
        while(chunk_size>0);

        (*size)=body_length;
        printf("Size: %10d\n", *size);
    }
    else if(body_length==0)
    {
        printf("Reading of HTTP/0.9 (no Content-length specified)\n");
        for(*size=0; (t=read(sd, entity+(*size), 1000000-(*size)))>0; (*size)+=t);
    }
    printf(LINE);
    printf("\n\n");
}

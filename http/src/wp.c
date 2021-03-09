#include "wp.h"
#include "net_utility.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <netdb.h>

struct sockaddr_in local, remote;
struct hostent* he;

int main(int argc, char** argv)
{
    char request[2000];
    char *method, *path, *version;
    int sd, sd2;
    int t;
    socklen_t len;
    int yes = 1;

    //Initialization of TCP socket for IPv4 protocol between client and proxy
    sd = socket(AF_INET, SOCK_STREAM, 0);
    control(sd, "Socket failed \n");

    //Bind the server to a specific port
    local.sin_family=AF_INET;
    local.sin_port = htons(atoi(argv[1])); //we need to use a port not in use
    local.sin_addr.s_addr = 0; //By default

    //Reuse the same IP already bind to other program
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    t = bind(sd, (struct sockaddr*) &local, sizeof(struct sockaddr_in));
    control(t, "Bind failed \n");

    //Queue of pending clients that want to connect
    t = listen(sd, QUEUE_MAX);
    control(t, "Listen failed \n");

    if(t==-1)
    {
        printf("Errno: %d\n", errno);
        perror("Listen Failed");
        return 1;
    }

    while(1)
    {
        remote.sin_family = AF_INET;
        len = sizeof(struct sockaddr_in);

        //Accept the new request and create its socket
        sd2 = accept(sd, (struct sockaddr*) &remote, &len);
        control(sd2, "Accept failed \n");

        //A child manages the single request
        if(!fork())
        {
            //Read the request of the client
            t = read(sd2, request, 1999);
            request[t]=0;

            //Parser of request line
            request_line(request, &method, &path, &version);

            //Manage the response to the request
            manage_request(method, path, version, sd2);

            //Shutdown the socket created with the specific client
            shutdown(sd2, SHUT_RDWR);
            close(sd2);
            exit(0);
        }
    }
}


void request_line(char* request, char** method, char** path, char** version)
{
    int i;
    *method = request;

    for(i=0; request[i]!=' '; i++);

    request[i]=0;
    *path=request+i+1;

    for(; request[i]!=' '; i++);

    request[i]=0;
    *version=request+i+1;

    for(; (request[i]!='\n' || request[i-1]!='\r') ; i++);

    request[i-1]=0;
}

void manage_request(char* method, char* path, char* version, int sd2)
{
    char request2[2000], response[2000], response2[2000];
    int t;

    printf("Method:  %s\n", method);
    printf("Path:  %s\n", path);
    printf("Version:  %s\n", version);

    if(!strncmp(method,"GET", 3)) //GET request
    {
        printf("\n\nGET\n\n");
        char *scheme, *host, *resource;
        parser_path(path, &scheme, &host, &resource);

        int sd3 = connect2server(host, "80"); //HTTP service

        //Write the request to the server
        sprintf(request2, "GET /%s HTTP/1.1\r\nHost:%s\r\nConnection:close\r\n\r\n", resource, host);
        write(sd3, request2, strlen(request2));
        printf("request2: %s\n\n", request2);

        //Forward response from server to client
        while((t=read(sd3, response2, 2000)))
        {
            write(sd2, response2,t);
        }

        //Shutdown the socket created with the server
        shutdown(sd3, SHUT_RDWR);
        close(sd3);
    }
    else if(!strncmp(method,"CONNECT", 7))
    {
        printf("\n\nCONNECT\n\n");
        char *host, *port;
        parser_connect(path, &host, &port);

        int sd3 = connect2server(host, port);

        sprintf(response, "HTTP/1.1 200 Established\r\n\r\n");

        write(sd2, response, strlen(response));

        int pid;

        if((pid=fork())==0)//child to forward data from client to server
        {
            //Forwaring request from client to server
            while((t=read(sd2, request2, 2000)))
            {
                printf("C2P>> t: %d\n", t);
                write(sd3, request2, t);
            }

            exit(0);
        }
        else if(pid>0)//parent to forward data from server to client
        {
            //Forwarding response from server to client
            while((t=read(sd3, response2, 2000)))
            {
                printf("S2P>> t: %d\n", t);
                write(sd2, response2, t);
            }

            //Kill child (process that manages data from client to server)
            kill(pid,SIGTERM);

            //Shutdown the socket created with the server
            shutdown(sd3, SHUT_RDWR);
            close(sd3);
        }
        else
            printf("\n\nERROR: creation of process\n\n");
    }
}

void parser_path(char* path, char** scheme, char** host, char** resource)
{
    //http://www.ciao.it/path
    *scheme = path;

    int i=0;
    for(; path[i]!=':'; i++);
    path[i]=0;

    *host = path+i+3;
    for(i=i+3; path[i]!='/'; i++);
    path[i]=0;

    *resource = path +i+1;

    printf("Scheme=%s  Host=%s  Resource=%s\n", *scheme, *host, *resource);
}

void parser_connect(char* path, char** host, char** port)
{
    int i=0;

    //www.ciao.it:8080
    printf("\n\narrivato\n\n");
    *host = path;

    for(; path[i]!=':'; i++);
    path[i]=0;

    *port = path+i+1;

    printf("Host=%s  Port=%s\n", *host, *port);
}

int connect2server(char* host, char* port)
{
    struct sockaddr_in server;
    int t, sd3;

    //Resolve name to IP address using DNS
    struct hostent* he;
    he = gethostbyname(host);

    if(he == NULL)
    {
        perror("Gethostbyname Failed");
        exit(1);
    }

    //Initialization of TCP socket for IPv4 protocol between proxy and server
    sd3 = socket(AF_INET, SOCK_STREAM, 0);
    control(sd3, "Socket failed 2\n");

    //Connect proxy to remote server
    server.sin_family = AF_INET;
    server.sin_port = htons((unsigned short) atoi(port));
    server.sin_addr.s_addr = * (uint32_t*) he->h_addr;

    t = connect(sd3, (struct sockaddr*) &server, sizeof(struct sockaddr_in));
    control(t, "Connection failed 2\n");

    return sd3;
}

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

#define QUEUE_MAX 10

void request_line(char* request, char** method, char** path, char** version);
void manage_request(char* method, char* path, char* version, int sd2);
void parser_path(char* path, char* scheme, char* host, char* resource);
void parser_connect(char* path, char* host, char* port);
int connect2server(char* host, char* port);

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

    sd = socket(AF_INET, SOCK_STREAM, 0);

    if(sd == -1)
    {
        printf("Errno: %d\n", errno);
        perror("Socket failed");
        return 1;
    }

    local.sin_family=AF_INET;
    //local.sin_port = htons(80); no possible because port 80 already used
    local.sin_port = htons(atoi(argv[1])); //we need to use a port not in use 
    local.sin_addr.s_addr = 0; //By default, it 

    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    t = bind(sd, (struct sockaddr*) &local, sizeof(struct sockaddr_in));

    if(t==-1)
    {
        printf("Errno: %d\n", errno);
        perror("Bind failed");
        return 1;
    }

    //To prevent the connection to the server
    //Queue of pending clients that want to connect
    t = listen(sd, QUEUE_MAX);

    if(t==-1)
    {
        printf("Errno: %d\n", errno);
        perror("Listen Failed");
        return 1;
    }

    //The server can have more file descriptors mapped on the same port
    //only one socket = listening socket to extablish the connection (bind is unique)
    //then with accept a new socket is created (unique for each connection) that is bound to the same port
    //accept has all the info to disambiguate the connection
    //sd only to accept the connection, sd2 to read and write
    //sd1 = accept(socket);

    while(1)
    {
        remote.sin_family = AF_INET;
        len = sizeof(struct sockaddr_in);  
        sd2 = accept(sd, (struct sockaddr*) &remote, &len);

        if(sd2==-1)
        {
            perror("Accept failed\n");
            return 1;
        }

        if(!fork())
        {
            t = read(sd2, request, 1999);
            request[t]=0;
        
            request_line(request, &method, &path, &version); 
            manage_request(method, path, version, sd2);
            
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

    for(i=1; request[i]!=' '; i++);
    
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

    if(!strcmp(method,"GET")) //it's not GET request
    { 
        printf("\n\nGET");
        char *scheme, *host, *resource;
        parser_path(path, scheme, host, resource);

        int sd3 = connect2server(host, "80");
        
        sprintf(request2, "GET /%s HTTP/1.1\r\nHost:%s\r\nConnection:close\r\n\r\n", resource, host);
        
        write(sd3, request2, strlen(request2));
        printf("request2: %s\n\n", request2);
        
        while((t=read(sd3, response2, 2000)))
                write(sd2, response2,t);

        shutdown(sd3, SHUT_RDWR);
        close(sd3);
    }
    else if(!strcmp(method,"CONNECT"))
    {
        printf("\n\nCONNECT\n\n");
        char* host, *port;
        parser_connect(path, host, port);

        int sd3 = connect2server(host, port);
        sprintf(response, "HTTP/1.1 200 Established\r\n\r\n");
        
        write(sd2, response, strlen(request2));

       int pid;

        if(!(pid=fork()))//child to forward data from client to server
        {
            while((t=read(sd2, request2, 2000)))
            {
                printf("C2P>> t: %d\n", t);
                write(sd3, request2, t);
            }

            exit(0);
        }
        else //parent to forward data from server to client
        {
            while((t=read(sd3, response2, 2000)))
            {
                printf("S2P>> t: %d\n", t);
                write(sd2, response2, t);
            }

            kill(pid,SIGTERM);
            shutdown(sd3, SHUT_RDWR);
            close(sd3);
        }
    }
}

void parser_path(char* path, char* scheme, char* host, char* resource)
{
    //http://www.ciao.it/path
    scheme = path;

    int i=0;
    for(; path[i]!=':'; i++);
    path[i]=0;

    host = path+i+3;
    for(i=i+3; path[i]!='/'; i++);
    path[i]=0;
    
    resource = path +i+1;

    printf("Scheme=%s  Host=%s  Resource=%s\n", scheme, host, resource);
}

void parser_connect(char* path, char* host, char* port)
{
    int i=0;
    
    //www.ciao.it:8080
    host = path;
    
    for(; path[i]!=':'; i++);
    path[i]=0;

    port = path+i+1;
}

int connect2server(char* host, char* port)
{
    struct sockaddr_in server;
    int t, sd3;

    struct hostent* he;
    he = gethostbyname(host);

    if(he == NULL)
    {
        perror("Gethostbyname Failed");
        exit(1);
    }

        //Print the server address
    printf("Server address = %u.%u.%u.%u\n", (unsigned char) he->h_addr[0], (unsigned char) he->h_addr[1], (unsigned char) he->h_addr[2], (unsigned char) he->h_addr[3]);
        
    sd3 = socket(AF_INET, SOCK_STREAM, 0);

    if(sd3 == -1)
    {
        perror("Socket to server failed");
        exit(1);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons((unsigned short) atoi(port));
    server.sin_addr.s_addr = * (uint32_t*) he->h_addr;

    t = connect(sd3, (struct sockaddr*) &server, sizeof(struct sockaddr_in));

    if(t == -1)
    {
        perror("Connection to server failed");
        exit(1);
    }

    return sd3;
}

#include <sys/types.h>
#include <signal.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>

#include "net_utility.h"

#define BLUE "\033[1;34m"
#define CYAN "\033[1;36m"
#define DEFAULT "\033[0m"
#define GREEN "\033[1;32m"
#define MAGENTA "\033[1;35m"
#define RED "\033[1;31m"
#define YELLOW "\033[1;33m"

struct hostent * he;
struct sockaddr_in local, remote, server;

char request[10000],response[10000],request2[10000],response2[10000];
char *method, *path, *version, *host, *scheme, *resource, *port; 

struct headers {
char *n;
char *v;
}h[30];

int main()
{
    FILE *f;
    char command[100];
    int i,s,t,s2,s3,n,len,c,yes=1,j,k,pid; 
    int chunked = 0;
    int keep_alive;
    char conn2server_type[30];
    int body_length=0;
    int size=0;
    int chunk_size;

    s = socket(AF_INET, SOCK_STREAM, 0);
    if ( s == -1) { perror("Socket Failed\n"); return 1;}
    
    local.sin_family=AF_INET;
    local.sin_port = htons(8080);
    local.sin_addr.s_addr = 0;
    
    setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int));
    t = bind(s,(struct sockaddr *) &local, sizeof(struct sockaddr_in));
    if ( t == -1) { perror("Bind Failed \n"); return 1;}
    
    t = listen(s,10);
    if ( t == -1) { perror("Listen Failed \n"); return 1;}
    
    while( 1 )
    {
        f = NULL;
        remote.sin_family=AF_INET;
        len = sizeof(struct sockaddr_in);

        s2 = accept(s,(struct sockaddr *) &remote, &len);
        if(fork()) continue; 
        if (s2 == -1) {perror("Accept Failed\n"); return 1;}
        
        j=0;k=0;
        h[k].n = request;
        while(read(s2,request+j,1))
        {
            if((request[j]=='\n') && (request[j-1]=='\r'))
            {
                request[j-1]=0;
                
                if(h[k].n[0]==0)
                    break;
                
                h[++k].n=request+j+1;
            }
            if(request[j]==':' && (h[k].v==0) && k!=0)
            {
                request[j]=0;
                h[k].v=request+j+1;
            }
            j++;
        }
          
        printf("%s\n",request);
        method = request;
        for(i=0;(i<2000) && (request[i]!=' ');i++); request[i]=0;
        path = request+i+1;
        for(   ;(i<2000) && (request[i]!=' ');i++); request[i]=0;
        version = request+i+1;
        printf("Method = %s, path = %s, version = %s\n",method,path,version);
        
        if(!strcmp(version, "HTTP/1.0"))
        {
            for(i=1; h[i].n[0]; i++)
            {
                if(!strcmp(h[i].n, "Connection"))
                {
                    if(!strcmp(h[i].v, "keep-alive"))
                        keep_alive = 1;
                    else if(!strcmp(h[i].v, "close"))
                        keep_alive = 0;
                    else
                        keep_alive = -1;
                }
            }
        }
        else if(!strcmp(version, "HTTP/1.1"))
            keep_alive = 1;

        //for(i=1; h[i].n[0]; i++)
        //    printf("%s----->%s\n", h[i].n, h[i].v);

        if(keep_alive>0)
            strcpy(conn2server_type,"close");
        else if(keep_alive==0)
            strcpy(conn2server_type,"keep-alive");

        if(!strcmp("GET",method)) //it is a GET
        {
            //  http://www.google.com/path
            scheme=path;
            for(i=0;path[i]!=':';i++); path[i]=0;
            host=path+i+3; 
            for(i=i+3;path[i]!='/';i++); path[i]=0;
            resource=path+i+1;
            printf("Scheme=%s%s%s, host=%s%s%s, resource=%s%s%s\n", CYAN, scheme, DEFAULT, RED, host, DEFAULT, GREEN, resource, DEFAULT);

            he = gethostbyname(host);
            if (he == NULL) { printf("Gethostbyname Failed\n"); return 1;}
            printf("Server address = %u.%u.%u.%u\n", (unsigned char ) he->h_addr[0],(unsigned char ) he->h_addr[1],(unsigned char ) he->h_addr[2],(unsigned char ) he->h_addr[3]); 			
            
            s3=socket(AF_INET,SOCK_STREAM,0);
            if(s3==-1){perror("Socket to server failed"); return 1;}
            
            server.sin_family=AF_INET;
            server.sin_port=htons(80);
            server.sin_addr.s_addr=*(unsigned int*) he->h_addr;			
            t=connect(s3,(struct sockaddr *)&server,sizeof(struct sockaddr_in));		
            if(t==-1){perror("Connect to server failed"); return 1;}

            if(keep_alive>=0)
            {
                sprintf(request2,"GET /%s HTTP/1.0\r\nHost:%s\r\nConnection:%s\r\n\r\n", resource, host, conn2server_type);
                write(s3,request2,strlen(request2));
                printf("%s%s%s\n", BLUE, request2, DEFAULT);            
                //Read the answer of the server and forward it to client
                memset(h, 0, 30*sizeof(struct headers));

                j=0;k=0;
                h[k].n = response;
                
                while(read(s3,response+j,1))
                {
                    if((response[j]=='\n') && (response[j-1]=='\r'))
                    {
                        response[j-1]=0;
                
                        if(h[k].n[0]==0)
                            break;
                
                        h[++k].n=response+j+1;
                    }
                    
                    if(response[j]==':' && (h[k].v==0) && k!=0)
                    {
                        response[j]=0;
                        h[k].v=response+j+1;
                    }
                    j++;
                }

                sprintf(response2, "%s\r\n", h[0].n);
                printf("%s%s%s\n", CYAN, h[0].n, DEFAULT);
                write(s2, response2, strlen(response2));

                if(keep_alive)
                {
                    for(i=1; h[i].n[0]; i++)
                    {
                        if(!strcmp(h[i].n, "Content-Length"))
                            body_length=atoi(h[i].v);
                            
                        sprintf(response2, "%s:%s\r\n", h[i].n, h[i].v);
                        printf("%s%s:%s%s\n", YELLOW, h[i].n, DEFAULT, h[i].v);
                        write(s2, response2, strlen(response2));
                    }
                }

                if(keep_alive)//Keep-alive on client, close from server
                {
                    sprintf(response, "Transfer-Encoding:chunked\r\n\r\n");
                    write(s2, response, strlen(response));

                    if(body_length==0)
                        body_length = 10000;
             
                    for(size=0; (t=read(s3, response, body_length-size))>0; size+=t)
                    {
                        sprintf(response2, "%x\r\n", t);
                        write(s2, response2, strlen(response2));
                        write(s2, response, t);
                        write(s2, "\r\n", 2);
                    }
                    write(s2, "0\r\n\r\n", 5);
                }
                else
                {
                    body_length=0;
                    do
                    {
                        char c;

                        chunk_size=0;
                        while((t=read(s3, &c, 1))>0)
                        {
                            if(c=='\n')
                                break;
                            else if(c=='\r')
                                continue;
                            else
                                c=hex2dec(c);

                            chunk_size = chunk_size*16 + c;
                        }

                        if(t==-1)
                            perror("line 223");

                        for(size=0; (t=read(s3, response+body_length+size, chunk_size-size))>0; size+=t);
                        read(s3, &c, 1);
                        read(s3, &c, 1);

                        body_length+=chunk_size;
                    }
                    while(chunk_size>0);

                    sprintf(response2, "Content-Length:%d\r\n\r\n", body_length);
                    write(s2, response2, strlen(response2));
                    write(s2, response, body_length);
                }

            }
            else
            {
                sprintf(response2, "HTTP/1.1 400 Bad Request\r\n\r\n");
                write(s2, response2, strlen(response2));
            }
            
            shutdown(s3,SHUT_RDWR);
            close(s3);
        }
        else if(!strcmp("CONNECT",method))  //CONNECT
        {    
            host=path;
            for(i=0;path[i]!=':';i++); path[i]=0;
            port=path+i+1;
            printf("host:%s, port:%s\n",host,port);
            
            he = gethostbyname(host);
            if (he == NULL) { printf("Gethostbyname Failed\n"); return 1;}
            printf("Connecting to address = %u.%u.%u.%u\n", (unsigned char ) he->h_addr[0],(unsigned char ) he->h_addr[1],(unsigned char ) he->h_addr[2],(unsigned char ) he->h_addr[3]); 			
            s3=socket(AF_INET,SOCK_STREAM,0);
            if(s3==-1){perror("Socket to server failed"); return 1;}

            server.sin_family=AF_INET;
            server.sin_port=htons((unsigned short)atoi(port));
            server.sin_addr.s_addr=*(unsigned int*) he->h_addr;			
            t=connect(s3,(struct sockaddr *)&server,sizeof(struct sockaddr_in));		
            if(t==-1){perror("Connect to server failed"); exit(0);}
            
            sprintf(response,"HTTP/1.1 200 Established\r\n\r\n");
            write(s2,response,strlen(response));
            
            if(!(pid=fork()))
            { 
                //Child
                while(t=read(s2,request2,2000))
                {	
                    write(s3,request2,t);
                    printf("CL >>>(%d)%s \n",t,host); //SOLO PER CHECK
                }

                exit(0);
            }
            else 
            { 
                //Parent	
                while(t=read(s3,response2,2000))
                {	
                    write(s2,response2,t);
                    printf("CL <<<(%d)%s \n",t,host);
                }	
                
                kill(pid,15);
                shutdown(s3,SHUT_RDWR);
                close(s3);
            }	
        }

        shutdown(s2,SHUT_RDWR);
        close(s2);	
        exit(0);
    }

    return 0;
}

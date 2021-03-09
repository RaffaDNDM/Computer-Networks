#include <sys/types.h>          /* See NOTES */
#include <signal.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>

#include "net_utility.h"
#define NUM_BLOCKED_IP 4

struct hostent * he;
struct sockaddr_in local,remote,server;
char request[10000],response[2000],request2[2000],response2[2000];
char * method, *path, *version, *host, *scheme, *resource,*port;
char blocked_IPs[NUM_BLOCKED_IP][4] = {{192,168,1,81},
                                       {192,168,1,210},
                                       {192,168, 1,14},
                                       {192,165,22,1}};

struct headers {
    char *n;
    char *v;
}h[30];

int main()
{
    FILE *f;
    char *type, *sub_type;
    char command[100], c;
    int i,s,t,s2,s3,n,len,yes=1,j,k,pid,size, block=0;

    s = socket(AF_INET, SOCK_STREAM, 0);
    if ( s == -1) 
    { 
        perror("Socket Failed\n"); 
        return 1;
    }

    local.sin_family=AF_INET;
    local.sin_port = htons(8080);
    local.sin_addr.s_addr = 0;
    setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int));
    t = bind(s,(struct sockaddr *) &local, sizeof(struct sockaddr_in));
    if ( t == -1) 
    {
        perror("Bind Failed \n"); 
        return 1;
    }

    t = listen(s,10);
    if ( t == -1) 
    { 
        perror("Listen Failed \n"); 
        return 1;
    }

    while( 1 )
    {
        f = NULL;
        remote.sin_family=AF_INET;
        len = sizeof(struct sockaddr_in);
   
        s2 = accept(s,(struct sockaddr *) &remote, &len);
        for(i=0; i<NUM_BLOCKED_IP; i++)
        {
            if(block = ((*(unsigned int*) blocked_IPs[i]) == remote.sin_addr.s_addr))
                break;
        }
        printf("remote: ");
        for(i=0; i<3; i++)
            printf("%u.", ((unsigned char*) &remote.sin_addr.s_addr)[i]);
        printf("%u      block: %d\n", ((unsigned char*) &remote.sin_addr.s_addr)[i], block);

        if(fork()) continue; //<< MULTI PROCESS HADLING
        if (s2 == -1) 
        {
            perror("Accept Failed\n"); 
            return 1;
        }

        // <---- ADDED HEADER PARSER
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

		method = request;
		for(i=0;(i<2000) && (request[i]!=' ');i++); request[i]=0;
		path = request+i+1;
		for(   ;(i<2000) && (request[i]!=' ');i++); request[i]=0;
		version = request+i+1;
		printf("\n%s%s %s %s%s\n", BOLD_GREEN, method, path, version, DEFAULT);	
		
        if(!strcmp("GET",method))
        {
            //  http://www.google.com/path
			scheme=path;
			for(i=0;path[i]!=':';i++); path[i]=0;
			host=path+i+3; 
			for(i=i+3;path[i]!='/';i++); path[i]=0;
			resource=path+i+1;
            printf("Scheme=%s, host=%s, resource = %s\n", scheme,host,resource);
			
            he = gethostbyname(host);
			if (he == NULL) 
            { 
                printf("Gethostbyname Failed\n"); 
                return 1;
            }

			printf("Server address = %u.%u.%u.%u\n", (unsigned char ) he->h_addr[0],(unsigned char ) he->h_addr[1],(unsigned char ) he->h_addr[2],(unsigned char ) he->h_addr[3]); 			
			s3=socket(AF_INET,SOCK_STREAM,0);
			if(s3==-1)
            {
                perror("Socket to server failed"); 
                return 1;
            }
			
            server.sin_family=AF_INET;
			server.sin_port=htons(80);
		 	server.sin_addr.s_addr=*(unsigned int*) he->h_addr;			
			t=connect(s3,(struct sockaddr *)&server,sizeof(struct sockaddr_in));		
            if(t==-1)
            {
                perror("Connect to server failed"); 
                return 1;
            }

			sprintf(request2,"GET /%s HTTP/1.1\r\nHost:%s\r\nConnection:close\r\n\r\n",resource,host);
            write(s3,request2,strlen(request2)); 
		    	
            memset(h, 0, 30*sizeof(struct headers));

            j=0;k=0;
            h[k].n = response;
            while((t=read(s3,response+j,1))>0)
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

            if(t==-1)
            {
                perror("Error on message");
                exit(1);
            }

            if(block)
            {
                for(i=1; h[i].n[0]; i++)
                {
                    if(!strcmp(h[i].n, "Content-Type"))
                    {
                        type = h[i].v;
                        for(j=0; h[i].v[j]!='/'; j++);
                        h[i].v[j]=0;
                        
                        printf("%s%15s%s/", BOLD_YELLOW, type, DEFAULT);

                        if(!strcmp(type, " text")) 
                        {
                            block=0;
                            h[i].v[j]='/';
                        }

                        sub_type = h[i].v + j +1;
                        int size_sub=strlen(sub_type);
                        for(j=j+1; j<size_sub && h[i].v[j]!=';'; j++);
                        h[i].v[j]=0;

                        printf("%s%-15s%s", BOLD_CYAN, sub_type, DEFAULT);
                        
                        if(block && !strcmp(sub_type, "html"))
                            block = 0;

                        if(j<size_sub)
                            h[i].v[j]=';';
                        
                        break;
                    }
                }
            }
            
            if(block)
            {
                sprintf(response2, "HTTP/1.1 401 Unauthorized\r\n\r\n");
                write(s2, response2, strlen(response2));
                printf("      %s%s%s", BOLD_RED, response2, DEFAULT);
            }
            else
            {
                sprintf(response2, "%s\r\n", h[0].n);
                write(s2, response2, strlen(response2));
                printf("      %s%s%s", BOLD_BLUE, response2, DEFAULT);

                for(i=1; h[i].n[0]; i++)
                {
                    sprintf(response2, "%s:%s\r\n", h[i].n, h[i].v);
                    write(s2, response2, strlen(response2));
                }

                sprintf(response2, "\r\n");
                write(s2, response2, 2);

                while((t=read(s3, response2, 2000))>0)
                    write(s2, response2, t);

                if(t==-1)
                {
                    perror("[PROXY ERROR] Reading server response");
                    exit(1);
                }
            }

            shutdown(s3,SHUT_RDWR);
            close(s3);
		}
		else if(!strcmp("CONNECT",method)) 
        { 
			host=path;
			for(i=0;path[i]!=':';i++); path[i]=0;
			port=path+i+1;
			printf("host:%s, port:%s\n",host,port);
			printf("Connect skipped ...\n");	
			he = gethostbyname(host);
			if (he == NULL) 
            { 
                printf("Gethostbyname Failed\n"); 
                return 1;
            }

			printf("Connecting to address = %u.%u.%u.%u\n", (unsigned char ) he->h_addr[0],(unsigned char ) he->h_addr[1],(unsigned char ) he->h_addr[2],(unsigned char ) he->h_addr[3]); 			
			s3=socket(AF_INET,SOCK_STREAM,0);
			if(s3==-1)
            {
                perror("Socket to server failed"); 
                return 1;
            }

			server.sin_family=AF_INET;
			server.sin_port=htons((unsigned short)atoi(port));
		 	server.sin_addr.s_addr=*(unsigned int*) he->h_addr;			
			t=connect(s3,(struct sockaddr *)&server,sizeof(struct sockaddr_in));		
            if(t==-1)
            {
                perror("Connect to server failed"); 
                exit(0);
            }
			
            sprintf(response,"HTTP/1.1 200 Established\r\n\r\n");
			write(s2,response,strlen(response));
			
            if(!(pid=fork())) //Child
            { 
				while(t=read(s2,request2,2000))
                {	
                    write(s3,request2,t);
					printf("CL >>>(%d)%s \n",t,host);
                }	
				exit(0);
            }
			else //Parent	
            { 
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
}

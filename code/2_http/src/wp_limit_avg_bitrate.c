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
#include <sys/time.h>

#define BOLD_RED "\033[1;31m"
#define BOLD_YELLOW "\033[1;33m"
#define BOLD_BLUE "\033[1;34m"
#define BOLD_CYAN "\033[1;36m"
#define DEFAULT "\033[0m"

struct hostent * he;
struct sockaddr_in local,remote,server;
char request[2000],response[2000],request2[2000],response2[2000];
char * method, *path, *version, *host, *scheme, *resource,*port;

struct headers {
char *n;
char *v;
}h[30];

int main()
{
    FILE *f;
    char command[100];
    int i,s,t,s2,s3,n,len,c,yes=1,j,k,pid; 
    
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
        if(fork()) continue;        
        if (s2 == -1) 
        {
            perror("Accept Failed\n"); 
            return 1;
        }
        
        j=0;k=0;
        h[k].n = request;
        while(read(s2,request+j,1))
        {
            if((request[j]=='\n') && (request[j-1]=='\r'))
            {
                request[j-1]=0;
                if(h[k].n[0]==0) break;
                h[++k].n=request+j+1;
            }
            if(request[j]==':' && (h[k].v==0) && k!=0)
            {
                request[j]=0;
                h[k].v=request+j+1;
            }
            
            j++;
        }

		printf("%s",request);
		method = request;
		for(i=0;(i<2000) && (request[i]!=' ');i++); request[i]=0;
		path = request+i+1;
		for(   ;(i<2000) && (request[i]!=' ');i++); request[i]=0;
		version = request+i+1;
		printf("Method = %s, path = %s , version = %s\n",method,path,version);	
		
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

            printf("Server address = %u.%u.%u.%u\n", (unsigned char) he->h_addr[0], (unsigned char) he->h_addr[1],
                                                     (unsigned char) he->h_addr[2], (unsigned char ) he->h_addr[3]); 			
            
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
            
            while(t=read(s3,response2,2000))	
                write(s2,response2,t);	
            
            shutdown(s3,SHUT_RDWR);
            close(s3);
		}
		else if(!strcmp("CONNECT",method)) 
        {  
			host=path;
			for(i=0;path[i]!=':';i++); path[i]=0;
			port=path+i+1;
			printf("host:%s, port:%s\n",host,port);
			
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
                struct timeval t1;
                struct timeval t2;
                suseconds_t diff_sec, diff_usec, estimated_sec, estimated_usec;
                
                if(gettimeofday(&t1, NULL))
                {
                    printf("[PROXY ERROR] gettimeofday\n");
                    exit(1);
                }
				
                while(t=read(s2,request2,2000))
                {	
					write(s3,request2,t);
                    
                    if(gettimeofday(&t2, NULL))
                    {
                        printf("[PROXY ERROR] gettimeofday\n");
                        exit(1);
                    }
					
                    estimated_usec = t*8000;
                    diff_sec = t2.tv_sec - t1.tv_sec;
                    
                    if(t2.tv_usec>t1.tv_usec)
                        diff_usec = t2.tv_usec - t1.tv_usec;
                    else
                    {
                        diff_usec = t2.tv_usec +1000000 - t1.tv_usec;
                        diff_sec--;
                    }

                    struct timeval t3;
                    memcpy(&t3, &t1, sizeof(t1));

                    if((diff_sec*1000000+diff_usec)<estimated_usec)
                        usleep(estimated_usec-diff_sec*1000000-diff_usec);

                    if(gettimeofday(&t1, NULL))
                    {
                        printf("[PROXY ERROR] gettimeofday\n");
                        exit(1);
                    }

                    if(t1.tv_usec>t3.tv_usec)
                        diff_usec = t1.tv_usec - t3.tv_usec;
                    else
                    {
                        diff_usec = t1.tv_usec +1000000 - t3.tv_usec;
                        diff_sec--;
                    }

                    printf("%sBitrate:%s  %lf Kbit/s    ", BOLD_RED, DEFAULT, ((double) (t*8*1000000))/(diff_sec*1000000+diff_usec));
					printf("%sC >>> S(%s%d%s)%s%s%s \n",BOLD_RED, DEFAULT, t, BOLD_RED, BOLD_YELLOW, host, DEFAULT);
				}	
				
                exit(0);
            }
			else //Parent	
            { 
                struct timeval t1;
                struct timeval t2;
                suseconds_t diff_sec, diff_usec, estimated_sec, estimated_usec;
                
                if(gettimeofday(&t1, NULL))
                {
                    printf("[PROXY ERROR] gettimeofday\n");
                    exit(1);
                }

				while(t=read(s3,response2,2000))
                {	
					write(s2,response2,t);
                    
                    if(gettimeofday(&t2, NULL))
                    {
                        printf("[PROXY ERROR] gettimeofday\n");
                        exit(1);
                    }
					
                    estimated_usec = t*800;
                    diff_sec = t2.tv_sec - t1.tv_sec;
                    
                    if(t2.tv_usec>t1.tv_usec)
                        diff_usec = t2.tv_usec - t1.tv_usec;
                    else
                    {
                        diff_usec = t2.tv_usec +1000000 - t1.tv_usec;
                        diff_sec--;
                    }

                    if((diff_sec*1000000+diff_usec)<estimated_usec)
                        usleep(estimated_usec-diff_sec*1000000-diff_usec);

                    struct timeval t3;
                    memcpy(&t3, &t1, sizeof(t1));

                    if((diff_sec*1000000+diff_usec)<estimated_usec)
                        usleep(estimated_usec-diff_sec*1000000-diff_usec);

                    if(gettimeofday(&t1, NULL))
                    {
                        printf("[PROXY ERROR] gettimeofday\n");
                        exit(1);
                    }

                    if(t1.tv_usec>t3.tv_usec)
                        diff_usec = t1.tv_usec - t3.tv_usec;
                    else
                    {
                        diff_usec = t1.tv_usec +1000000 - t3.tv_usec;
                        diff_sec--;
                    }

                    printf("%sBitrate:%s  %lf bit/s    ", BOLD_BLUE, DEFAULT, (((double) t)*8*1000000)/(diff_sec*1000000+diff_usec));
					printf("%sC >>> S(%s%d%s)%s%s%s \n",BOLD_BLUE, DEFAULT, t, BOLD_BLUE, BOLD_CYAN, host, DEFAULT);
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

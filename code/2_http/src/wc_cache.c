#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <arpa/inet.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>

#define __USE_XOPEN
#include <time.h>

struct sockaddr_in server;

struct headers {
    char *n;
    char *v;
}h[30];

#define LINE_SIZE 100

int main(int argc, char** argv)
{
    int s,t,size,i,j,k;
    char request[100],response[1000000];
    unsigned char ipaddr[4]={192,168,1,81};
    int bodylength=0;
    char resource[50];
    char resource_path[50] = "./cache/";
    FILE* f;
    int head=0;
    char line[LINE_SIZE];
    int is_updated=0;
    time_t download_time;
    time_t last_time;
    char *version, *code, *comment;

    s = socket(AF_INET, SOCK_STREAM, 0);
    if ( s == -1) { printf("Errno = %d\n", errno); perror("Socket Failed"); return 1; }
    
    server.sin_family = AF_INET;
    server.sin_port = htons(8083);
    server.sin_addr.s_addr = *(uint32_t *)ipaddr;
    // WRONG : server.sin_addr.s_addr = (uint32_t )*ipaddr
    
    t = connect(s, (struct sockaddr *)&server, sizeof(server));
    if ( t == -1) { perror("Connect Failed"); return 1; }

    strcpy(resource, argv[1]);
    for(i=0; i<strlen(argv[1]); i++)
    {
        if(argv[1][i]=='/')
            resource[i]='_'; 
    }

    strcat(resource_path, resource);
    if((f=fopen(resource_path, "r"))!=NULL)
    {
        sprintf(request,"HEAD %s HTTP/1.0\r\nHost:192.168.1.81\r\n\r\n", argv[1]);
        head=1;
    }
    else
        sprintf(request,"GET %s HTTP/1.0\r\nHost:192.169.1.81\r\n\r\n", argv[1]);

    for(size=0;request[size];size++);
    t=write(s,request,size);
    if ( t == -1 ) { perror("Write failed"); return 1; }
	
    j=0;k=0;
	h[k].n = response;
	while(read(s,response+j,1))
    {
        if((response[j]=='\n') && (response[j-1]=='\r'))
        {
            response[j-1]=0;
            if(h[k].n[0]==0) break;
            h[++k].n=response+j+1;
        }

        if(response[j]==':' && (h[k].v==0) )
        {
            response[j]=0;
            h[k].v=response+j+1;
        }
        
        j++;
	}

    char *last_modified = NULL;
    
    version = h[0].n;
    for(i=0; h[0].n[i]!=' '; i++);
    h[0].n[i]=0;

    i++;
    code = h[0].n+i;
    for(; h[0].n[i]!=' '; i++);
    h[0].n[i]=0;

    comment = h[0].n+i+1;
   
    printf("%s %s %s\n", version, code, comment);
    for(i=1;h[i].n[0];i++)
    {
		if (!strcmp(h[i].n,"Content-Length"))
            bodylength = atoi(h[i].v);
        else if (!strcmp(h[i].n, "Last-Modified"))
            last_modified = h[i].v;
            
		printf("%s:%s\n",h[i].n,h[i].v);
    }
	 
    if(head && last_modified!=NULL)
    {
        //fopen works well
        struct tm tm, tm2;
        memset(&tm, 0, sizeof(tm));
        strptime(last_modified, "%a, %d %b %Y %H:%M:%S %Z", &tm); 
        last_time = timegm(&tm);

        time_t cache_time;
        char date[100];
        fgets(date, 100, f);
        strptime(date, "%a, %d %b %Y %H:%M:%S %Z", &tm2); 
        cache_time = timegm(&tm2);

        if(cache_time<last_time)
        {
            shutdown(s, SHUT_RDWR);
            close(s);
            s = socket(AF_INET, SOCK_STREAM, 0);
            if ( s == -1) { printf("Errno = %d\n", errno); perror("Socket Failed"); return 1; }
            
            server.sin_family = AF_INET;
            server.sin_port = htons(8083);
            server.sin_addr.s_addr = *(uint32_t *)ipaddr;
            // WRONG : server.sin_addr.s_addr = (uint32_t )*ipaddr
            
            t = connect(s, (struct sockaddr *)&server, sizeof(server));
            if ( t == -1) { perror("Connect Failed"); return 1; }

            sprintf(request,"GET %s HTTP/1.0\r\nHost:192.168.1.81\r\n\r\n", argv[1]);
            write(s, request, strlen(request));        
           
            for(i=0; h[i].n[0]; h[i++].v=0); 
            
            j=0;k=0;
            h[k].n = response;
            while(read(s,response+j,1))
            {
                printf("%c", response[j]);
                if((response[j]=='\n') && (response[j-1]=='\r'))
                {
                    response[j-1]=0;
                    if(h[k].n[0]==0) break;
                    h[++k].n=response+j+1;
                }

                if(response[j]==':' && (h[k].v==0) )
                {
                    response[j]=0;
                    h[k].v=response+j+1;
                }
                
                j++;
            }
        }
        else
            is_updated=1;
        
        fclose(f);
    }
    
    if(!is_updated)
    {
        //fopen works bad
        assert((f= fopen(resource_path, "w"))!=NULL);
        
        if (bodylength) // we have content-length
	 	    for(size=0; (t=read(s,response+size,bodylength-size))>0;size=size+t);
	    else
	 	    for(size=0; (t=read(s,response+size,1000000-size))>0;size=size+t);

	    if ( t == -1 ) { perror("Read failed"); return 1; }

        response[size]=0; 
      

        char down_time[30];
        time_t download_time = time(0);
        struct tm* tm=gmtime(&download_time);
        strftime(down_time, 30, "%a, %d %b %Y %H:%M:%S %Z", tm); 
        fprintf(f, "%s\n%s", down_time, response);

        fclose(f);
    }
    else
	 	for(size=0; (t=read(s,response+size,1000000-size))>0;size=size+t);

    return 0;
}

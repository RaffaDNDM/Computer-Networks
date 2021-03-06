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
//#define USE_GMT

struct sockaddr_in server;

struct headers {
    char *n;
    char *v;
}h[30];

#define LINE_SIZE 100

int main(int argc, char** argv)
{
    int s,t,size,i,j,k;
    char *version, *code, *comment;
    char request[100],response[1000000];
    unsigned char ipaddr[4]={93,184,216,34};
    int bodylength=0;
    char resource[50];
    char resource_path[50] = "./cache/";
    FILE* f;
    char line[LINE_SIZE];
    int is_updated=0;
    time_t download_time;
    time_t last_time;
    
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
        char date[100];
        fgets(date, 100, f);
        fclose(f);

        sprintf(request,"GET %s HTTP/1.0\r\nHost:www.example.com\r\nIf-Modified-Since:%s\r\n\r\n", argv[1], date);
    }
    else
    {
        sprintf(request,"GET %s HTTP/1.0\r\nHost:www.example.com\r\n\r\n", argv[1]);
    }

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
    for(i=0; response[i]!=' '; i++);
    response[i]=0;

    code = h[0].n+i+1;
    for(i=i+1; response[i]!=' '; i++);
    response[i]=0;

    comment = h[0].n+i+1;
    printf("%s %s %s\n", version, code, comment);

	for(i=1;h[i].n[0];i++)
    {
		if (!strcmp(h[i].n,"Content-Length"))
            bodylength = atoi(h[i].v);
            
		printf("%s:%s\n",h[i].n,h[i].v);
    }
	 
    if(strcmp(code, "304"))
    {
        //fopen works bad
        assert((f= fopen(resource_path, "w"))!=NULL);
        
        if (bodylength) // we have content-length
	 	    for(size=0; (t=read(s,response+size,bodylength-size))>0;size=size+t);
	    else
	 	    for(size=0; (t=read(s,response+size,1000000-size))>0;size=size+t);

	    if ( t == -1 ) { perror("Read failed"); return 1; }

        response[size]=0; 
      

        char down_time[40];
        time_t download_time = time(0);
        struct tm* tm;
        
        #ifdef USE_GMT
            tm=gmtime(&download_time);
        #else
            tm=localtime(&download_time);
        #endif

        strftime(down_time, 40, "%a, %d %b %Y %H:%M:%S %Z", tm); 
        fprintf(f, "%s\n%s", down_time, response);

        fclose(f);
    }

    return 0;
}

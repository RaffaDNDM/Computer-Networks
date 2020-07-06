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
struct sockaddr_in server;
struct headers {
char *n;
char *v;
}h[30];
int main()
{
	 int s,t,size,i,j,k,l;
	 unsigned char c;
	 int L;
	 char request[100],response[10000],entity[1000000];
	 unsigned char ipaddr[4]={192,168,1,81};
	 int bodylength=0;
         s = socket(AF_INET, SOCK_STREAM, 0);
	 if ( s == -1) { printf("Errno = %d\n", errno); perror("Socket Failed"); return 1; }
	 server.sin_family = AF_INET;
	 server.sin_port = htons(8083); //Ex: Write a function to revert endianness
	 server.sin_addr.s_addr = *(uint32_t *)ipaddr;
	 // WRONG : server.sin_addr.s_addr = (uint32_t )*ipaddr
	 t = connect(s, (struct sockaddr *)&server, sizeof(server));
	 if ( t == -1) { perror("Connect Failed"); return 1; }
	 sprintf(request,"GET /prova.html HTTP/1.1\r\nHost: 192.168.1.81\r\n\r\n"); // request[0]='G', request[1]='E',... request[7]=0 (or '\0') 
	 for(size=0;request[size];size++);
	 t=write(s,request,size);	
	 if ( t == -1 ) { perror("Write failed"); return 1; }
	j=0;k=0;
	h[k].n = response;
	while(read(s,response+j,1)){
        if((response[j]=='\n') && (response[j-1]=='\r')){
                response[j-1]=0;
                if(h[k].n[0]==0) break;
                h[++k].n=response+j+1;
                }
        if(response[j]==':' && (h[k].v==0) ){
                response[j]=0;
                h[k].v=response+j+1;
                }
        j++;
	}
	printf("Status line: %s\n",h[0].n);
	for(i=1;h[i].n[0];i++){
		if (!strcmp(h[i].n,"Content-Length")) { bodylength = atoi(h[i].v);} 
		if ((!strcmp(h[i].n,"Transfer-Encoding")) && (!strcmp(h[i].v," chunked")))
			bodylength=-1; 

		printf("Name = %s ---> Value = %s\n",h[i].n,h[i].v);
		}
	 if (bodylength>0){ // we have content-length
	 	for(size=0; (t=read(s,entity+size,bodylength-size))>0;size=size+t);
	 	if ( t == -1 ) { perror("Chunk Body Read Failed"); return 1; }
		}
	 else if (bodylength==0){ // Connection Close 
	 	for(size=0; (t=read(s,entity+size,1000000-size))>0;size=size+t);
	 	if ( t == -1 ) { perror("Chunk Body Read Failed"); return 1; }
		}
	else if (bodylength== -1){ //Chunked
		printf("Chunked!");	
		L=0;
		while(1){l=0;
			while(t=read(s,&c,1)){
				if (t==0) break;
				if (t==-1) { perror ("Chunk size read failed\n"); return 1;}
				if (c=='\n') break;
				if (c=='\r') continue;
				printf("length digit:%c\n",c);
				switch(c){
						case '0' ... '9': c = c - '0'; break;
						case 'A' ... 'F': c = c + 10 - 'A'; break;
						case 'a' ... 'f': c = c + 10 - 'a'; break;
						default: printf("Ill-formed chunk\n"); return 1;
						}
						l = l*16 + c;
				}
			printf("New Chunk: Length =%d\n",l);
			if (l==0) break;
	 		for(size=0; (t=read(s,entity+L+size,l-size))>0;size+=t);
	 		if ( t == -1 ) { perror("Chunk Body Read Failed"); return 1; }
			printf("size = %d\n",size);
			read(s,&c,1); printf("CR = %d\n", c);  // CR after chunk body
			read(s,&c,1); printf("LF = %d\n", c);// LF after chunk body
			L=L+l;
			}
		size = L;
		}
	 for(i=0;i<size;i++) 
	 	printf("%c",entity[i]);
	}

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
struct hostent * he;
struct sockaddr_in local,remote;
char request[2000],response[2000];
char * method, *path, *version, *host, *scheme, *resource;
int main()
{
FILE *f;
char command[100];
int i,s,t,s2,n,len,c,yes=1; 
s = socket(AF_INET, SOCK_STREAM, 0);
if ( s == -1) { perror("Socket Failed\n"); return 1;}
local.sin_family=AF_INET;
local.sin_port = htons(8083);
local.sin_addr.s_addr = 0;
setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int));
t = bind(s,(struct sockaddr *) &local, sizeof(struct sockaddr_in));
if ( t == -1) { perror("Bind Failed \n"); return 1;}
t = listen(s,10);
if ( t == -1) { perror("Listen Failed \n"); return 1;}
while( 1 ){
	f = NULL; // <<<<< BACO
	remote.sin_family=AF_INET;
	len = sizeof(struct sockaddr_in);
	s2 = accept(s,(struct sockaddr *) &remote, &len);
	if (s2 == -1) {perror("Accept Failed\n"); return 1;}
		n=read(s2,request,1999);
		request[n]=0;
		printf("%s",request);
		method = request;
		for(i=0;(i<2000) && (request[i]!=' ');i++); request[i]=0;
		path = request+i+1;
		for(   ;(i<2000) && (request[i]!=' ');i++); request[i]=0;
		version = request+i+1;
		for(   ;(i<2000) && (request[i]!='\r');i++); request[i]=0;
		printf("Method = %s, path = %s , version = %s\n",method,path,version);	
		if(!strcmp("GET",method)){ // it is a GET
			//  http://www.google.com/path
			scheme=path;
			for(i=0;path[i]!=':';i++); path[i]=0;
			host=path+i+3; 
			for(i=i+3;path[i]!='/';i++); path[i]=0;
			resource=path+i+1;
			printf("Scheme=%s, host=%s, resource = %s\n", scheme,host,resource);
			he = gethostbyname(host);
			if (he == NULL) { printf("Gethostbyname Failed\n"); return 1;}
			printf("Server address = %u.%u.%u.%u\n", (unsigned char ) he->h_addr[0],(unsigned char ) he->h_addr[1],(unsigned char ) he->h_addr[2],(unsigned char ) he->h_addr[3]); 			
		}
		else if(!strcmp("CONNECT",method)) { // it is a connect 
			printf("Connect skipped ...\n");	
		}
		shutdown(s2,SHUT_RDWR);
		close(s2);	
	}
}

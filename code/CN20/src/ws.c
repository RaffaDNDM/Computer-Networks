#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

struct sockaddr_in local,remote;
char request[2000],response[2000];
char * method, *path, *version;
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
	if (!fork()){
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
		if(strcmp("GET",method)) // it is not a GET
			sprintf(response, "HTTP/1.1 501 Not Implemented\r\n\r\n");
		else { // it is a get
			if(!strncmp(path,"/cgi-bin/",9)){ // CGI interface  
				sprintf(command,"%s > results.txt",path+9);
				printf("executing %s\n", command);
				system(command);
				if((f=fopen("results.txt","r"))==NULL){
					printf("cgi bin error\n");
					return 1;
					}
			sprintf(response,"HTTP/1.1 200 OK\r\nConnection:close\r\n\r\n");
			}
			else  if((f=fopen(path+1,"r"))==NULL)
				sprintf(response,"HTTP/1.1 404 Not Found\r\nConnection:Close\r\n\r\n");
			else sprintf(response,"HTTP/1.1 200 OK\r\nConnection:close\r\n\r\n");
		}
		write(s2,response,strlen(response)); // HTTP Headers
		if(f!=NULL){ // if present, the Entity Body 
			while((c=fgetc(f))!=EOF)
				write(s2,&c,1);
			fclose(f); 
			}
		shutdown(s2,SHUT_RDWR);
		close(s2);	
		exit(0);
	}
	}
}

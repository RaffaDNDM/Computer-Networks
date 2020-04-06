#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <arpa/inet.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
struct sockaddr_in server;
int main()
{
	 int s,t,size,i;
	 char request[100],response[1000000];
	 unsigned char ipaddr[4]={216,58,211,163};
         s = socket(AF_INET, SOCK_STREAM, 0);
	 if ( s == -1) { printf("Errno = %d\n", errno); perror("Socket Failed"); return 1; }
	 server.sin_family = AF_INET;
	 server.sin_port = htons(80); //Ex: Write a function to revert endianness
	 server.sin_addr.s_addr = *(uint32_t *)ipaddr;
	 // WRONG : server.sin_addr.s_addr = (uint32_t )*ipaddr
	 t = connect(s, (struct sockaddr *)&server, sizeof(server));
	 if ( t == -1) { perror("Connect Failed"); return 1; }
	 sprintf(request,"GET /\r\n"); // request[0]='G', request[1]='E',... request[7]=0 (or '\0') 
	 for(size=0;request[size];size++);
	 t=write(s,request,size);	
	 if ( t == -1 ) { perror("Write failed"); return 1; }

	 for(size=0; (t=read(s,response+size,1000000-size))>0;size=size+t);
	 if ( t == -1 ) { perror("Read failed"); return 1; }
	 for(i=0;i<size;i++) 
	 	printf("%c",response[i]);
}

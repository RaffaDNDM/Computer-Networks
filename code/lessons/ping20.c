#include <stdio.h>
#include <arpa/inet.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h> /* the L2 protocols */
#include <net/if.h>

unsigned char myip[4]={88,80,187,84};
unsigned char mymac[6]={0xf2,0x3c,0x91,0xdb,0xc2,0x98};
unsigned char targetip={   };


unsigned char buffer[1500];

struct sockaddr_ll sll;

int printpacket(unsigned char *b,int l){
int i;
 for(i=0;i<l;i++){
	printf("%.2x(%.3d) ",b[i],b[i]);  
	if(i%4 == 3) printf("\n");
	}
}	
	

int main(){

int n,s,len ;

s = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)); 

sll.sll_family = AF_PACKET;
sll.sll_ifindex = if_nametoindex("eth0");

len = sizeof(sll);

if (n=recvfrom(s,buffer,1500, 0,(struct sockaddr *)&sll,&len));
if (n == -1) {perror("Recvfrom failed"); return 0;}
printpacket(buffer,n);

}

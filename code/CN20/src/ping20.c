#include <stdio.h>
#include <arpa/inet.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h> /* the L2 protocols */
#include <net/if.h>
#include <string.h>

unsigned char myip[4]={88,80,187,84};
unsigned char netmask[4]={255,255,255,0};
unsigned char mymac[6]={0xf2,0x3c,0x91,0xdb,0xc2,0x98};
unsigned char gateway[4]={88,80,187,1};


//unsigned char targetip[4]={88,80,187,50};
unsigned char targetip[4]={216,58,212,196};
unsigned char targetmac[6];
unsigned char buffer[1500];
int s;
struct sockaddr_ll sll;

int printpacket(unsigned char *b,int l){
int i;
 for(i=0;i<l;i++){
	printf("%.2x(%.3d) ",b[i],b[i]);  
	if(i%4 == 3) printf("\n");
	}
	printf("\n ================\n");
}	
	
struct eth_frame {
unsigned char dst[6];
unsigned char src[6];
unsigned short type;
unsigned char payload[1460];
};

struct arp_packet{
unsigned short hw;
unsigned short proto;
unsigned char hlen;
unsigned char plen;
unsigned short op;
unsigned char srcmac[6];
unsigned char srcip[4];
unsigned char dstmac[6];
unsigned char dstip[4];
};

struct ip_datagram {
unsigned char ver_ihl;
unsigned char tos;
unsigned short len;
unsigned short id;
unsigned short flag_offs;
unsigned char ttl;
unsigned char proto;
unsigned short  checksum;
unsigned int src;
unsigned int dst;
unsigned char payload[1480];
};

unsigned short checksum(char *buf, int size){
int i;
unsigned int tot=0; //32 bits
unsigned short *p ;
p = (unsigned short *)buf;
for(i=0;i<size/2;i++){
	tot = tot + htons(p[i]);
	if(tot&0x10000) tot = ((tot&0xffff) + 1);
	}
return (unsigned short) (0xffff-tot);	
}

int forge_ip(struct ip_datagram *ip, unsigned char * dst, int payloadlen,unsigned char proto) 
{
ip->ver_ihl=0x45;
ip->tos=0;
ip->len=htons(payloadlen+20);
ip->id=htons(0xABCD);
ip->flag_offs=htons(0);
ip->ttl=128;
ip->proto=proto;
ip->checksum=htons(0);
ip->src= *(unsigned int*)myip;
ip->dst= *(unsigned int*)dst;
ip->checksum =htons(checksum((unsigned char *)ip,20));
/* Calculate the checksum!!!*/
};

struct icmp_packet 
{
unsigned char type;
unsigned char code;
unsigned short checksum;
unsigned short id;
unsigned short seq;
unsigned char payload[1400];
};

int forge_icmp(struct icmp_packet * icmp, int payloadsize)
{
int i;
icmp->type=8;
icmp->code=0;
icmp->checksum=htons(0);
icmp->id=htons(0x1234);
icmp->seq=htons(1);
for(i=0;i<payloadsize;i++)icmp->payload[i]=i&0xFF;
icmp->checksum=htons(checksum((unsigned char*)icmp,8 + payloadsize));
} 



int arp_resolve(unsigned char* destip, unsigned char * destmac)
{
int len,n,i;
unsigned char pkt[1500];
struct eth_frame *eth;
struct arp_packet *arp;

eth = (struct eth_frame *) pkt;
arp = (struct arp_packet *) eth->payload; 
for(i=0;i<6;i++) eth->dst[i]=0xff;
for(i=0;i<6;i++) eth->src[i]=mymac[i];
eth->type=htons(0x0806);
arp->hw=htons(1);
arp->proto=htons(0x0800);
arp->hlen=6;
arp->plen=4;
arp->op=htons(1);
for(i=0;i<6;i++) arp->srcmac[i]=mymac[i];
for(i=0;i<4;i++) arp->srcip[i]=myip[i];
for(i=0;i<6;i++) arp->dstmac[i]=0;
for(i=0;i<4;i++) arp->dstip[i]=destip[i];
//printpacket(pkt,14+sizeof(struct arp_packet));
sll.sll_family = AF_PACKET;
sll.sll_ifindex = if_nametoindex("eth0");
len = sizeof(sll);
n=sendto(s,pkt,14+sizeof(struct arp_packet), 0,(struct sockaddr *)&sll,len);
if (n == -1) {perror("Recvfrom failed"); return 0;}
while( 1 ){
	n=recvfrom(s,pkt,1500, 0,(struct sockaddr *)&sll,&len);
	if (n == -1) {perror("Recvfrom failed"); return 0;}
	if (eth->type == htons (0x0806)) //it is ARP
		if(arp->op == htons(2)) // it is a reply
			if(!memcmp(destip,arp->srcip,4)){ // comes from our target
				memcpy(destmac,arp->srcmac,6);
				printpacket(pkt,14+sizeof(struct arp_packet));
				return 0;
				}	
	}
}

unsigned char packet[1500];

int main(){
int i,n,len ;
unsigned char dstmac[6];

struct eth_frame * eth;
struct ip_datagram * ip;
struct icmp_packet * icmp;

s = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)); 
if(s==-1){perror("socket failed");return 1;}

/**** HOST ROUTING ****/
if( (*(unsigned int*)&myip) & (*(unsigned int*)&netmask) == 
    (*(unsigned int*)&targetip) & (*(unsigned int*)&netmask))
	arp_resolve(targetip,dstmac);
else
	arp_resolve(gateway,dstmac);

/********/

printf("destmac: ");printpacket(dstmac,6);

eth = (struct eth_frame *) packet;
ip = (struct ip_datagram *) eth->payload; 
icmp = (struct icmp_packet *) ip->payload;

for(i=0;i<6;i++) eth->dst[i]=dstmac[i];
for(i=0;i<6;i++) eth->src[i]=mymac[i];
eth->type=htons(0x0800);
forge_icmp(icmp, 20);
forge_ip(ip,targetip, 20+8, 1); 
printpacket(packet,14+20+8+20);

for(i=0;i<sizeof(sll);i++) ((char *)&sll)[i]=0;

sll.sll_family=AF_PACKET;
sll.sll_ifindex = if_nametoindex("eth0");
len=sizeof(sll);
n=sendto(s,packet,14+20+8+20, 0,(struct sockaddr *)&sll,len);
if (n == -1) {perror("Recvfrom failed"); return 0;}

while( 1 ){
	len=sizeof(sll);
	n=recvfrom(s,packet,1500, 0,(struct sockaddr *)&sll,&len);
	if (n == -1) {perror("Recvfrom failed"); return 0;}
	if (eth->type == htons (0x0800)) //it is IP
		if(ip->proto == 1) // it is ICMP 
			if(icmp->type==0){
				printpacket(packet,n);	
				break;
				}
}

return 0;

sll.sll_family = AF_PACKET;
sll.sll_ifindex = if_nametoindex("eth0");
len = sizeof(sll);
if (n=recvfrom(s,buffer,1500, 0,(struct sockaddr *)&sll,&len));
if (n == -1) {perror("Recvfrom failed"); return 0;}
printpacket(buffer,n);

}

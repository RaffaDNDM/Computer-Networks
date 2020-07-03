#include <arpa/inet.h>

unsigned short checksum(unsigned char* buf, int size)
{
    int i;
    unsigned int sum=0;
    unsigned short* p = (unsigned short*) buf;

    for(i=0; i<size/2; i++)
    {
        sum += htons(p[i]);

        if(sum&0x10000)
            sum = (sum&0xffff)+1;
    }

    return (unsigned short) ~sum;
}

#include "utility.h"
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>

void print_packet(unsigned char* pkt, int size, char* color)
{
    int i=0;
    int count = ((size%4)==0)? 4: (size%4);


    printf("%s%s%s", color, LINE_32_BITS, DEFAULT);
    for(; i<size; i++)
    {
        printf("%s|%s 0x%02x (%s%03u%s)%s ", color, YELLOW, pkt[i], DEFAULT, pkt[i], YELLOW, DEFAULT);

        if((i%4)==3 || i==(size-1))
        {
            printf("%s|%s\n", color, DEFAULT);

            if(i!=(size-1))
                printf("%s%s%s", color, LINE_32_BITS, DEFAULT);
        }
    }

    for(i=0; i<count; i++)
    {
        printf("%s-------------", color);
    }

    printf("-%s\n\n", DEFAULT);

}

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

void control(int code, char* message)
{
    if(code==-1)
    {
        printf("Errno = %d \n", errno);
        printf("%s \n", message);
        exit(0);
    }
}

#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#define BUFFER_SIZE 1500

void print_packet(unsigned char*buff, int len);

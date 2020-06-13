#ifndef PING
#define PING

#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <netdb.h>
#include <time.h>
#include <float.h>

#include "utility.h"

/**
 * @brief Ping single iteration (it sends ECHO request and wait for reply).
 * @param sd socket descriptor
 * @param id_pkt identifier for the packet
 * @param size_pkt size of the packet
 * @param interface name of the network interface to use
 * @param src addresses of source host
 * @param dst addresses of remote host
 */
int ping_iteration(int sd, int id_pkt, int size_pkts, char* interface, host src, host dst);

/**
 * @brief Compute multiple ECHO requests and replies.
 * @param sd socket descriptor
 * @param num_pkt number of packets to manage
 * @param size_pkt size of the packet
 * @param interface name of the network interface to use
 * @param src addresses of source host
 * @param dst addresses of remote host
 */
void ping(int sd, int num_pkts, int size_pkt, char* interface, host src, host dst);

/**
 * @brief Print info obtained by ECHO reply.
 * @param id identifier of the packet
 * @param ttl time to live from ECHO reply
 * @param size size of the packet
 * @elapsed time of the packet
 */
void print_ping(int id, int ttl, int size, double elapsed_time);

#endif

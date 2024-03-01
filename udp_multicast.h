#ifndef UDP_MULTICAST_H
#define UDP_MULTICAST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>

#define UDP_MULTICAST_ADDR "239.0.0.1"
#define UDP_PORT 6000
#define TCP_PORT 7000
#define MAX_BUF_SIZE 1024

char START_IP[INET_ADDRSTRLEN];
char END_IP[INET_ADDRSTRLEN];
char IP_ADDRESS[INET_ADDRSTRLEN];

void *multicast_udp(void *arg);

#endif /* UDP_MULTICAST_H */

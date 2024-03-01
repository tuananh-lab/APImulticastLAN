#ifndef NETWORK_INFO_H
#define NETWORK_INFO_H

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

void getIPAddressInfo(struct sockaddr_in *sa, const char *subnetmask,char *ip_address);

#endif /* NETWORK_INFO_H */

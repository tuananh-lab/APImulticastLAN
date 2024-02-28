#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

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

void *handle_tcp_connection(void *arg);

#endif /* TCP_CONNECTION_H */

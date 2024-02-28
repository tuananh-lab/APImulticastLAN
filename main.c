#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "network_info.h"
#include "udp_multicast.h"
#include "tcp_connection.h"

#define TCP_PORT 7000

int main() {
    int udp_socket, tcp_socket;
    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = inet_addr("192.168.200.128");

    getIPAddressInfo(&sa, "255.255.255.0");

    printf("Server started.\n");

    if ((udp_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("UDP socket creation failed");
        exit(EXIT_FAILURE);
    }

    if ((tcp_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("TCP socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(TCP_PORT);

    if (bind(tcp_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("TCP bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(tcp_socket, 2) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("TCP server started on port %d\n", TCP_PORT);

    pthread_t udp_thread;
    if (pthread_create(&udp_thread, NULL, multicast_udp, &udp_socket) != 0) {
        perror("Thread creation for UDP broadcast failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_socket;

        if ((client_socket = accept(tcp_socket, (struct sockaddr *)&client_addr, &client_addr_len)) == -1) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        pthread_t tcp_thread;
        if (pthread_create(&tcp_thread, NULL, handle_tcp_connection, &client_socket) != 0) {
            perror("Thread creation for TCP connection handling failed");
            exit(EXIT_FAILURE);
        }
    }

    close(udp_socket);
    close(tcp_socket);

    return 0;
}

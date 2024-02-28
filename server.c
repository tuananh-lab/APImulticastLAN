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

void getIPAddressInfo(struct sockaddr_in *sa, const char *subnetmask) {
    char ipaddress[INET_ADDRSTRLEN];

    // Convert binary IP address to human-readable format
    inet_ntop(AF_INET, &(sa->sin_addr), ipaddress, INET_ADDRSTRLEN);

    // Convert IP address and subnet mask strings to network addresses
    struct in_addr addr, mask;
    inet_aton(ipaddress, &addr);
    inet_aton(subnetmask, &mask);

    // Calculate network address
    struct in_addr network;
    network.s_addr = addr.s_addr & mask.s_addr;

    // Calculate broadcast address
    struct in_addr broadcast;
    broadcast.s_addr = network.s_addr | ~mask.s_addr;

    // Convert network and broadcast addresses to human-readable format
    inet_ntop(AF_INET, &network, START_IP, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &broadcast, END_IP, INET_ADDRSTRLEN);

    // Print information
    printf("IP Address: %s\n", ipaddress);
    printf("Subnet Mask: %s\n", subnetmask);
    printf("IP Range: %s - %s\n", START_IP, END_IP);
}


void *multicast_udp(void *arg) {
    int udp_socket = *((int *)arg);
    struct sockaddr_in multicast_addr;

    memset(&multicast_addr, 0, sizeof(multicast_addr));
    multicast_addr.sin_family = AF_INET;
    multicast_addr.sin_port = htons(UDP_PORT);
    multicast_addr.sin_addr.s_addr = inet_addr(UDP_MULTICAST_ADDR);

    struct in_addr start_ip, end_ip;
    inet_aton(START_IP, &start_ip);
    inet_aton(END_IP, &end_ip);

    struct in_addr ip;
    for (ip = start_ip; ip.s_addr <= end_ip.s_addr; ip.s_addr = htonl(ntohl(ip.s_addr) + 1)) {
        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &ip, ip_str, INET_ADDRSTRLEN);

        char message[MAX_BUF_SIZE];
        snprintf(message, MAX_BUF_SIZE, "Server hello: <%s>  <%d>", ip_str, TCP_PORT);

        sendto(udp_socket, message, strlen(message), 0, (struct sockaddr*)&multicast_addr, sizeof(multicast_addr));
        printf("Multicast message sent to %s: %s\n", ip_str, message);

        sleep(1);
    }
    return NULL;
}

void *handle_tcp_connection(void *arg) {
    int client_socket = *((int *)arg);
    char buffer[MAX_BUF_SIZE];
    ssize_t bytes_received;

    bytes_received = recv(client_socket, buffer, MAX_BUF_SIZE, 0);
    if (bytes_received == -1) {
        perror("Receive failed");
        exit(EXIT_FAILURE);
    } else if (bytes_received == 0) {
        printf("Client disconnected.\n");
        close(client_socket);
        pthread_exit(NULL);
    }

    buffer[bytes_received] = '\0';
    printf("Received from client: %s\n", buffer);

    // Extracting client IP from the received message
    char *token = strtok(buffer, ":");
    token = strtok(NULL, ":");
    printf("Client IP: %s\n", token);

    // Respond to client
    snprintf(buffer, MAX_BUF_SIZE, "Send from client:%s", token);
    send(client_socket, buffer, strlen(buffer), 0);

    close(client_socket);
    pthread_exit(NULL);
}

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
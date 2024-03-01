#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define UDP_PORT 6000
#define TCP_PORT 7000
#define MAX_BUF_SIZE 1024
#define MULTICAST_GROUP "239.0.0.1"

void *handle_tcp_connection(void *arg) {
    int tcp_socket = *((int *)arg);
    char buffer[MAX_BUF_SIZE];
    ssize_t bytes_received;

    bytes_received = recv(tcp_socket, buffer, MAX_BUF_SIZE, 0);
    if (bytes_received == -1) {
        perror("Receive failed");
        exit(EXIT_FAILURE);
    } else if (bytes_received == 0) {
        printf("Server disconnected.\n");
        close(tcp_socket);
        pthread_exit(NULL);
    }

    buffer[bytes_received] = '\0';
    printf("Received from server: %s\n", buffer);
    close(tcp_socket);
    pthread_exit(NULL);
}

int main() {
    int udp_socket, tcp_socket;
    struct sockaddr_in server_addr;
    char buffer[MAX_BUF_SIZE];
    char client_ip[INET_ADDRSTRLEN];

    // Create UDP socket
    if ((udp_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("UDP socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Join multicast group
    struct ip_mreq multicast_req;
    multicast_req.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
    multicast_req.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(udp_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *)&multicast_req, sizeof(multicast_req)) == -1) {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Bind to UDP port
    struct sockaddr_in udp_server_addr;
    memset(&udp_server_addr, 0, sizeof(udp_server_addr));
    udp_server_addr.sin_family = AF_INET;
    udp_server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    udp_server_addr.sin_port = htons(UDP_PORT);
    if (bind(udp_socket, (struct sockaddr *)&udp_server_addr, sizeof(udp_server_addr)) == -1) {
        perror("UDP bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Waiting for multicast message...\n");

    // Receive multicast message
    ssize_t bytes_received;
    struct sockaddr_in sender_addr;
    socklen_t sender_addr_len = sizeof(sender_addr);
    bytes_received = recvfrom(udp_socket, buffer, MAX_BUF_SIZE, 0, (struct sockaddr *)&sender_addr, &sender_addr_len);
    if (bytes_received == -1) {
        perror("Receive failed");
        exit(EXIT_FAILURE);
    }

    buffer[bytes_received] = '\0';
    printf("Received multicast message from server: %s\n", buffer);

    // Extract server IP address and TCP port from the received message
    char *token = strtok(buffer, " ");
    token = strtok(NULL, " "); // Skip "Hello"
    char *ip_address = token;
    token = strtok(NULL, " ");
    int tcp_port = atoi(token);

    printf("Server IP Address: %s\n", ip_address);
    printf("Server TCP Port: %d\n", tcp_port);

    // Create TCP socket
    if ((tcp_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("TCP socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Connect to server via TCP
    struct sockaddr_in tcp_server_addr;
    memset(&tcp_server_addr, 0, sizeof(tcp_server_addr));
    tcp_server_addr.sin_family = AF_INET;
    tcp_server_addr.sin_addr.s_addr = inet_addr(ip_address);
    tcp_server_addr.sin_port = htons(tcp_port);

    if (connect(tcp_socket, (struct sockaddr *)&tcp_server_addr, sizeof(tcp_server_addr)) == -1) {
        perror("TCP connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server via TCP.\n");

    // Get client's IP address
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    getsockname(tcp_socket, (struct sockaddr *)&client_addr, &client_addr_len);
    inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);

    // Send message to server
    snprintf(buffer, MAX_BUF_SIZE, "Send from client: %s", client_ip);
    ssize_t bytes_sent = send(tcp_socket, buffer, strlen(buffer), 0);
    if (bytes_sent == -1) {
        perror("Send failed");
        exit(EXIT_FAILURE);
    }
    printf("Message sent to server via TCP: %s\n", buffer);

    // Create a thread to handle TCP communication
    pthread_t tcp_thread;
    if (pthread_create(&tcp_thread, NULL, handle_tcp_connection, &tcp_socket) != 0) {
        perror("Thread creation for TCP communication failed");
        exit(EXIT_FAILURE);
    }

    pthread_join(tcp_thread, NULL);

    close(udp_socket);
    close(tcp_socket);

    return 0;
}

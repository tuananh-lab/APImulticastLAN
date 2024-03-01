#include "tcp_connection.h"

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


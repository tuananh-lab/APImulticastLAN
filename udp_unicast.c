#include "udp_unicast.h"

void *send_unicast(void *arg) {
    int udp_socket = *((int *)arg);
    struct sockaddr_in client_addr;

    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(UDP_PORT);

    struct in_addr start_ip, end_ip;
    inet_aton(START_IP, &start_ip);
    inet_aton(END_IP, &end_ip);

    struct in_addr ip;
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ip, ip_str, INET_ADDRSTRLEN);

    for (ip = start_ip; ip.s_addr <= end_ip.s_addr; ip.s_addr = htonl(ntohl(ip.s_addr) + 1)) {
        inet_ntop(AF_INET, &ip, ip_str, INET_ADDRSTRLEN);
        char message[MAX_BUF_SIZE];
        snprintf(message, MAX_BUF_SIZE, "Hello %s %d", IP_ADDRESS, TCP_PORT);

        // Set the client IP address
        client_addr.sin_addr.s_addr = ip.s_addr;

        // Send unicast message to the client
        sendto(udp_socket, message, strlen(message), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
        printf("Unicast message sent to %s: %s\n", ip_str, message); 
        sleep(1);
    }
    return NULL;
}
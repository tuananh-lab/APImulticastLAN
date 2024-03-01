#include "udp_multicast.h"

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
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ip, ip_str, INET_ADDRSTRLEN);

    for (ip = start_ip; ip.s_addr <= end_ip.s_addr; ip.s_addr = htonl(ntohl(ip.s_addr) + 1)) {
        inet_ntop(AF_INET, &ip, ip_str, INET_ADDRSTRLEN);
        char message[MAX_BUF_SIZE];
        snprintf(message, MAX_BUF_SIZE, "Hello %s %d", IP_ADDRESS, TCP_PORT);
        sendto(udp_socket, message, strlen(message), 0, (struct sockaddr*)&multicast_addr, sizeof(multicast_addr));
        printf("Multicast message sent to %s: %s\n", ip_str, message);
        sleep(1);
    }
    return NULL;
}
#include "network_info.h"

void getIPAddressInfo(struct sockaddr_in *sa, const char *subnetmask, char *ip_address) {
    char ipaddress[INET_ADDRSTRLEN];

    // Convert binary IP address to human-readable format
    inet_ntop(AF_INET, &(sa->sin_addr), ipaddress, INET_ADDRSTRLEN);

    strcpy(IP_ADDRESS, ipaddress);

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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    // Parse command line arguments
    char *asIP = "127.0.0.1"; // Default AS IP address
    int asPort = 58000; // Default AS port
    if (argc >= 3) {
        asIP = argv[2];
        asPort = atoi(argv[4]);
    }

    // Create UDP socket for communication with AS
    int udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpSocket == -1) {
        perror("Failed to create UDP socket");
        exit(EXIT_FAILURE);
    }

    // Set up AS address
    struct sockaddr_in asAddr;
    memset(&asAddr, 0, sizeof(asAddr));
    asAddr.sin_family = AF_INET;
    asAddr.sin_port = htons(asPort);
    if (inet_pton(AF_INET, asIP, &(asAddr.sin_addr)) <= 0) {
        perror("Invalid AS IP address");
        exit(EXIT_FAILURE);
    }

    // TODO: Implement the User application logic

    // Close UDP socket
    close(udpSocket);

    return 0;
}

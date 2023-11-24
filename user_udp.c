#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#define PORT "58025"

int main(int argc, char* argv[]) {
    int fd, errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char buffer[128];

    char ASIP[];
    char ASport[];

    if (argc == 1) {
        strcpy(ASIP, "localhost");
        strcpy(ASport, PORT);
    }
    else if (argc == 3) {
        if (strcmp(argv[1], "-n") == 0) {
            strcpy(ASIP, argv[2]);
            strcpy(ASport, PORT);
        }
        else if (strcmp(argv[1], "-p") == 0) {
            strcpy(ASIP, "localhost");
            strcpy(ASport, argv[2]);
        }
        else {
            printf("Invalid arguments\n");
            exit(1);
        }
    }
    else if (argc == 5) {
        if (strcmp(argv[1], "-n") == 0 && strcmp(argv[3], "-p") == 0) {
            strcpy(ASIP, argv[2]);
            strcpy(ASport, argv[4]);
        }
        else if (strcmp(argv[1], "-p") == 0 && strcmp(argv[3], "-n") == 0) {
            strcpy(ASIP, argv[4]);
            strcpy(ASport, argv[2]);
        }
        else {
            printf("Invalid arguments\n");
            exit(1);
        }
    }
    else {
        printf("Invalid arguments\n");
        exit(1);
    }
    

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1) exit(1);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP

    errcode = getaddrinfo("localhost", PORT, &hints, &res);
    if(errcode != 0) exit(1);

    n = sendto(fd, "Hello!\n", 7, 0, res->ai_addr, res->ai_addrlen);
    if(n == -1) exit(1);

    addrlen = sizeof(addr);
    n = recvfrom(fd, buffer, 128, 0, (struct sockaddr*)&addr, &addrlen);
    if(n == -1) exit(1);

    write (1, "echo: ", 6);
    write(1, buffer, n);

    freeaddrinfo(res);
    close(fd);
}
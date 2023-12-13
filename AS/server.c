#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#define PORT "58150"
#define TEJO "tejo.tecnico.ulisboa.pt"

int main (int argc, char* argv[]) {
    
    int n;
    char buffer[128], username[7],password[9];
    char function[13], arg1[20], arg2[20], arg3[20], arg4[20];

    char ASIP[20]; // n tenho a certeza se 16 é o suficiente
    char ASport[8];
    int verbose_mode = 0;

    //set username as row of \0
    memset(username, '\0', sizeof(username));
    memset(password, '\0', sizeof(password));
    memset(ASIP, '\0', sizeof(ASIP));
    memset(ASport, '\0', sizeof(ASport));

    //input processing
    if (argc == 1) {
        strcpy(ASport, PORT);
    }
    else if (argc == 2) {
        if (strcmp(argv[1], "-v") == 0) {
            verbose_mode = 1;
        }
        else {
            printf("Invalid arguments\n");
            exit(1);
        }
    }
    else if (argc == 3) {
     00   if (strcmp(argv[1], "-p") == 0) {
            strcpy(ASport, argv[2]);
        }
        else {
            printf("Invalid arguments\n");
            exit(1);
        }
    }
    else if (argc == 4) {
        if (strcmp(argv[1], "-p") == 0 && strcmp(argv[3], "-v") == 0) {
            strcpy(ASport, argv[2]);
            verbose_mode = 1;
        }
        else if (strcmp(argv[1], "-v") == 0 && strcmp(argv[2], "-p") == 0) {
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

}
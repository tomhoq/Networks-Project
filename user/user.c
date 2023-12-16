#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <netdb.h>
#include <ctype.h>

#include <sys/stat.h>
#include <fcntl.h>
#include "udp.c"
#include "tcp.c"


int main (int argc, char* argv[]) {
    
    int n;
    char buffer[128], username[7],password[9];
    char function[13], arg1[20], arg2[20], arg3[20], arg4[20];

    char ASIP[16]; // n tenho a certeza se 16 é o suficiente
    char ASport[6];

    //set username as row of \0
    memset(username, '\0', sizeof(username));
    memset(password, '\0', sizeof(password));
    memset(ASIP, '\0', sizeof(ASIP));
    memset(ASport, '\0', sizeof(ASport));
    
    //input processing
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
            printf("Invalid initialization arguments.\n");
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
            printf("Invalid initialization arguments.\n");
            exit(1);
        }
    }
    else {
        printf("Invalid initialization arguments.\n");
        exit(1);
    }

    printf("%s %s\n", ASIP, ASport);
    //user input
    while (1) {

        memset(function, '\0', sizeof(function));
        memset(arg1, '\0', sizeof(arg1));
        memset(arg2, '\0', sizeof(arg2));
        memset(arg3, '\0', sizeof(arg3));
        memset(arg4, '\0', sizeof(arg4));
        memset(buffer, '\0', sizeof(buffer));

        fgets(buffer, 128, stdin);
        
        n = sscanf(buffer, "%s %s %s %s %s", function, arg1, arg2, arg3, arg4);

        //avoid warning, ignore...
        if (n == 0) {}

        if (strcmp(function, "exit") == 0) {
            exit_program(username);
        }
        else if (strcmp(function, "login") == 0) {
            if (username[0] != '\0') {
                printf("You are already logged in. Stop.\n");
                continue;
            }

            if (login(arg1, arg2, ASIP, ASport) == 1){
                strncpy(username, arg1, 6);
                strncpy(password, arg2, 8);
            }

        }
        else if (strcmp(function, "logout") == 0) {
            if (username[0] == '\0') {
                printf("You are not logged in. Stop.\n");
                continue;
            }
            if (logout(username, password, ASIP, ASport) ==-1){
                printf("Error logging out.\n");
            }
            else {
                memset(username, '\0', sizeof(username));
                memset(password, '\0', sizeof(password));
            }

        }
        else if (strcmp(function, "unregister") == 0) {
            if (username[0] == '\0') {
                printf("You are not logged in. Stop.\n");
                continue;
            }
            if (unregister(username, password, ASIP, ASport) ==-1){
                printf("Error unregistering.\n");
            }
            else {
                memset(username, '\0', sizeof(username));
                memset(password, '\0', sizeof(password));
            }
        }
        else if (strcmp(function, "open") == 0) {
            if (username[0] == '\0') {
                printf("You are not logged in. Stop.\n");
                continue;
            }
            else if (n != 5){
                printf("Open: invalid arguments.\n");
                continue;
            }
            else if (open_(username, password, arg1, arg2, arg3, arg4, ASIP, ASport) == -1) {
                printf("Error opening auction.\n");
                continue;
            }
        }
        else if (strcmp(function, "close") == 0) {
            if (username[0] == '\0') {
                printf("You are not logged in. Stop.\n");
                continue;
            }
            else if (n != 2){
                printf("Close: invalid arguments.\n");
                continue;
            }
            else if (close_(username, password, arg1, ASIP, ASport) == -1) {
                printf("Error closing auction.\n");
                continue;
            }
        }
        else if (strcmp(function, "list") == 0 || strcmp(function,"l") == 0) {
            if(list_(ASIP, ASport) == -1) {
                printf("Error listing auctions.\n");
                continue;
            }
        }
        else if (strcmp(function, "myauctions") == 0 || strcmp(function,"ma") == 0) {
            if (username[0] == '\0') {
                printf("You are not logged in. Stop.\n");
                continue;
            }
            if (myauctions(username, ASIP, ASport) == -1) {
                printf("Error fetching auctions.\n");
                continue;
            }
        }
        else if (strcmp(function, "mybids") == 0 || strcmp(function,"mb") == 0) {
            if (username[0] == '\0') {
                printf("You are not logged in. Stop.\n");
                continue;
            }
            if (mybids(username, ASIP, ASport) == -1) {
                printf("Error fetching bids.\n");
                continue;
            }
        }
        else if (strcmp(function, "show_asset") == 0 || strcmp(function,"sa") == 0) {
            if (n == 2){
                if (show_asset(arg1, ASIP, ASport) == -1) {
                    printf("Error showing asset.\n");
                    continue;
                }
            }
            else
                printf("Show asset: invalid arguments.\n");
        }
        else if (strcmp(function, "bid") == 0 || strcmp(function,"b") == 0) {
            if (username[0] == '\0') {
                printf("You are not logged in. Stop.\n");
                continue;
            }
            if (make_bid(username, password, arg1, arg2, ASIP, ASport)==-1) {
                printf("Error making bid.\n");
                continue;
            }
        }
        else if ((strcmp(function, "show_record") == 0 || strcmp(function,"sr") == 0)) {
            if (n == 2){
                if (show_record(arg1, ASIP, ASport) == -1) {
                    printf("Error showing record of auction.\n");
                    continue;
                }
            }
            else
                printf("You must specify an Auction ID.\n");
        }
        else {
            printf("Invalid command.\n");
        }
    }

    return 0;

}



#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <stdio.h>
#include <netdb.h>

void login(char username[20], char pass[20]) {
        if (strlen(username) != 6) {
            printf("Username must have 6 characters.\n");
            return;
        }
        else {
            for (int i = 0; i < 6; i++) {
                if (!isdigit(username[i])) {
                    printf("Invalid username. Must contain only digits.\n");
                return;
                }
            } 
        }
        if (strlen(pass) != 8) {
            printf("Password must have 8 characters.\n");
            return;
        }
        else {
            for (int i = 0; i < 8; i++) {
                if (!isalnum(pass[i])) {
                    printf("Invalid pass. Must contain only alphanumeric characters.\n");
                    return;

            }
        } 
        }
        //open UDP socket to AS and send LIN UID password
        
    }

int main() {
    char username[6];
    char buffer[128];
    char function[13], arg1[20], arg2[20], arg3[20], arg4[20];
    int n;

    while (1) {
        fgets(buffer, 128, stdin);
        n = sscanf(buffer, "%s %s %s %s %s", function, arg1, arg2, arg3, arg4);
        printf("function: %s\n", function);

        if (strcmp(function, "exit\n") == 0 && n == 1) {
            printf("entered exit\n");
            exit(0);
        }
        else if (strcmp(function, "login") == 0 && n >= 3) {
            login(arg1, arg2);
            strncpy(username, arg1, 6);
            printf("username: %s\n", username);

        }
        else {
            printf("Invalid command\n");
        }
    }
}

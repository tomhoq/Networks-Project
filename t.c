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


void login(char *token) {
    char username[7];
    char password[9];

    token = strtok(NULL, " ");
    printf("token user: %s\n", token);

    if (strlen(token) != 6) {
        if (!strcmp(token + 6, "\n")) {
            printf("Password can't be NULL\n");
            return;
        }
        else {
            printf("Username must have 6 characters.\n");
            return;
        }   
    }
    else {
        for (int i = 0; i < 6; i++) {
            if (!isdigit(token[i])) {
                printf("Invalid username. Must contain only digits.\n");
                return;
            }
        } 
    }
    strcpy(username, token);

    token = strtok(NULL, " ");
    printf("token pass: %s\n", token);
    if (strlen(token) -1 != 8) {
        printf("Password must have 8 characters.\n");
        return;
    }
    else {
        for (int i = 0; i < 8; i++) {
            if (!isalnum(token[i])) {
                printf("Invalid pass. Must contain only digits.\n");
                return;
        }
    } 
    }
    strcpy(password, token);
    printf("username: %s\n", username);
    printf("password: %s\n", password);

    //open UDP socket to AS and send LIN UID password

}

int main() {
    char buffer[128];
    while (1) {
        fgets(buffer, 128, stdin);
        char *token = strtok(buffer, " ");
        if (strcmp(token, "exit\n") == 0) {
            printf("entered exit\n");
            exit(0);
        }
        else if (strcmp(buffer, "login") == 0) {
            login(token);
        }
        else {
            printf("Invalid command\n");
        }
    }
}

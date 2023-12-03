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
#define PORT "58150"
#define TEJO "tejo.tecnico.ulisboa.pt"

#define LOGIN 0
#define LOGOUT 1

void print_all_characters(const char *input_string) {
    while (*input_string != '\0') {
        printf("Character: %c, ASCII: %d\n", *input_string, *input_string);
        input_string++;
    }
}

int login(char username[20], char pass[20], char ASIP[16], char ASport[6]);
int logout(char username[20], char password[20], char ASIP[16], char ASport[6]);
void exit_program(char username[6]);
int communicate_udp(int type, char message[50], char ASIP[16], char ASport[6]);

int main (int argc, char* argv[]) {
    
    int n;
    char buffer[128], username[7],password[9];
    char function[13], arg1[20], arg2[20], arg3[20], arg4[20];

    char ASIP[16]; // n tenho a certeza se 16 é o suficiente
    char ASport[6];

    //set username as row of \0
    memset(username, '\0', sizeof(username));
    memset(password, '\0', sizeof(password));

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
        printf("function: %s, arg1: %s, arg2: %s\n", function, arg1, arg2);

        //ignore just to avoid warning
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
                printf("Error logging out\n");
            }
            else {
                memset(username, '\0', sizeof(username));
                memset(password, '\0', sizeof(password));
            }

        }
        else if (strcmp(function, "unregister\n") == 0) {
            printf("entered unregister");
            //unregister();
        }
        else if (strcmp(function, "open") == 0) {
            printf("entered open");
            //open(token);
        }
        else if (strcmp(function, "close") == 0) {
            printf("entered close");
            //close(token);
        }
        else if (strcmp(function, "list\n") == 0 || strcmp(function,"l\n") == 0) {
            printf("entered list");
            //list();
        }
        else if (strcmp(function, "myauctions\n") == 0 || strcmp(function,"ma\n") == 0) {
            printf("entered myauctions");
            //myauctions();
        }
        else if (strcmp(function, "mybids\n") == 0 || strcmp(function,"mb\n") == 0) {
            printf("entered mybids");
            //mybids();
        }
        else if (strcmp(function, "show_asset") == 0 || strcmp(function,"sa") == 0) {
            printf("entered show_asset");
            //show_asset(token);
        }
        else if (strcmp(buffer, "bid") == 0 || strcmp(buffer,"b") == 0) {
            printf("entered bid");
            //bid(token);
        }
        else if (strcmp(buffer, "show_record") == 0 || strcmp(buffer,"sr") == 0) {
            printf("entered show_record");
            //show_record(token);
        }
        else {
            printf("Invalid command\n");
        }
    }

    return 0;

}

int login(char username[20], char password[20], char ASIP[16], char ASport[6]) {
        if (strlen(username) != 6) {
            printf("Username must have 6 characters.\n");
            return -1;
        }
        else {
            for (int i = 0; i < 6; i++) {
                if (!isdigit(username[i])) {
                    printf("Invalid username. Must contain only digits.\n");
                return -1;
                }
            } 
        }
        if (strlen(password) != 8) {
            printf("Password must have 8 characters.\n");
            return -1;
        }
        else {
            for (int i = 0; i < 8; i++) {
                if (!isalnum(password[i])) {
                    printf("Invalid pass. Must contain only alphanumeric characters.\n");
                    return -1;

                }
            } 
        }


        char message[50]; 

        //IMPORTANTE: Nao esquecer \0, todas as strings tem de ter  um \0 no final
        memset(message, '\0', sizeof(message));
        snprintf(message, sizeof(message), "LIN %s %s\n", username, password);

        //open UDP socket to AS and send LIN UID password;
        communicate_udp(LOGIN, message, ASIP, ASport);
        

        return 1;
}

int logout(char username[20], char password[20], char ASIP[16], char ASport[6]) {

    char message[50]; 

    memset(message, '\0', sizeof(message));

    snprintf(message, sizeof(message), "LOU %s %s\n", username, password);

    //open UDP socket to AS and send LOU UID password;
    communicate_udp(LOGOUT, message, ASIP, ASport);

    return 1;
}

void exit_program(char username[6]) {
    if (username[0] == '\0') {
        printf("Exiting...\n");
        exit(0);
    } else {
        printf("Please log out before exiting.\n");
    }
}

int communicate_udp(int type, char message[50], char ASIP[16], char ASport[6]) {
    int fd, errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char buffer[128];

    memset(buffer, '\0', sizeof(buffer));

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1){
        printf("Error opening socket\n");
        exit(1);
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP
    /*
    printf("ASIP: %s\n", ASIP);
    printf("ASport: %s\n", ASport);
    */
    

    //DEPOIS ALTERAR IP E PORT PARA ASIP E ASport !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    errcode = getaddrinfo(TEJO, "58011", &hints, &res);
    if(errcode != 0) {
        printf("Error getaddrinfo\n");
        exit(1);
    }

    n = sendto(fd, message, 20, 0, res->ai_addr, res->ai_addrlen);
    if(n == -1) {
        printf("Error sending to socket\n");
        exit(1);
    }
    //printf("sent to socket\n");

    addrlen = sizeof(addr);
    n = recvfrom(fd, buffer, 128, 0, (struct sockaddr *) &addr, &addrlen);
    //printf("received from socket\n");
    if(n == -1) {
        printf("Error receviving from socket\n");
        exit(1);
    }

    printf("%s\n", buffer);
    freeaddrinfo(res);
    close(fd);

    switch(type) {
        case LOGIN:
            if (strcmp(buffer, "RLI OK\n") == 0) {
                printf("Login successful\n");
                return 1;
            }
            else if (strcmp(buffer, "RLI REG\n") == 0) {
                printf("Registered user\n");
                return 1;
            }
            else if (strcmp(buffer, "RLI NOK\n") == 0) {
                printf("Login unsuccessful\n");
                return -1;
            }
            else {
                printf("Error receiving answer from AS\n");
                return -1;
            }
            break;
        case LOGOUT:
            if (strcmp(buffer, "RLO OK\n") == 0) {
                printf("Logout successful\n");
                return 1;
            }
            else if (strcmp(buffer, "RLO NOK\n") == 0) {
                printf("Logout unsuccessful\n");
                return -1;
            }
            else if (strcmp(buffer, "RLI UNR\n") == 0) {
                printf("Unregistered user\n");
                return -1;
            }
            else {
                printf("Error receiving answer from AS\n");
                return -1;
            }
        default:
            printf("Invalid type\n");
            return -1;
    }

    return 1;
}
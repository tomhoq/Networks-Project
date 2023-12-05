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
#define UNREGISTER 2
#define LIST 3
#define SHOW_RECORD 4

void print_all_characters(const char *input_string) {
    int i= 1;
    while (*input_string != '\0') {
        printf("Character %d: %c, ASCII: %d\n", i, *input_string, *input_string);
        input_string++;
        i++;
    }
}

int login(char username[20], char pass[20], char ASIP[16], char ASport[6]);
int logout(char username[7], char password[9], char ASIP[16], char ASport[6]);
int unregister(char username[7], char password[9], char ASIP[16], char ASport[6]);
int list_(char ASIP[16], char ASport[6]);
int myauctions(char username[7], char ASIP[16], char ASport[6]);
int mybids(char username[7], char ASIP[16], char ASport[6]);
int show_record(char AID[7], char ASIP[16], char ASport[6]);
void exit_program(char username[6]);
int communicate_udp(int type, char message[25], char ASIP[16], char ASport[6]);

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
        //printf("function: %s, arg1: %s, arg2: %s\n", function, arg1, arg2);

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
        else if (strcmp(function, "unregister") == 0) {
            if (username[0] == '\0') {
                printf("You are not logged in. Stop.\n");
                continue;
            }
            if (unregister(username, password, ASIP, ASport) ==-1){
                printf("Error unregistering\n");
            }
            else {
                memset(username, '\0', sizeof(username));
                memset(password, '\0', sizeof(password));
            }
        }
        else if (strcmp(function, "open") == 0) {
            printf("entered open");
            //open(token);
        }
        else if (strcmp(function, "close") == 0) {
            printf("entered close");
            //close(token);
        }
        else if (strcmp(function, "list") == 0 || strcmp(function,"l") == 0) {
            if(list_(ASIP, ASport) == -1) {
                printf("Error getting list\n");
                continue;
            }
        }
        else if (strcmp(function, "myauctions") == 0 || strcmp(function,"ma") == 0) {
            if (username[0] == '\0') {
                printf("You are not logged in. Stop.\n");
                continue;
            }
            if (myauctions(username, ASIP, ASport) == -1) {
                printf("Error getting auctions\n");
                continue;
            }
        }
        else if (strcmp(function, "mybids") == 0 || strcmp(function,"mb") == 0) {
            if (username[0] == '\0') {
                printf("You are not logged in. Stop.\n");
                continue;
            }
            if (mybids(username, ASIP, ASport) == -1) {
                printf("Error getting auctions\n");
                continue;
            }
        }
        else if (strcmp(function, "show_asset") == 0 || strcmp(function,"sa") == 0) {
            printf("entered show_asset");
            //show_asset(token);
        }
        else if (strcmp(function, "bid") == 0 || strcmp(function,"b") == 0) {
            printf("entered bid");
            //bid(token);
        }
        else if ((strcmp(function, "show_record") == 0 || strcmp(function,"sr") == 0)) {
            if (n == 2){
                if (show_record(arg1, ASIP, ASport) == -1) {
                    printf("Error showing record of auction\n");
                    continue;
                }
            }
            else
                printf("You must specify an Auction ID\n");
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


        char message[25]; 

        //IMPORTANTE: Nao esquecer \0, todas as strings tem de ter  um \0 no final
        memset(message, '\0', sizeof(message));
        snprintf(message, sizeof(message), "LIN %s %s\n", username, password);

        //open UDP socket to AS and send LIN UID password;
        if (communicate_udp(LOGIN, message, ASIP, ASport) == -1) {
            return -1;
        }
    
        return 1;
}

int logout(char username[7], char password[9], char ASIP[16], char ASport[6]) {

    char message[25]; 

    memset(message, '\0', sizeof(message));

    snprintf(message, sizeof(message), "LOU %s %s\n", username, password);

    //open UDP socket to AS and send LOU UID password;
    if (communicate_udp(LOGOUT, message, ASIP, ASport) == -1) {
        return -1;
    }

    return 1;
}

int unregister(char username[7], char password[9], char ASIP[16], char ASport[6]) {

    char message[25]; 

    memset(message, '\0', sizeof(message));

    snprintf(message, sizeof(message), "UNR %s %s\n", username, password);

    //open UDP socket to AS and send LOU UID password;
    if (communicate_udp(UNREGISTER, message, ASIP, ASport) == -1) {
        return -1;
    }

    return 1;
}

int list_(char ASIP[16], char ASport[6]) {

    char message[25]; 

    memset(message, '\0', sizeof(message));

    snprintf(message, sizeof(message), "LST\n");

    //open UDP socket to AS and send LOU UID password;
    if (communicate_udp(LIST, message, ASIP, ASport) == -1) {
        return -1;
    }

    return 1;
}

int myauctions(char username[7], char ASIP[16], char ASport[6]) {

    char message[25]; 

    memset(message, '\0', sizeof(message));

    snprintf(message, sizeof(message), "LMA %s\n", username);

    //open UDP socket to AS and send LOU UID password;
    if (communicate_udp(LIST, message, ASIP, ASport) == -1) {
        return -1;
    }

    return 1;
}

int mybids(char username[7], char ASIP[16], char ASport[6]) {

    char message[25]; 

    memset(message, '\0', sizeof(message));

    snprintf(message, sizeof(message), "LMB %s\n", username);

    //open UDP socket to AS and send RMB UID;
    if (communicate_udp(LIST, message, ASIP, ASport) == -1) {
        return -1;
    }

    return 1;
}

int show_record(char AID[7], char ASIP[16], char ASport[6]) {
    if (strlen(AID) != 3) {
        printf("Auction ID must have 3 characters.\n");
        return -1;
    }
    for (int i = 0; i < 3; i++) {
        if (!isdigit(AID[i])) {
            printf("Invalid AID. Must contain only digits.\n");
            return -1;
        }
    } 

    char message[25]; 

    memset(message, '\0', sizeof(message));

    snprintf(message, sizeof(message), "SRC %s\n", AID);

    //open UDP socket to AS and send SRC AID;
    if (communicate_udp(SHOW_RECORD, message, ASIP, ASport) == -1) {
        return -1;
    }
    
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

int communicate_udp(int type, char message[25], char ASIP[16], char ASport[6]) {
    int fd, errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char *buffer = NULL; 

    if(ASIP == NULL || ASport == NULL) {
        printf("Invalid ASIP or ASport\n");
        return -1;
    }

    buffer = (char *)malloc(128 * sizeof(char));
    if (buffer == NULL) {
        printf("Memory allocation failed\n");
        return -1;
    }
    memset(buffer, '\0', sizeof(buffer));

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1){
        printf("Error opening socket\n");
        return -1;
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
        return -1;
    }

    //print_all_characters(message);
    //printf("%d\n", strlen(message));
    n = sendto(fd, message, strlen(message), 0, res->ai_addr, res->ai_addrlen);
    if(n == -1) {
        printf("Error sending to socket\n");
        return -1;
    }
    printf("sent to socket\n");

    addrlen = sizeof(addr);

    ssize_t totalReceived = 0;
    ssize_t bytesRead;
    ssize_t bufferSize = 128;
    
    char *tmp = (char *)malloc(bufferSize);
    if (tmp == NULL) {
        printf("Memory allocation failed\n");
        return -1;
    }

    while ((bytesRead = recvfrom(fd, tmp + totalReceived, bufferSize, 0, (struct sockaddr *)&addr, &addrlen)) > 0) {
        printf("read bytes\n");
        printf("%d\n", bytesRead);
        printf("%s\n", tmp);
        if (bytesRead < bufferSize) {
            break;
        }
        totalReceived += bytesRead;
        printf("total received: %d\n", totalReceived);
        if (totalReceived >= bufferSize) {
            bufferSize *= 2;  // Double the buffer size
            tmp = (char *)realloc(tmp, bufferSize);
            if (tmp == NULL) {
                printf("Memory reallocation failed\n");
                return -1;
            }
        }
        
        printf("%s\n", tmp);
    }
    printf("EXIT: %d\n", bytesRead);

    buffer = (char *)malloc(totalReceived + 1); // +1 for null terminator

    if (buffer == NULL) {
        printf("Memory allocation failed for buffer\n");
        free(tmp);
        return -1;
    }

    // Copy the content of tmp to buffer
    memcpy(buffer, tmp, totalReceived);
    buffer[totalReceived] = '\0'; // Null-terminate the buffer

    free(tmp);

    printf("AAAAAAAAAAAA\n");
    printf("AAAAAAAAAAAA %s\n", buffer);


    if (bytesRead == -1) {
        printf("Error receiving from socket\n");
        free(buffer);
        return -1;
    }

    freeaddrinfo(res);
    close(fd);


    char arg1[4], arg2[4];
    char AID[4], activ[2];
    char *arg3 = NULL; 

    switch(type) {
        case LOGIN:
            printf("entered login\n");
            if (strcmp(buffer, "RLI OK\n") == 0) {
                printf("Login successful\n");
                free(buffer);
                return 1;
            }
            else if (strcmp(buffer, "RLI REG\n") == 0) {
                printf("Registered user\n");
                free(buffer);
                return 1;
            }
            else if (strcmp(buffer, "RLI NOK\n") == 0) {
                printf("Login unsuccessful\n");
                free(buffer);
                return -1;
            }
            else {
                printf("Error receiving answer from AS\n");
                free(buffer);
                return -1;
            }
            break;
        case LOGOUT:
            if (strcmp(buffer, "RLO OK\n") == 0) {
                printf("Logout successful\n");
                free(buffer);
                return 1;
            }
            else if (strcmp(buffer, "RLO NOK\n") == 0) {
                printf("Logout unsuccessful. You are not logged in.\n");
                free(buffer);
                return -1;
            }
            else if (strcmp(buffer, "RLI UNR\n") == 0) {
                printf("Unregistered user\n");
                free(buffer);
                return -1;
            }
            else {
                printf("Error receiving answer from AS\n");
                free(buffer);
                return -1;
            }
        case UNREGISTER:
            if (strcmp(buffer, "RUR OK\n") == 0) {
                printf("Unregister successful\n");
                free(buffer);
                return 1;
            }
            else if (strcmp(buffer, "RUR NOK\n") == 0) {
                printf("Unregister unsuccessful. You are not logged in.\n");
                free(buffer);
                return -1;
            }
            else if (strcmp(buffer, "RUR UNR\n") == 0) {
                printf("Unregister unsuccessful. You are not registered.\n");
                free(buffer);
                return -1;
            }
            else {
                printf("Error receiving answer from AS\n");
                free(buffer);
                return -1;
            }
        case LIST:    // This case attends both LST and LMA commands!
            arg3 = (char *)malloc((strlen(buffer)-5)*sizeof(char));

            int n = sscanf(buffer, "%s %s %[^\n]", arg1, arg2, arg3);  // RLS OK <list> or RMA OK <list>

            //printing lists for LST and LMA
            if ((strcmp(arg1, "RLS") == 0 && strcmp(arg2, "OK") == 0) || (    // list
             strcmp(arg1, "RMA") == 0 && strcmp(arg2, "OK") == 0) ||   (      // myauctions
             strcmp(arg1, "RMB") == 0 && strcmp(arg2, "OK") == 0))            // mybids
            {
                if (n != 3) {
                    printf("Reading list failed\n");
                    free(arg3);  // Free the original memory before exiting
                    free(buffer);
                    return -1;
                }
                if (arg3 == NULL) {
                    // Handle realloc failure
                    printf("Memory reallocation failed for list\n");
                    free(arg3);  // Free the original memory before exiting
                    free(buffer);
                    return -1;    // Return an error code
                } else {
                    //printf("%s, %d\n", arg3, strlen(arg3));
                    char *answer = (char *)malloc(strlen(arg3)*sizeof(char));
                    char toAdd[30];
                    int n;
                    memset(toAdd, '\0', sizeof(toAdd));

                    //in this loop im storing all the auctions in the variable answer
                    while((n = sscanf(arg3, "%s %s %[^\n]", AID, activ, arg3)) == 3) { 
                        answer = (char *)realloc(answer, strlen(answer) + strlen("Auction number 111: inactive\n") + 1);

                        snprintf(toAdd, strlen("Auction number 111: inactive\n") + 1, "Auction number %s: %s\n", AID, (activ[0] == '1' ? "active" : "inactive"));
                        
                        strcat(answer, toAdd);
                        memset(toAdd, '\0', sizeof(toAdd));
                    }

                    //last time 
                    if (n != 0 ){
                        printf("n: %d\n", n);
                        answer = (char *)realloc(answer, strlen(answer) + strlen("Auction number 111: inactive\n") + 1);
                        snprintf(toAdd, strlen("Auction number 111: inactive\n") + 1, "Auction number %s: %s\n", AID, (activ[0] == '1' ? "active" : "inactive"));
                        strcat(answer, toAdd);
                    }

                    printf("%s", answer);

                    free(answer);
                    free(arg3);
                    free(buffer);
                    return 1;
                }
                
            }
            else if (strcmp(buffer, "RLS NOK\n") == 0) {
                printf("List unsuccessful. No active auctions.\n");
                free(buffer);
                return -1;
            }
            else if (strcmp(buffer, "RMA NOK\n") == 0) {
                printf("My auctions unsuccessful. No active auctions.\n");
                free(buffer);
                return -1;
            }
            else if (strcmp(buffer, "RMB NOK\n") == 0) {
                printf("My bids unsuccessful. No ongoing bids.\n");
                free(buffer);
                return -1;
            }
            else if (strcmp(buffer, "RMA NLG\n") == 0) {
                printf("My auctions unsuccessful. You are not logged in.\n");
                free(buffer);
                return -1;
            }
            else if (strcmp(buffer, "RMB NLG\n") == 0) {
                printf("My bids unsuccessful. You are not logged in.\n");
                free(buffer);
                return -1;
            }
            else {
                printf("Error receiving answer from AS\n");
                free(buffer);
                return -1;
            }
        default:
            printf("Invalid type\n");
            free(buffer);
            return -1;
    }

    return 1;
}
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



void login(char *token);
void exit_program();

int main (int argc, char* argv[]) {
    
    int fd, errcode, n;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char buffer[128];

    char ASIP[16]; // n tenho a certeza se 16 é o suficiente
    char ASport[6];

    char userId[7];

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

    while (1) {
        fgets(buffer, 128, stdin);
        char *token = strtok(buffer, " ");

        if (strcmp(token, "exit\n") == 0) {
            printf("entered exit");
            exit_program(userId);
            exit(0);
        }
        else if (strcmp(buffer, "login") == 0) {
            printf("entered login");
            login(token);
            break;
        }
        else if (strcmp(buffer, "logout") == 0) {
            printf("entered logout");
            logout();
            break;
        }
        else if (strcmp(buffer, "unregister\n") == 0) {
            printf("entered unregister");
            unregister();
            break;
        }
        else if (strcmp(buffer, "open") == 0) {
            printf("entered open");
            open(token);
            break;
        }
        else if (strcmp(buffer, "close") == 0) {
            printf("entered close");
            close(token);
            break;
        }
        else if (strcmp(buffer, "list\n") == 0 || strcmp(buffer,"l\n") == 0) {
            printf("entered list");
            list();
        }
        else if (strcmp(buffer, "myauctions\n") == 0 || strcmp(buffer,"ma\n") == 0) {
            printf("entered myauctions");
            myauctions();
            break;
        }
        else if (strcmp(buffer, "mybids\n") == 0 || strcmp(buffer,"mb\n") == 0) {
            printf("entered mybids");
            mybids();
            break;
        }
        else if (strcmp(buffer, "show_asset") == 0 || strcmp(buffer,"sa") == 0) {
            printf("entered show_asset");
            show_asset(token);
            break;
        }
        else if (strcmp(buffer, "bid") == 0 || strcmp(buffer,"b") == 0) {
            printf("entered bid");
            bid(token);
            break;
        }
        else if (strcmp(buffer, "show_record") == 0 || strcmp(buffer,"sr") == 0) {
            printf("entered show_record");
            show_record(token);
            break;
        }
        else {
            printf("Invalid command\n");
        }
    }

    


    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1) exit(1);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP

    errcode = getaddrinfo(TEJO, PORT, &hints, &res);
    if(errcode != 0) exit(1);

    n = connect(fd, res->ai_addr, res->ai_addrlen);
    if(n == -1) exit(1);

    n = write(fd, "Hello!\n", 7);
    if(n == -1) exit(1);

    n = read(fd, buffer, 128);
    if(n == -1) exit(1);

    write (1, "echo: ", 6);
    write(1, buffer, n);

    freeaddrinfo(res);
    close(fd);
}

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

void exit_program(char userId[7]) {
    if (userId != NULL) {
        exit(0);
    }
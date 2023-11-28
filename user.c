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


int login(char username[20], char pass[20]);
void exit_program();
int communicate_udp(int type, char arg1[20], char arg2[20]);

int main (int argc, char* argv[]) {
    
    int n;
    char buffer[128], username[6];
    char function[13], arg1[20], arg2[20], arg3[20], arg4[20];
    int n, i;

    char ASIP[16]; // n tenho a certeza se 16 é o suficiente
    char ASport[6];


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
        n = sscanf(buffer, "%s %s %s %s %s", function, arg1, arg2, arg3, arg4);
        printf("function: %s\n", function);

        if (strcmp(function, "exit\n") == 0) {
            printf("entered exit");
            exit_program(username);
            exit(0);
        }
        else if (strcmp(function, "login") == 0) {
            printf("entered login");
            i = login(arg1, arg2);
            if (i==1)
                strncpy(username, arg1, 6);
                
            break;
        }
        else if (strcmp(function, "logout") == 0) {
            printf("entered logout");
            logout();
            break;
        }
        else if (strcmp(function, "unregister\n") == 0) {
            printf("entered unregister");
            unregister();
            break;
        }
        else if (strcmp(function, "open") == 0) {
            printf("entered open");
            open(token);
            break;
        }
        else if (strcmp(function, "close") == 0) {
            printf("entered close");
            close(token);
            break;
        }
        else if (strcmp(function, "list\n") == 0 || strcmp(function,"l\n") == 0) {
            printf("entered list");
            list();
        }
        else if (strcmp(function, "myauctions\n") == 0 || strcmp(function,"ma\n") == 0) {
            printf("entered myauctions");
            myauctions();
            break;
        }
        else if (strcmp(function, "mybids\n") == 0 || strcmp(function,"mb\n") == 0) {
            printf("entered mybids");
            mybids();
            break;
        }
        else if (strcmp(function, "show_asset") == 0 || strcmp(function,"sa") == 0) {
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

    



}

int login(char username[20], char pass[20]) {
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
        if (strlen(pass) != 8) {
            printf("Password must have 8 characters.\n");
            return -1;
        }
        else {
            for (int i = 0; i < 8; i++) {
                if (!isalnum(pass[i])) {
                    printf("Invalid pass. Must contain only alphanumeric characters.\n");
                    return -1;

                }
            } 
        }
        //open UDP socket to AS and send LIN UID password;
        communicate_udp(LOGIN, username, pass);

        return 1;
}

void exit_program(char username[7]) {
    if (username != NULL) {
        exit(0);
    } else {
        printf("Please log out before exiting.\n");
    }
}

int communicate_udp(int type, char arg1[20], char arg2[20]){
    int fd, errcode, n;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char buffer[128];
    fd = socket(AF_INET, SOCK_STREAM, 0);
    printf("openincg socket UDP\n");
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
    return 1
}
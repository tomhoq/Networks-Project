#include "udp.h"

void print_all_characters(const char *input_string) {
    int i= 1;
    while (*input_string != '\0') {
        printf("Character %d: %c, ASCII: %d\n", i, *input_string, *input_string);
        input_string++;
        i++;
    }
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
    char buffer[6000];  //este valor não vai funcionar no futuro, para o comando list o server pode 
    //devolver ate 6000 bytes, e nao faz sentido estarmos a definir um buffer[6000] pelo que vamos ter 
    //ler do recvfrom em loop ate nao haver mais nada para ler, vamos ter q usar memoria dinamica 

    if(ASIP == NULL || ASport == NULL) {
        printf("Invalid ASIP or ASport\n");
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
    n = sendto(fd, message, strlen(message), 0, res->ai_addr, res->ai_addrlen);
    if(n == -1) {
        printf("Error sending to socket\n");
        return -1;
    }
    //printf("sent to socket\n");

    addrlen = sizeof(addr);
    n = recvfrom(fd, buffer, 5999, 0, (struct sockaddr *) &addr, &addrlen);

    //printf("buffer: %s\n", buffer);
    //printf("n: %d\n", n);
    
    //printf("received from socket\n");
    if(n == -1) {
        printf("Error receviving from socket\n");
        return -1;
    }

    printf("%s\n", buffer);
    freeaddrinfo(res);
    close(fd);

    char *arg3 = NULL; 
    char AID[4];

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
                printf("Logout unsuccessful. You are not logged in.\n");
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
        case UNREGISTER:
            if (strcmp(buffer, "RUR OK\n") == 0) {
                printf("Unregister successful\n");
                return 1;
            }
            else if (strcmp(buffer, "RUR NOK\n") == 0) {
                printf("Unregister unsuccessful. You are not logged in.\n");
                return -1;
            }
            else if (strcmp(buffer, "RUR UNR\n") == 0) {
                printf("Unregister unsuccessful. You are not registered.\n");
                return -1;
            }
            else {
                printf("Error receiving answer from AS\n");
                return -1;
            }
        case LIST:    // This case attends both LST and LMA commands!
            arg3 = (char *)malloc((strlen(buffer)-5)*sizeof(char));
            char arg1[4], arg2[4];
            char activ[2];
            n = sscanf(buffer, "%s %s %[^\n]", arg1, arg2, arg3);  // RLS OK <list> or RMA OK <list>

            //printing lists for LST and LMA
            if ((strcmp(arg1, "RLS") == 0 && strcmp(arg2, "OK") == 0) || (    // list
             strcmp(arg1, "RMA") == 0 && strcmp(arg2, "OK") == 0) ||   (      // myauctions
             strcmp(arg1, "RMB") == 0 && strcmp(arg2, "OK") == 0))            // mybids
            {
                if (n != 3) {
                    printf("Reading list failed\n");
                    free(arg3);  // Free the original memory before exiting
                    return -1;
                }
                if (arg3 == NULL) {
                    // Handle realloc failure
                    printf("Memory reallocation failed for list\n");
                    free(arg3);  // Free the original memory before exiting
                    return -1;    // Return an error code
                } else {
                    //printf("%s, %d\n", arg3, strlen(arg3));

                    //in this loop im storing all the auctions in the variable answer
                    while((n = sscanf(arg3, "%s %s %[^\n]", AID, activ, arg3)) == 3) { 
                        printf("Auction %s: %s\n", AID, (activ[0] == '1' ? "Active" : "Inactive"));
                    }

                    //last time 
                    if (n != 0 ){
                        printf("Auction %s: %s\n", AID, (activ[0] == '1' ? "Active" : "Inactive"));
                    }

                    free(arg3);
                    return 1;
                }
                
            }
            else if (strcmp(buffer, "RLS NOK\n") == 0) {
                printf("List unsuccessful. No active auctions.\n");
                return -1;
            }
            else if (strcmp(buffer, "RMA NOK\n") == 0) {
                printf("My auctions unsuccessful. No active auctions.\n");
                return -1;
            }
            else if (strcmp(buffer, "RMB NOK\n") == 0) {
                printf("My bids unsuccessful. No ongoing bids.\n");
                return -1;
            }
            else if (strcmp(buffer, "RMA NLG\n") == 0) {
                printf("My auctions unsuccessful. You are not logged in.\n");
                return -1;
            }
            else if (strcmp(buffer, "RMB NLG\n") == 0) {
                printf("My bids unsuccessful. You are not logged in.\n");
                return -1;
            }
            else {
                printf("Error receiving answer from AS\n");
                return -1;
            }
        case SHOW_RECORD:
            arg3 = (char *)malloc((strlen(buffer)-5)*sizeof(char));
            
            memset(AID, '\0', sizeof(AID));

            sscanf(message, "SRC %s\n", AID);

            n = sscanf(buffer, "%s %s %[^\n]", arg1, arg2, arg3);  // RLS OK <list> or RMA OK <list>
            //printf("arg3: %s\n", arg3);
            //printing lists for LST and LMA
            if (strcmp(arg1, "RRC") == 0 && strcmp(arg2, "OK") == 0){
                if (n != 3) {
                    printf("Reading record failed\n");
                    free(arg3);  // Free the original memory before exiting
                    return -1;
                }
                if (arg3 == NULL) {
                    // Handle realloc failure
                    printf("Memory reallocation failed for record\n");
                    free(arg3);  // Free the original memory before exiting
                    return -1;    // Return an error code
                } else {
                    //printf("%s, %d\n", arg3, strlen(arg3));
                    char host_UID[20], auction_name[20], asset_name[20], start_value[20], start_date[20], start_time[20], duration[20];
                    memset(host_UID, '\0', sizeof(host_UID));
                    memset(auction_name, '\0', sizeof(auction_name));
                    memset(asset_name, '\0', sizeof(asset_name));
                    memset(start_value, '\0', sizeof(start_value));
                    memset(start_date, '\0', sizeof(start_date));
                    memset(start_time, '\0', sizeof(start_time));
                    memset(duration, '\0', sizeof(duration));

                    //printf("arg3: %s\n", arg3);
                    n = sscanf(arg3, "%s %s %s %s %s %s %s %[^\n]", host_UID, auction_name, asset_name, start_value, start_date, start_time, duration, arg3);
                    printf("arg3 after start:\n%s\n", arg3);


                    printf("-------------------------- Auction nº %s --------------------------\n", AID);
                    printf("UID: %s \nAuction name: %s \nAsset name: %s \nStart value: %s \nStart date: %s-%s \nTime active: %s\n",
                    host_UID, auction_name, asset_name, start_value, start_date, start_time, duration);

                    char letter[2], bidder_UID[20], bid_value[20], bid_date[20], bid_time[20], time_until_bid[20], end_date[20], end_time[20], time_first_bid[20];
                    int j;
                    if (arg3[0] == 'B'){
                        printf("        ------------------------------------------------\n");
                        while(1) { 
                            memset(letter, '\0', sizeof(letter));
                            memset(bidder_UID, '\0', sizeof(bidder_UID));
                            memset(bid_value, '\0', sizeof(bid_value));
                            memset(bid_date, '\0', sizeof(bid_date));
                            memset(bid_time, '\0', sizeof(bid_time));
                            memset(time_until_bid, '\0', sizeof(time_until_bid));
                            j = sscanf(arg3, "%s %s %s %s %s %s %[^\n]", letter, bidder_UID, bid_value, bid_date, bid_time, time_until_bid, arg3);
                            if (letter[0] == 'E')
                                break;

                            printf("Bidder ID: %s\nBid value: %s \tBid date: %s-%s \tTime until bid: %s\n", 
                            bidder_UID, bid_value, bid_date, bid_time, time_until_bid);
                            if (j == 6) {
                                sscanf(arg3, "%s %s %s %s", letter, bidder_UID, bid_value, bid_date);
                                break;
                            }
                        }
                        //printf("letter: %s\n", letter);
                        if (letter[0] == 'E') {
                            printf("        ------------------------------------------------\nAuction finished at: %s-%s\nTime until first bid: %s\n",
                            bidder_UID, bid_value, bid_date);     //THESE VARIABLES ARE RIGHT EVEN IF IT DOENS'T LOOK LIKE IT   
                        }
                    }
                    else if (n == 8 && arg3[0] == 'E'){
                        //process E
                        //printf("finish no bid:\n %s\n", arg3);
                        memset(letter, '\0', sizeof(letter));
                        memset(end_date, '\0', sizeof(end_date));
                        memset(end_time, '\0', sizeof(end_time));
                        memset(time_first_bid, '\0', sizeof(time_first_bid));

                        sscanf(arg3, "%s %s %s %s", letter, end_date, end_time, time_first_bid);
                        printf("        ------------------------------------------------\nAuction finished at: %s-%s\nTime until first bid: %s\n",
                        end_date, end_time, time_first_bid);     //THESE VARIABLES ARE RIGHT EVEN IF IT DOENS'T LOOK LIKE IT   
                    }

                    printf("--------------------------------------------------------------------\n");

                    free(arg3);
                    return 1;
                }
            }
            if (strcmp(buffer, "RRC OK\n") == 0) {
                printf("Show record unsuccessful. No such auction.\n");
                return 1;
            }
            else if (strcmp(buffer, "RRC NOK\n") == 0) {
                printf("Show record unsuccessful. No such auction.\n");
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
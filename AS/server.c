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
#include <math.h>
#include "db.c"
#define PORT "58025"
#define TEJO "tejo.tecnico.ulisboa.pt"


int only_numbers(char *str) {
    int i = 0;
    while (str[i] != '\0') {
        if (str[i] < '0' || str[i] > '9')
            return 0;
        i++;
    }
    return 1;
}

int only_alphanumerical(char *str) {
    int i = 0;
    while (str[i] != '\0') {
        if ((str[i] < '0' || str[i] > '9') && (str[i] < 'A' || str[i] > 'Z') && (str[i] < 'a' || str[i] > 'z'))
            return 0;
        i++;
    }
    return 1;
}

int only_alphanumerical_and_extensions(char *str) {
    int i = 0;
    while (str[i] != '\0') {
        if ((str[i] < '0' || str[i] > '9') && (str[i] < 'A' || str[i] > 'Z') && (str[i] < 'a' || str[i] > 'z') && str[i] != '.' && str[i] != '-' && str[i] != '_')
            return 0;
        i++;
    }
    return 1;
}

int main (int argc, char* argv[]) {

    // Socket variables
    struct addrinfo hints_udp, *res_udp, *res_tcp, hints_tcp;
    struct sockaddr_in udp_useraddr, tcp_useraddr;
    socklen_t addrlen;
    int i, out_fds, n, errcode, ret, ufd, tcp, wr;
    
    char buffer[128], username[7],password[9];
    char function[13], arg1[20], arg2[20], arg3[20], arg4[20];

    char in_str[128];

    fd_set inputs, testfds;
    struct timeval timeout;

    char prt_str[128];

    char host[NI_MAXHOST], service[NI_MAXSERV];

    char ASIP[20];
    char ASport[8];
    int verbose_mode = 0;

    memset(username, '\0', sizeof(username));
    memset(password, '\0', sizeof(password));
    memset(ASIP, '\0', sizeof(ASIP));
    memset(ASport, '\0', sizeof(ASport));

    // Input processing
    if (argc == 1) {
        strcpy(ASport, PORT);
    }
    else if (argc == 2) {
        if (strcmp(argv[1], "-v") == 0) {
            verbose_mode = 1;
            strcpy(ASport, PORT);
        }
        else {
            printf("Invalid initialization arguments.\n");
            exit(1);
        }
    }
    else if (argc == 3) {
        if (strcmp(argv[1], "-p") == 0) {
            strcpy(ASport, argv[2]);
        }
        else {
            printf("Invalid initialization arguments.\n");
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
            verbose_mode = 1;
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

    // SERVER SECTION
    memset(&hints_udp,0,sizeof(hints_udp));
    memset(&hints_tcp,0,sizeof(hints_tcp));
    hints_udp.ai_family = AF_INET;
    hints_udp.ai_socktype = SOCK_DGRAM;
    hints_udp.ai_flags = AI_PASSIVE | AI_NUMERICSERV;
    hints_tcp.ai_family = AF_INET;
    hints_tcp.ai_socktype = SOCK_STREAM;
    hints_tcp.ai_flags = AI_PASSIVE | AI_NUMERICSERV;


    if((errcode=getaddrinfo(NULL,ASport,&hints_udp,&res_udp))!=0)
        exit(1); // On error

    if((errcode=getaddrinfo(NULL,ASport,&hints_tcp,&res_tcp))!=0)
        exit(1); // On error

    ufd=socket(res_udp->ai_family,res_udp->ai_socktype,res_udp->ai_protocol);
    if(ufd==-1)
        exit(1);

    tcp=socket(res_tcp->ai_family,res_tcp->ai_socktype,res_tcp->ai_protocol);
    if(tcp==-1)
        exit(1);

    if(bind(ufd,res_udp->ai_addr,res_udp->ai_addrlen)==-1)
    {
        perror("Bind error UDP server.\n");
        exit(1); // On error
    }

    if(bind(tcp,res_tcp->ai_addr,res_tcp->ai_addrlen)==-1)
    {
        perror("Bind error TCP server.\n");
        exit(1); // On error
    }
    
    // Listen on the TCP socket
    if (listen(tcp, 5) == -1) {
        perror("TCP socket listen failed.");
        exit(EXIT_FAILURE);
    }

    if(res_udp!=NULL)
        freeaddrinfo(res_udp);
    if(res_tcp!=NULL)
        freeaddrinfo(res_tcp);

    FD_ZERO(&inputs); // Clear input mask
    FD_SET(tcp,&inputs); // Set standard input channel on
    FD_SET(ufd,&inputs); // Set UDP channel on

    while(1)
    {
        testfds=inputs; // Reload mask
        memset((void *)&timeout,0,sizeof(timeout));
        timeout.tv_sec=10;

        out_fds=select(FD_SETSIZE,&testfds,(fd_set *)NULL,(fd_set *)NULL,(struct timeval *) &timeout);
        // testfds is now '1' at the positions that were activated

        memset(prt_str, '\0', sizeof(prt_str));

        switch(out_fds)
        {
            case 0:
                break;
            case -1:
                perror("select");
                exit(1);
            default:

                // TCP Socket
                if(FD_ISSET(tcp,&testfds))
                {
                    int sock = accept(tcp, NULL, NULL);
                    if (sock == -1) {
                        perror("TCP socket accept error.\n");

                    } else {
                        memset(prt_str, '\0', sizeof(prt_str));
                        addrlen = sizeof(tcp_useraddr);
                        ret = read(sock, prt_str, 127);
                        
                        if (ret>=0) {

                            errcode=getnameinfo( (struct sockaddr *) &tcp_useraddr,
                                    addrlen,host,sizeof host, service,sizeof service,0);

                            char code[5], arg1[30], arg2[30], arg3[30], arg4[30], arg5[30], arg6[30], arg7[30], arg8[128];
                            char answer[128];
                            memset(answer, '\0', sizeof(answer));
                            memset(code, '\0', sizeof(code));
                            memset(arg1, '\0', sizeof(arg1));
                            memset(arg2, '\0', sizeof(arg2));
                            memset(arg3, '\0', sizeof(arg3));
                            memset(arg4, '\0', sizeof(arg4));
                            memset(arg5, '\0', sizeof(arg5));
                            memset(arg6, '\0', sizeof(arg6));
                            memset(arg7, '\0', sizeof(arg7));
                            memset(arg8, '\0', sizeof(arg8));

                            n = sscanf(prt_str, "%s %s %s %s %s %s %s %s %[^\n]", code, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
                            int size_arg8 = 127 -(strlen(code) + strlen(arg1) + strlen(arg2) + strlen(arg3) + strlen(arg4) + strlen(arg5) + strlen(arg6) + strlen(arg7) + 8);
                            int k, i = 0;

                            if (!strcmp(code, "OPA")) {
                                if (!only_numbers(arg1) || strlen(arg1) != 6) {
                                    strcpy(answer, "ROA ERR\n");
                                }
                                else if (!only_alphanumerical(arg2) || strlen(arg2) != 8) {
                                    strcpy(answer, "ROA ERR\n");
                                }
                                else if (!only_alphanumerical(arg3) || strlen(arg3) > 10) {
                                    strcpy(answer, "ROA ERR\n");
                                }
                                else if (!only_numbers(arg4) || strlen(arg4) > 6) {
                                    strcpy(answer, "ROA ERR\n");
                                }
                                else if (!only_numbers(arg5) || strlen(arg5) > 5) {
                                    strcpy(answer, "ROA ERR\n");
                                }
                                else if (!only_alphanumerical_and_extensions(arg6) || strlen(arg6) > 24) {
                                    strcpy(answer, "ROA ERR\n");
                                }
                                else if (!only_numbers(arg7) || strlen(arg7) > 8) {
                                    strcpy(answer, "ROA ERR\n");
                                }
                                else if (n != 9 || (ret < 127 && prt_str[strlen(prt_str)-1] != '\n')) {
                                    strcpy(answer, "ROA ERR\n");
                                } 
                                else {
                                    k = create_auction(arg1, arg2, arg3, arg4, arg5, arg6);
                                    strcpy(answer, "ROA");
                                    if (k == OK) {    
                                        char open_aid[5];
                                        if (get_most_recent_aid(open_aid) == -1){
                                            printf("Error getting most recent aid.\n");
                                            delete_auction(open_aid, arg1);
                                            strcat(answer, " NOK");

                                        } else {
                                            char *path = get_auction_path(open_aid);
                                            strcat(path, arg6);

                                            FILE *fp = fopen(path, "w+");
                                            if (fp == NULL) {
                                                printf("Error opening file.\n");
                                                strcpy(answer, "ROA ERR\n");
                                            }
                                            else {
                                                int bytes_to_read = atoi(arg7);

                                                int bytes_read = 0; // MAX 10 MB
                                                int stop = 0;

                                                wr = fwrite(arg8, 1, size_arg8, fp);
                                                bytes_read += wr;

                                                if (bytes_read < bytes_to_read) {
                                                    memset(prt_str, '\0', sizeof(prt_str));
                                                    
                                                    while (1) {
                                                        ret = read(sock, prt_str, 127);
                                                        if (ret < 0) {
                                                            stop = 1;
                                                            printf("Error reading from socket.\n");
                                                            break;
                                                        }

                                                        // Write the received data to the file
                                                        wr = fwrite(prt_str, 1, ret, fp);
                                                        if (wr != ret) {
                                                            printf("Error writing to file.\n");
                                                            stop = 1;
                                                            break;
                                                        }
                                                        fflush(fp);  // Flush the file stream to ensure data is written immediately

                                                        memset(prt_str, '\0', sizeof(prt_str));
                                                        bytes_read += ret;

                                                        if (bytes_read == bytes_to_read) {
                                                            memset(prt_str, '\0', sizeof(prt_str));
                                                            break;
                                                        } else if (bytes_read > bytes_to_read) {
                                                            printf("File size is surpassing the one provided:\n %d > %d\n", bytes_read, bytes_to_read);
                                                            stop = 1;
                                                            break;
                                                        }
                                                        if (bytes_read >= pow(10, 7)) {
                                                            printf("File size cannot exceed 10MB.\n");
                                                            stop = 1;
                                                            break;
                                                        }
                                                    }
                                                }
                                                else if (bytes_read > bytes_to_read) {
                                                    stop = 1;
                                                }
                                                if (fclose(fp) != 0) {
                                                    printf("Error closing file.\n");
                                                    memset(answer, '\0', sizeof(answer));
                                                    strcpy(answer, "ROA NOK\n");
                                                    delete_auction(open_aid, arg1);
                                                }
                                                else {
                                                    if (verbose_mode && !stop) {
                                                        printf("Auction created.\n");
                                                    }
                                                    strcat(answer, " OK");

                                                    sprintf(answer, "%s %s\n", answer, open_aid);
                                                }
                                                if (stop == 1) {
                                                    delete_auction(open_aid, arg1);
                                                    memset(answer, '\0', sizeof(answer));
                                                    strcpy(answer, "ROA NOK\n");
                                                }
                                            }
                                            free(path);  
                                        }            
                                    }
                                    else if (k == NLG) {
                                        strcat(answer, " NLG\n");
                                    }
                                    else if (k == NOK) {
                                        strcat(answer, " NOK\n");
                                    }
                                    else {
                                        strcat(answer, " ERR\n");
                                    }
                                }
                            } 
                            else if (!strcmp(code, "CLS")) {
                                if (!only_numbers(arg1) || strlen(arg1) != 6) {
                                    strcpy(answer, "RCL ERR\n");
                                }
                                else if (!only_alphanumerical(arg2) || strlen(arg2) != 8) {
                                    strcpy(answer, "RCL ERR\n");
                                }
                                else if (!only_numbers(arg3) || strlen(arg3) != 3) {
                                    strcpy(answer, "RCL ERR\n");
                                }
                                else if (n != 4 || prt_str[strlen(prt_str)-1] != '\n') {
                                    strcpy(answer, "RCL ERR\n");
                                }
                                else {
                                    k = close_auction(arg1, arg2, arg3);
                                    strcpy(answer, "RCL");
                                    if (k == OK) {
                                        strcat(answer, " OK\n");
                                    }
                                    else if (k == NLG) {
                                        strcat(answer, " NLG\n");
                                    }
                                    else if (k == EAU) {
                                        strcat(answer, " EAU\n");
                                    }
                                    else if (k == NOK) {
                                        strcat(answer, " NOK\n");
                                    }
                                    else if (k == EOW) {
                                        strcat(answer, " EOW\n");
                                    }
                                    else if (k == END) {
                                        strcat(answer, " END\n");
                                    }
                                    else {
                                        strcat(answer, " ERR\n");
                                    }
                                
                                }

                            }
                            else if (!strcmp(code, "SAS")) {
                                if (!only_numbers(arg1) || strlen(arg1) != 3) {
                                    strcpy(answer, "RSA ERR\n");
                                    write(sock, answer, strlen(answer));
                                }
                                else if (n != 2 || prt_str[strlen(prt_str)-1] != '\n') {
                                    strcpy(answer, "RSA ERR\n");
                                    write(sock, answer, strlen(answer));
                                }
                                else {
                                    char asset_path[100];
                                    char *path = get_auction_path(arg1);
                                        char *asset_name = get_asset_name(arg1);
                                        size_t asset_size = get_asset_size(arg1);
                                        sprintf(asset_path, "%s%s", path, asset_name);

                                        sprintf(answer, "RSA OK %s %lu ", asset_name, asset_size);
                                        write(sock, answer, strlen(answer));

                                        FILE *fp = fopen(asset_path, "r"); 
                                        free(path);
                                        free(asset_name);
                                        if (fp == NULL) {
                                            printf("Error opening file.\n");
                                            memcpy(answer, "\0", sizeof(answer));
                                            strcpy(answer, "RSA NOK\n");
                                        }   
                                        else {     
                                            printf("Sending file...\n");
                                            while (1) {
                                                memset(prt_str, '\0', sizeof(prt_str));
                                                ret = fread(prt_str, 1, 127, fp);
                                                if (ret < 0) {
                                                    printf("Error reading file.\n");
                                                    memcpy(answer, "\0", sizeof(answer));
                                                    strcpy(answer, "RSA NOK\n");
                                                    break;
                                                }
                                                if (ret == 0) {
                                                    printf("File sent.\n");
                                                    break;
                                                }
                                                wr = write(sock, prt_str, ret);
                                                
                                            } 
                                            if (fclose(fp) != 0) {
                                                printf("Error closing file.\n");
                                                memcpy(answer, "\0", sizeof(answer));
                                                strcpy(answer, "RSA NOK\n");
                                            }
                                        }
                                }
                            }
                            else if (!strcmp(code, "BID")) {
                                if (!only_numbers(arg1) || strlen(arg1) != 6) {
                                    strcpy(answer, "RBD ERR\n");
                                }
                                else if (!only_alphanumerical(arg2) || strlen(arg2) != 8) {
                                    strcpy(answer, "RBD ERR\n");
                                }
                                else if (!only_numbers(arg3) || strlen(arg3) != 3) {
                                    strcpy(answer, "RBD ERR\n");
                                }
                                if (!only_numbers(arg4) || strlen(arg4) > 6) {
                                    strcpy(answer, "RBD ERR\n");
                                }
                                else if (n != 5 || prt_str[strlen(prt_str)-1] != '\n') {
                                    strcpy(answer, "RBD ERR\n");
                                }
                                else {
                                    k = create_bid(arg1, arg2, arg3, arg4);
                                    strcpy(answer, "RBD");
                                    if (k == NOK) {
                                        strcat(answer, " NOK\n");
                                    }
                                    if (k == NLG) {
                                        strcat(answer, " NLG\n");
                                    }
                                    if (k == ACC) {
                                        strcat(answer, " ACC\n");
                                    }
                                    if (k == REF) {
                                        strcat(answer, " REF\n");
                                    }
                                    if (k == ILG) {
                                        strcat(answer, " ILG\n");
                                    }
                                }
                            }

                            if (strcmp(code, "SAS") != 0) {
                                ret = write(sock, answer, strlen(answer)+1);
                            }

                            printf("-------------------------------------------------------\n"); 

                        }          
                        }
                    }
                }

                // UDP Socket
                if(FD_ISSET(ufd,&testfds))
                {
                    addrlen = sizeof(udp_useraddr);
                    ret=recvfrom(ufd,prt_str,89,0,(struct sockaddr *)&udp_useraddr,&addrlen);
                    if(ret>=0)
                    {
                        errcode=getnameinfo( (struct sockaddr *) &udp_useraddr,
                                addrlen,host,sizeof host, service,sizeof service,0);
                                                
                        char code[5], arg1[30], arg2[30], arg3[30], answer[6000], l[30];
                        memset(answer, '\0', sizeof(answer));
                        memset(code, '\0', sizeof(code));
                        memset(arg1, '\0', sizeof(arg1));
                        memset(arg2, '\0', sizeof(arg2));
                        memset(arg3, '\0', sizeof(arg3));
                        memset(l, '\0', sizeof(l));
                        n = sscanf(prt_str, "%s %s %s %s", code, arg1, arg2, arg3);

                        int k, i = 0;
                        if (!strcmp(code, "LST")) {
                            if (verbose_mode) {
                                printf("Request: (%s)\n", code);
                                printf("Origin: (%s, %s)\n", host, PORT);
                            }   
                            auction a[1001];
                            strcpy(answer, "RLS");
                            if(n != 1 || prt_str[strlen(prt_str)-1] != '\n') {
                                strcat(answer, " ERR\n");
                            }
                            else {
                                k = get_all_auctions(a);
                                if (k == OK) {
                                    strcat(answer, " OK");
                                    while (a[i].active != last)
                                    {                                            
                                        snprintf(l, sizeof(l)," %s %d", a[i].id, a[i].active);
                                        strcat(answer, l);
                                        i++;
                                    }
                                    strcat(answer, "\n");
                                }
                                else {
                                    strcat(answer, " NOK\n");
                                }
                            }
                        } else if (!strcmp(code, "LMB")) {
                            if (verbose_mode) {
                                printf("Request: (%s, %s)\n", code, arg1);
                                printf("Origin: (%s, %s)\n", host, PORT);
                            }   
                            auction a[1001];
                            strcpy(answer, "RMB");
                            if(n != 2 || prt_str[strlen(prt_str)-1] != '\n' || strlen(arg1) != 6 || !only_numbers(arg1)) {
                                strcat(answer, " ERR\n");
                            }
                            else {
                                k = get_user_bids(arg1, a);
                                if (k == OK) {
                                    strcat(answer, " OK");
                                    while (a[i].active != last)
                                    {                                            
                                        snprintf(l, sizeof(l)," %s %d", a[i].id, a[i].active);
                                        strcat(answer, l);
                                        i++;
                                    }
                                    strcat(answer, "\n");
                                }
                                else if (k == NLG) {
                                    strcat(answer, " NLG\n");
                                }
                                else {
                                    strcat(answer, " NOK\n");
                                }
                            }                            
                        } else if (!strcmp(code, "LMA")) {
                            if (verbose_mode) {
                                printf("Request: (%s, %s)\n", code, arg1);
                                printf("Origin: (%s, %s)\n", host, PORT);
                            }  
                            auction a[1001];
                            strcpy(answer, "RMA");
                            if(n != 2 || prt_str[strlen(prt_str)-1] != '\n' || strlen(arg1) != 6 || !only_numbers(arg1)) {
                                strcat(answer, " ERR\n");
                            }
                            else {
                                k = get_user_auctions(arg1, a);
                                if (k == OK) {
                                    strcat(answer, " OK");
                                    while (a[i].active != last)
                                    {                                            
                                        snprintf(l, sizeof(l)," %s %d", a[i].id, a[i].active);
                                        strcat(answer, l);
                                        i++;
                                    }
                                    strcat(answer, "\n");
                                }
                                else if (k == NLG) {
                                    strcat(answer, " NLG\n");
                                }
                                else {
                                    strcat(answer, " NOK\n");
                                }
                            }
                        } else if (!strcmp(code, "SRC")) {
                            if (verbose_mode) {
                                printf("Request: (%s, %s)\n", code, arg1);
                                printf("Origin: (%s, %s)\n", host, PORT);
                            }  
                            strcpy(answer, "RRC");

                            if(n != 2 || prt_str[strlen(prt_str)-1] != '\n' || strlen(arg1) != 3 || !only_numbers(arg1)) {
                                strcat(answer, " ERR\n");
                            }
                            else {
                                auction a;
                                k = get_record(arg1, &a);
                                if (k == OK){
                                    strcat(answer, " OK");

                                    char intro[150], bid[100], end[100];
                                    memset(intro, '\0', sizeof(intro));
                                    sprintf(intro, " %s %s %s %s %s %s %s", a.host_uid, a.auction_name, a.asset_name, 
                                                    a.start_value, a.start_date, a.start_time,  a.time_active);
                                    strcat(answer, intro);

                                    i = 0;
                                    while (a.bids[i].last_bid != last)
                                    {  
                                        memset(bid, '\0', sizeof(bid));
                                        sprintf(bid ," B %s %s %s %s %s", a.bids[i].bidder_UID, a.bids[i].bid_value, a.bids[i].bid_date,
                                                 a.bids[i].bid_time, a.bids[i].bid_sec_time);
                                        strcat(answer, bid);
                                        i++;
                                    }
                                    if (a.active == 0) {
                                        memset(end, '\0', sizeof(end));
                                        sprintf(end," E %s %s %s", a.end_date, a.end_time, a.end_sec_time);
                                        strcat(answer, end);
                                    }

                                    strcat(answer, "\n");
                                }
                                else {
                                    strcat(answer, " NOK\n");
                                }
                            }
                        
                        } else if (!strcmp(code, "LIN")) {
                            if (verbose_mode) {
                                printf("Request: (%s, %s, %s)\n", code, arg1, arg2);
                                printf("Origin: (%s, %s)\n", host, PORT);
                            }  
                            if (n != 3 || prt_str[strlen(prt_str)-1] != '\n' 
                            || strlen(arg1) != 6 || !only_numbers(arg1) || strlen(arg2) != 8 || !only_alphanumerical(arg2)) {
                                strcpy(answer, "RLI ERR\n");
                            }
                            else {
                                k = login_user(arg1, arg2);
                                if (k == OK) {
                                    strcpy(answer, "RLI OK\n");
                                }
                                else if (k == REG) {
                                    strcpy(answer, "RLI REG\n");
                                }
                                else {
                                    strcpy(answer, "RLI NOK\n");
                                }
                            }
                        } else if (!strcmp(code, "LOU")) {
                            if (verbose_mode) {
                                printf("Request: (%s, %s, %s)\n", code, arg1, arg2);
                                printf("Origin: (%s, %s)\n", host, PORT);
                            }  
                            if (n != 3 || prt_str[strlen(prt_str)-1] != '\n' 
                            || strlen(arg1) != 6 || !only_numbers(arg1) || strlen(arg2) != 8 || !only_alphanumerical(arg2)) {
                                strcpy(answer, "RLO ERR\n");
                            }
                            else {
                                k = logout(arg1, arg2);
                                if (k == OK) {
                                    strcpy(answer, "RLO OK\n");
                                }
                                else if (k == UNR) {
                                    strcpy(answer, "RLO UNR\n");
                                }
                                else {
                                    strcpy(answer, "RLO NOK\n");
                                }
                            }
                        } else if (!strcmp(code, "UNR")) {
                            if (verbose_mode) {
                                printf("Request: (%s, %s, %s)\n", code, arg1, arg2);
                                printf("Origin: (%s, %s)\n", host, PORT);
                            }  
                            if (n != 3 || prt_str[strlen(prt_str)-1] != '\n' 
                            || strlen(arg1) != 6 || !only_numbers(arg1) || strlen(arg2) != 8 || !only_alphanumerical(arg2)) {
                                strcpy(answer, "RUR ERR\n");
                            }
                            else {
                                k = unregister(arg1, arg2);
                                if (k == OK) {
                                    strcpy(answer, "RUR OK\n");
                                }
                                else if (k == UNR) {
                                    strcpy(answer, "RUR UNR\n");
                                }
                                else {
                                    strcpy(answer, "RUR NOK\n");
                                }
                            }
                        } else {
                            if (verbose_mode) {
                                printf("Request: (%s, %s, %s)\n", code, arg1, arg2);
                                printf("Origin: (%s, %s)\n", host, PORT);
                            }  
                            strcpy(answer, "ERR\n");
                        }                        
                        
                        if (verbose_mode) {
                            memset(code, '\0', sizeof(code));
                            memset(arg1, '\0', sizeof(arg1));
                            memset(arg2, '\0', sizeof(arg2));
                            int j;
                            j = sscanf(answer, "%s %s %30[^\n]", code, arg1, arg2);
                            if (arg2[strlen(arg2)-1] == '\n')
                                arg2[strlen(arg2)-1] = '\0';
                            printf("Sending: %s\nStatus: %s\n", code, arg1);
                            if (j == 3)
                                printf("Data (first 30 bytes): %s\n", arg2);
                            int port = ntohs(udp_useraddr.sin_port);
                            printf("Destination: (%s, %d)\n", host, port);
                        }
                        ret = sendto(ufd, answer,strlen(answer)+1,0, (struct sockaddr *)&udp_useraddr, addrlen);
                        
                        printf("-------------------------------------------------------\n");               
                        
                    }

                }
    }
}
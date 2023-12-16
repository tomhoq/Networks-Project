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
#include "db.c"
#define PORT "58025"
#define TEJO "tejo.tecnico.ulisboa.pt"

void print_all_characters(const char *input_string) {
    int i= 1;
    while (*input_string != '\0') {
        printf("Character %d: %c, ASCII: %d\n", i, *input_string, *input_string);
        input_string++;
        i++;
    }
}

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

int main (int argc, char* argv[]) {

    // socket variables
    struct addrinfo hints_udp, *res_udp, *res_tcp, hints_tcp;
    struct sockaddr_in udp_useraddr, tcp_useraddr;
    socklen_t addrlen;
    int i, out_fds, n, errcode, ret, ufd, tcp;
    
    char buffer[128], username[7],password[9];
    char function[13], arg1[20], arg2[20], arg3[20], arg4[20];

    char in_str[128];

    fd_set inputs, testfds;
    struct timeval timeout;

    char prt_str[128];

    char host[NI_MAXHOST], service[NI_MAXSERV];

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
            strcpy(ASport, PORT);
        }
        else {
            printf("Invalid arguments\n");
            exit(1);
        }
    }
    else if (argc == 3) {
        if (strcmp(argv[1], "-p") == 0) {
            strcpy(ASport, argv[2]);  //falta verificar que argv[2] é um port number!!!!!!
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
            strcpy(ASport, argv[2]);  //falta verificar que argv[2] é um port number!!!!!!
            verbose_mode = 1;
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

    printf("%s %d\n", ASport, verbose_mode);

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
        exit(1);// On error

    if((errcode=getaddrinfo(NULL,ASport,&hints_tcp,&res_tcp))!=0)
        exit(1);// On error

    ufd=socket(res_udp->ai_family,res_udp->ai_socktype,res_udp->ai_protocol);
    if(ufd==-1)
        exit(1);

    tcp=socket(res_tcp->ai_family,res_tcp->ai_socktype,res_tcp->ai_protocol);
    if(tcp==-1)
        exit(1);
    if(bind(ufd,res_udp->ai_addr,res_udp->ai_addrlen)==-1)
    {
        perror("Bind error UDP server.\n");
        exit(1);// On error
    }

    if(bind(tcp,res_tcp->ai_addr,res_tcp->ai_addrlen)==-1)
    {
        perror("Bind error UDP server.\n");
        exit(1);// On error
    }
    
    // Listen on the TCP socket
    if (listen(tcp, 5) == -1) {
        perror("TCP socket listen failed");
        exit(EXIT_FAILURE);
    }

    if(res_udp!=NULL)
        freeaddrinfo(res_udp);
    if(res_tcp!=NULL)
        freeaddrinfo(res_tcp);

    FD_ZERO(&inputs); // Clear input mask
    FD_SET(tcp,&inputs); // Set standard input channel on
    FD_SET(ufd,&inputs); // Set UDP channel on

    //    printf("Size of fd_set: %d\n",sizeof(fd_set));
    //    printf("Value of FD_SETSIZE: %d\n",FD_SETSIZE);

    while(1)
    {
        testfds=inputs; // Reload mask
        //printf("testfds byte: %d\n",((char *)&testfds)[0]); // Debug
        memset((void *)&timeout,0,sizeof(timeout));
        timeout.tv_sec=10;

        out_fds=select(FD_SETSIZE,&testfds,(fd_set *)NULL,(fd_set *)NULL,(struct timeval *) &timeout);
        // testfds is now '1' at the positions that were activated
        // printf("testfds byte: %d\n",((char *)&testfds)[0]); // Debug

        memset(prt_str, '\0', sizeof(prt_str));
        printf("%d\n",out_fds);

        switch(out_fds)
        {
            case 0:
                printf("\n ---------------Timeout event-----------------\n");
                break;
            case -1:
                perror("select");
                exit(1);
            default:
                if(FD_ISSET(tcp,&testfds))
                {
                    //fazer fork aqui
                    int sock = accept(tcp, NULL, NULL);
                    
                    if (sock == -1) {
                        perror("TCP socket accept error.\n");
                    
                    } else {

                        addrlen = sizeof(tcp_useraddr);
                        ret = read(sock, prt_str, 127);
                        
                        if (ret>=0) {

                            if(strlen(prt_str)>0){
                                prt_str[ret-1]='\0';
                            }

                            printf("---TCP socket: %s\n",prt_str); //debug

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

                            n = sscanf(prt_str, "%s %s %s %s %s %s %s %s %s", code, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
                            int k, i = 0;

                            if (!strcmp(code, "OPA")) {
                                k = create_auction(arg1, arg2, arg3, arg4, arg5, arg6);
                                strcpy(answer, "ROA");
                                if (k == OK) {
                                    strcat(answer, " OK");
                                    char *open_aid = malloc(5*sizeof(char));
                                    memset(open_aid, '\0', sizeof(open_aid));
                                    sprintf(open_aid, " %s", get_most_recent_aid());

                                    strcat(answer, open_aid);
                                    strcat(answer, "\n");
                                    free(open_aid);
                                    
                                    char *path = get_auction_path(get_most_recent_aid());

                                    printf("Debug stage 1\n");

                                    FILE *fp = fopen(path, "wb");
                                    if (fp == NULL) {
                                        printf("Error opening file.\n");
                                        strcpy(answer, "ROA ERR\n");
                                    }
                                    else {
                                        fwrite(arg8, 1, strlen(arg8), fp);
                                        while ((ret=read(sock, prt_str, 127)) > 0) {
                                        printf("%s\n", prt_str);
                                        printf("%d\n", ret);
                                        if(strlen(prt_str)>0){
                                            prt_str[ret-1]='\0';
                                        }
                                        fwrite(prt_str, 1, strlen(prt_str), fp);
                                        }

                                        if (fclose(fp) != 0) {
                                            printf("Error closing file.\n");
                                            strcpy(answer, "ROA ERR\n");
                                        }
                                        if (verbose_mode) {
                                            printf("Auction created.\n");
                                        }
                                    }
                                    free(path);              
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
                        }
                    }
                }
                if(FD_ISSET(ufd,&testfds))
                {
                    addrlen = sizeof(udp_useraddr);
                    ret=recvfrom(ufd,prt_str,80,0,(struct sockaddr *)&udp_useraddr,&addrlen);
                    if(ret>=0)
                    {
                        if(strlen(prt_str)>0){
                            prt_str[ret-1]='\0';
                        }
                        printf("---UDP socket: %s\n",prt_str);
                        errcode=getnameinfo( (struct sockaddr *) &udp_useraddr,
                                addrlen,host,sizeof host, service,sizeof service,0);

                        if(errcode==0)
                            printf("       Sent by [%s:%s]\n", host, service);
                        
                        char code[5], arg1[30], arg2[30], arg3[30], answer[6000], l[30];
                        memset(answer, '\0', sizeof(answer));
                        memset(code, '\0', sizeof(code));
                        memset(arg1, '\0', sizeof(arg1));
                        memset(arg2, '\0', sizeof(arg2));
                        memset(arg3, '\0', sizeof(arg3));
                        memset(l, '\0', sizeof(l));
                        n = sscanf(prt_str, "%s %s %s %s", code, arg1, arg2, arg3);
                        int k, i = 0;
                        //print_all_characters(prt_str);

                        if (!strcmp(code, "LST")) {
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
                                    strcat(answer, " ERR\n");
                                }
                            }
                        } else if (!strcmp(code, "LMB")) {
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
                            if (n != 3 || prt_str[strlen(prt_str)-1] != '\n' 
                            || strlen(arg1) != 6 || !only_numbers(arg1) || strlen(arg2) != 8 || !only_alphanumerical(arg2)) {
                                strcpy(answer, "RLI ERR\n");
                            }
                            else {
                                k = login_user(arg1, arg2);
                                //printf("k = %d\n", k);
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
                            if (n != 3 || prt_str[strlen(prt_str)-1] != '\n' 
                            || strlen(arg1) != 6 || !only_numbers(arg1) || strlen(arg2) != 8 || !only_alphanumerical(arg2)) {
                                strcpy(answer, "RLO ERR\n");
                            }
                            else {
                                k = logout(arg1, arg2);
                                //printf("k = %d\n", k);
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
                            if (n != 3 || prt_str[strlen(prt_str)-1] != '\n' 
                            || strlen(arg1) != 6 || !only_numbers(arg1) || strlen(arg2) != 8 || !only_alphanumerical(arg2)) {
                                strcpy(answer, "RUR ERR\n");
                            }
                            else {
                                k = unregister(arg1, arg2);
                                //printf("k = %d\n", k);
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
                            strcpy(answer, "ERR\n");
                        }

                        printf("sending : %s\n", answer);
                        ret = sendto(ufd, answer,strlen(answer)+1,0, (struct sockaddr *)&udp_useraddr, addrlen);
                        if (ret < strlen(buffer))
                            printf("Did not send all.\n");
                
                        
                    }

                }
    }
}
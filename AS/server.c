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
#define PORT "58025"
#define TEJO "tejo.tecnico.ulisboa.pt"

int main (int argc, char* argv[]) {

    // socket variables
    struct addrinfo hints_udp, *res_udp, *res_tcp, hints_tcp;
    struct sockaddr_in udp_useraddr, tcp_useraddr;
    socklen_t addrlen;
    int i,out_fds,n,errcode, ret, ufd, tcp;
    
    char buffer[128], username[7],password[9];
    char function[13], arg1[20], arg2[20], arg3[20], arg4[20];

    char in_str[128];

    fd_set inputs, testfds;
    struct timeval timeout;


    char prt_str[90];

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

// UDP SERVER SECTION
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
        perror("Bind error UDP server\n");
        exit(1);// On error
    }

    if(bind(tcp,res_tcp->ai_addr,res_tcp->ai_addrlen)==-1)
    {
        perror("Bind error UDP server\n");
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
                        perror("TCP socket accept error");
                    } else {
                        addrlen = sizeof(tcp_useraddr);
                        ret = read(sock, prt_str, 80);
                        printf("adaw\n");

                        if(ret>=0)
                        {
                            printf("adaw\n");

                            if(strlen(prt_str)>0){
                                prt_str[ret-1]='\0';
                            }
                            printf("---TCP socket: %s\n",prt_str);
                            errcode=getnameinfo( (struct sockaddr *) &tcp_useraddr,
                                    addrlen,host,sizeof host, service,sizeof service,0);
                            if(errcode==0)
                                printf("       Sent by [%s:%s]\n",host,service);
                            
                            char buffer[6] = "aaaaa\0";

                            ret = write(sock, buffer,sizeof(buffer));
                            if (ret < sizeof(buffer))
                                printf("Did not send all\n");
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
                        errcode=getnameinfo( (struct sockaddr *) &udp_useraddr,addrlen,host,sizeof host, service,sizeof service,0);
                        if(errcode==0)
                            printf("       Sent by [%s:%s]\n", host, service);
                        char buffer[6] = "aaaaaa\0";  
                        printf("sending\n");
                        ret = sendto(ufd, buffer,strlen(buffer),0, (struct sockaddr *)&udp_useraddr, addrlen);
                        if (ret < strlen(buffer))
                            printf("Did not send all\n");
                        
                    }

                }
        }
    }
}
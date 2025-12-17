/* Version from: 15.12.2025 */

#include "s_aux.h"



int main(int argc, char **argv) {

    int acceptFd, connFd;
    struct sockaddr_in servAddr;
    size_t parcelSize;

    char buffLogs[MSGMLEN];

    fflush(stdout);
    setvbuf(stdout, NULL, _IONBF, 0);

    char *inputServIp = "127.0.0.1";
    unsigned short inputServPort = 9877;  

    if (argc >= 2) {
        inputServIp = argv[1];
    }

    int logFd;
    if ((logFd = open(SERVLOGPATH, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1) {
        printf("%s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    else {
        snprintf(buffLogs, MSGMLEN, "Log file accessed, path: %s\n", SERVLOGPATH);
        writeft(logFd, buffLogs, inputServIp);
    }
    snprintf(buffLogs, MSGMLEN, "Starting... Server's IP Address: %s, Inbound Port: %u.\n", inputServIp, inputServPort);
    writeft(logFd, buffLogs, inputServIp);


    struct pollfd readFromCliFds[CLIENTCAP];                                   /* poll for sockets reading client's msgs */
    memset(&readFromCliFds, 0, sizeof(readFromCliFds));

    struct pollfd acceptSockFds[1];                                            /* poll for sockets listening to connections*/
    memset(&acceptSockFds, 0, sizeof(acceptSockFds));


    if ((acceptFd = getlsocket(inputServIp, inputServPort)) == -1) {        
        write(0, "Error creating a listening socket, aborting.\n", 46);
        exit(EXIT_FAILURE);
    }     
    acceptSockFds[0].fd = acceptFd;
    acceptSockFds[0].events = POLLIN | POLLPRI;
    int tmpFlags = fcntl(acceptFd, F_GETFL, 0);
    if (fcntl(acceptFd, F_SETFL, tmpFlags | O_NONBLOCK) == -1) {
        write(0, "fcntl failed\n", 14);
        exit(EXIT_FAILURE);
    }
    
    int curOnline = 0;
    for ( ; ; ) {

                                                                                    /* set .revents everywhere to 0 */
        for (int i = 0; i < sizeof(readFromCliFds)/sizeof(readFromCliFds[0]); i++) 
        {
            readFromCliFds[i].revents = 0;
        }
        for (int i = 0; i < sizeof(acceptSockFds)/sizeof(acceptSockFds[0]); i++)
        {
            acceptSockFds[i].revents = 0;
        }

                                                                                    /* start polling */
        int acceptSockFdsResult = poll(acceptSockFds, 1, 200);
        int readFromCliFdsResult = poll(readFromCliFds, curOnline, 200); 
        
        //printf("readFromCliFdsResult = %i; acceptSockFdsResult = %i.\n", readFromCliFdsResult, acceptSockFdsResult);


                                                                                    /* acceptSockFds */
        if (acceptSockFdsResult < 0) {
            write(0, "acceptSockFds poll error!\n", 27);
        }  
        else if (acceptSockFdsResult == 0) {
            
        }          
        else if (acceptSockFdsResult > 0) {
            printf("in here\n");
            for (int i = 0; i < sizeof(acceptSockFds)/sizeof(acceptSockFds[0]); i++)
                {
                    printf("listeSockarr size %lu\n", sizeof(acceptSockFds)/sizeof(acceptSockFds[0]));

                    if (acceptSockFds[i].revents & POLLIN) {

                        char cliIpStr[INET_ADDRSTRLEN];
                        uint16_t cliPort;
                        struct sockaddr_in cliAddr;
                        socklen_t addrSize = sizeof(cliAddr);


                        connFd = accept(acceptFd, (struct sockaddr *) &cliAddr, &addrSize);
                        curOnline++;
                        add_b_socket(connFd, cliAddr);

                        b_socket *pCurUser;
                        pCurUser = find_b_socket(connFd);

                        inet_ntop(AF_INET, (struct sockaddr *) &pCurUser->addr.sin_addr.s_addr, cliIpStr, INET_ADDRSTRLEN); /* cliIpStr init */
                        cliPort = ntohs(pCurUser->addr.sin_port);
                        snprintf(buffLogs, MSGMLEN, "Accepted a connection from: %s:%u. Socket: %i.\n", cliIpStr, cliPort, connFd); 
                        writeft(logFd, buffLogs, inputServIp);

                        int availIndexInreadFromCliFds = -1;
                        for (int j = 0; j < CLIENTCAP; j++) {
                            if (readFromCliFds[j].fd == 0) {
                                availIndexInreadFromCliFds = j;
                                break;
                            }
                        } 
                        if (availIndexInreadFromCliFds == -1) {
                            write(0, "No available space found in readFromCliFds[]!\n", 47);
                            exit(EXIT_FAILURE);
                        }
                        readFromCliFds[availIndexInreadFromCliFds].fd = connFd;
                        readFromCliFds[availIndexInreadFromCliFds].events = POLLIN | POLLPRI;
                        tmpFlags = fcntl(connFd, F_GETFL, 0);
                        if (fcntl(connFd, F_SETFL, tmpFlags | O_NONBLOCK) == -1) {
                            perror("fcntl");
                            exit(EXIT_FAILURE);
                        }

                    }

                }
        }  


                                                                                /* readFromCliFds */
        if (readFromCliFdsResult < 0) {
            write(0, "readFromCliFds poll error\n", 27);
        }
        else if (readFromCliFdsResult == 0) {
            continue;
            printf("here\n");
        }
        else {
            
            for (int i = 0; i < sizeof(readFromCliFds)/sizeof(readFromCliFds[0]); i++)                                        /* check the sockets for read() */   
            {
                if (readFromCliFds[i].fd > 0 && readFromCliFds[i].revents & POLLIN) //  
                {
                    
                    char buffMsg[MSGMLEN];
                    char cliIpStr[INET_ADDRSTRLEN];
                    char cliActorStr[AUTHORMLEN];
                    int cliFd = readFromCliFds[i].fd;
                    uint16_t cliPort = 0;
                    struct sockaddr_in cliAddr;
                    memset(&cliAddr, 0, sizeof(cliAddr));

                    b_socket *pCurUser;
                    pCurUser = find_b_socket(cliFd);
                    cliAddr = pCurUser->addr;

                    inet_ntop(AF_INET, (struct sockaddr *) &cliAddr.sin_addr.s_addr, cliIpStr, INET_ADDRSTRLEN);
                    cliPort = ntohs(cliAddr.sin_port); 
                                                                

                    int n = read(readFromCliFds[i].fd, buffMsg, MSGMLEN);
                    switch (n)
                    {
                    case -1:
                        break;
                    case 0:
                        snprintf(buffLogs, MSGMLEN, "%s has disconnected.\n", cliActorStr); 
                        memset(&readFromCliFds[i], 0, sizeof(readFromCliFds[i]));
                        delete_b_socket(pCurUser);
                        curOnline--;
                        close(connFd);
                        break;
                    default:
                        buffMsg[MSGMLEN - 1] = '\0';    
                        snprintf(cliActorStr, sizeof(cliActorStr), "%s:%u", cliIpStr, cliPort); 


                        parcelSize = closest_pow2(2 + sizeof(buffMsg) + sizeof(cliActorStr));
                        char *parcel = malloc(parcelSize);
                        anm_create_msg(parcel, parcelSize, cliActorStr, buffMsg);
                        //write(readFromCliFds[i].fd, parcel, parcelSize);
                        write(readFromCliFds[i].fd, "parcel", 7);
                        writeft(logFd, buffMsg, cliActorStr);
                        free(parcel);
                        //snprintf(cliActorStr, sizeof(cliActorStr), "%s:%u", cliIpStr, cliPort);
                        //writeft(logFd, buffMsg, cliActorStr);
                        break;
                    }
                    
                }
            } 
        }

    }




}
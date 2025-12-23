/* Version from: 15.12.2025 */

#include "s_aux.h"



int main(void) {

    int acceptFd, connFd;
    struct sockaddr_in servAddr;
    size_t parcelSize;

    char buffLogs[MSGMLEN];

    fflush(stdout);
    setvbuf(stdout, NULL, _IONBF, 0);
    startup_text();
    

    char inputServIp[INET_ADDRSTRLEN];
    unsigned short inputServPort;  

    const char * const pathToJson = "config.json";
    int g0 = get_server_specs(inputServIp, INET_ADDRSTRLEN, &inputServPort, pathToJson);
    if (g0 < 0) {
        printf("main(): error parsing config.json\n");
        exit(EXIT_FAILURE);
    }

/*    if (argc >= 2) {
        inputServIp = argv[1];
    } */
                                                                                            /* opening the log dir+file */
    
    printf("IPv4 address: %s\n", inputServIp);
    printf("Port:         %u\n", inputServPort);
    

    int logFd;   
    int d0 = 0;                                                                                         
    char *logDirPath = "../logs/";                                                                                            
    DIR *logDir = opendir(logDirPath);
    if (logDir != NULL) {
        // directory exists
    }
    else {
        int d0 = mkdir("../logs/", 0777);
        if (d0 < 0) {
            printf("Error: failed to open/create a log directory./n");
        }
    }

    if (d0 == 0) {
        if ((logFd = open(SERVLOGPATH, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1) {
        printf("%s", strerror(errno));
        exit(EXIT_FAILURE);
        }
        else {
            printf("Log path:     %s\n\n", SERVLOGPATH);
            snprintf(buffLogs, MSGMLEN, "Log file accessed, path: %s\n", SERVLOGPATH);
            writeft(logFd, buffLogs, inputServIp);
        }
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
        


                                                                                    /* acceptSockFds */
        if (acceptSockFdsResult < 0) {
            write(0, "acceptSockFds poll error!\n", 27);
        }  
        else if (acceptSockFdsResult == 0) {
            
        }          
        else if (acceptSockFdsResult > 0) {
            for (int i = 0; i < sizeof(acceptSockFds)/sizeof(acceptSockFds[0]); i++)
                {

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
                        snprintf(buffLogs, MSGMLEN, "%s:%u has joined.\n", cliIpStr, cliPort);
                        char *p2 = malloc(PARCELMLEN);
                        anm_construct_msg(p2, PARCELMLEN, inputServIp, buffLogs);
                        broadcast(readFromCliFds, CLIENTCAP, p2, PARCELMLEN);
                        free(p2);

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
                        printf("Avail fd found: %d\n", availIndexInreadFromCliFds);
                        readFromCliFds[availIndexInreadFromCliFds].fd = connFd;
                        readFromCliFds[availIndexInreadFromCliFds].events = POLLIN | POLLPRI | POLLOUT;
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
        }
        else {
            
            for (int i = 0; i < sizeof(readFromCliFds)/sizeof(readFromCliFds[0]); i++)                                        /* check the sockets for read() */   
            {
                if (readFromCliFds[i].fd > 0) //  && readFromCliFds[i].revents & POLLIN
                {
                    
                    char buffMsg[MSGMLEN];
                    memset(buffMsg, 0, MSGMLEN);
                    char cliIpStr[INET_ADDRSTRLEN];
                    char cliAuthorStr[AUTHORMLEN];
                    int cliFd = readFromCliFds[i].fd;
                    uint16_t cliPort = 0;
                    struct sockaddr_in cliAddr;
                    memset(&cliAddr, 0, sizeof(cliAddr));

                    b_socket *pCurUser;
                    pCurUser = find_b_socket(cliFd);
                    cliAddr = pCurUser->addr;

                    inet_ntop(AF_INET, (struct sockaddr *) &cliAddr.sin_addr.s_addr, cliIpStr, INET_ADDRSTRLEN);
                    cliPort = ntohs(cliAddr.sin_port); 
                                                                

                    int n = read(cliFd, buffMsg, MSGMLEN);
                    switch (n)
                    {
                    case -1:
                        break;
                    case 0:
                        char *parcel = malloc(PARCELMLEN);

                        snprintf(cliAuthorStr, sizeof(cliAuthorStr), "%s:%u", cliIpStr, cliPort);
                        snprintf(buffLogs, MSGMLEN, "%s has disconnected.\n", cliAuthorStr); 

                        //memset(&readFromCliFds[i], 0, sizeof(readFromCliFds[i]));
                        readFromCliFds[i].fd = 0;
                        readFromCliFds[i].events = 0;
                        readFromCliFds[i].revents = 0;
                        delete_b_socket(pCurUser);
                        curOnline--;
                        close(cliFd);

                        anm_construct_msg(parcel, PARCELMLEN, inputServIp, buffLogs);
                        broadcast(readFromCliFds, CLIENTCAP, parcel, PARCELMLEN);
                        writeft(logFd, buffLogs, inputServIp);
                        free(parcel);

                        break;
                    default:
                        buffMsg[MSGMLEN - 1] = '\0';    
                        snprintf(cliAuthorStr, sizeof(cliAuthorStr), "%s:%u", cliIpStr, cliPort); 

                        char *parcel2 = malloc(PARCELMLEN);
                        anm_construct_msg(parcel2, PARCELMLEN, cliAuthorStr, buffMsg);
                                                                            /* send the message to all the clients */
                        int c1 = broadcast(readFromCliFds, CLIENTCAP, parcel2, PARCELMLEN);
//                        printf("%d message(s) sent out.\n", c1);
                        writeft(logFd, buffMsg, cliAuthorStr);
                        free(parcel2);
                        break;
                    }
                    
                }
            } 
        }

    }




}
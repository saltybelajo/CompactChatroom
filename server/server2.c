/* Version from: 15.12.2025 */

#include "s_aux.h"



int main(int argc, char **argv) {

    int acceptFd, connFd;
    struct sockaddr_in servAddr;
    size_t parcelSize;

    char buffLogs[MSGMLEN];

    fflush(stdout);
    setvbuf(stdout, NULL, _IONBF, 0);
    srand(time(NULL));
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

    char authorServer[22];
    snprintf(authorServer, 22, "%s:%u", inputServIp, inputServPort);
    

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

    struct pollfd otherFds[2];                                            /* poll for other fds */
    memset(&otherFds, 0, sizeof(otherFds));


    uint8_t otherCount = 0;
    /* creating a listening socket. i == 0 */
    if ((acceptFd = getlsocket(inputServIp, inputServPort)) == -1) {        
        write(0, "Error creating a listening socket, aborting.\n", 46);
        exit(EXIT_FAILURE);
    }     
    int tmpFlags = fcntl(acceptFd, F_GETFL, 0);
    if (fcntl(acceptFd, F_SETFL, tmpFlags | O_NONBLOCK) == -1) {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }
    otherFds[otherCount].fd = acceptFd;
    otherFds[otherCount].events = POLLIN | POLLPRI;
    int curOnline = 0;
    otherCount++;

    /* fd for reading from getline(). i == 1 */
    size_t getlineSize;
    ssize_t nread;
    char *inputLine;
    otherFds[otherCount].fd = 0;
    otherFds[otherCount].events = POLLIN | POLLPRI; //stdin is already non-blocking
    otherCount++;

    /* fd for periodic server announcements. i == 2 */
    int announceFd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (announceFd < 0) {
        printf("main(): failed to create announceFd\n");
        exit(EXIT_FAILURE);
    }
    tmpFlags = fcntl(announceFd, F_GETFL, 0);
    if (fcntl(announceFd, F_SETFL, tmpFlags | O_NONBLOCK) == -1) {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }
    otherFds[otherCount].fd = announceFd;
    otherFds[otherCount].events = POLLIN;
    otherCount++;

    
    

    time_t t_last = time(NULL);
    for ( ; ; ) {

        time_t t_cur = time(NULL);
                                                                                    /* set .revents everywhere to 0 */
        for (int i = 0; i < sizeof(readFromCliFds)/sizeof(readFromCliFds[0]); i++) 
        {
            readFromCliFds[i].revents = 0;
        }
        for (int i = 0; i < sizeof(otherFds)/sizeof(otherFds[0]); i++)
        {
            otherFds[i].revents = 0;
        }

                                                                                    /* start polling */
        int otherFdsResult = poll(otherFds, otherCount, 20);
        int readFromCliFdsResult = poll(readFromCliFds, curOnline, 20); 
        


                                                                                    /* otherFds */
        if (otherFdsResult < 0) {
            write(0, "otherFds poll error!\n", 22);
        }  
        else if (otherFdsResult == 0) {
            
        }          
        else if (otherFdsResult > 0) {
            for (int i = 0; i < sizeof(otherFds)/sizeof(otherFds[0]); i++)
                {

                    if (i == 0 && otherFds[i].revents & POLLIN) {

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
                        anm_construct_msg(p2, PARCELMLEN, authorServer, buffLogs);
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
                        printf("A client connected.\n");
                        readFromCliFds[availIndexInreadFromCliFds].fd = connFd;
                        readFromCliFds[availIndexInreadFromCliFds].events = POLLIN | POLLPRI | POLLOUT;
                        tmpFlags = fcntl(connFd, F_GETFL, 0);
                        if (fcntl(connFd, F_SETFL, tmpFlags | O_NONBLOCK) == -1) {
                            perror("fcntl");
                            exit(EXIT_FAILURE);
                        }

                    }

                    if (i == 1 && otherFds[i].revents & POLLIN) {
                        inputLine = NULL;
                        getlineSize = 0;

                        if ((nread = getline(&inputLine, &getlineSize, stdin)) > 0) {
                            
                            if (inputLine == NULL) {
                                /* ignore it */
                                free(inputLine);
                            }
                            else if (inputLine[0] == '/') {
                                unsigned long hashValue = hash_sdbm(inputLine);
                                if (hashValue == hash_sdbm("/quit\n")) {
                                    free(inputLine);
                                    close(logFd);

                                    char *p3 = malloc(PARCELMLEN);
                                    strncpy(buffLogs, "Server is shutting down...\n", 28);
                                    anm_construct_msg(p3, PARCELMLEN, authorServer, buffLogs);
                                    broadcast(readFromCliFds, CLIENTCAP, p3, PARCELMLEN);
                                    exit(EXIT_SUCCESS);
                                }
                                else if (hashValue == hash_sdbm("/online\n")) {
                                    free(inputLine);
                                    printf("Current online: %d\n", curOnline);
                                }
                                else if (hashValue == hash_sdbm("/clients\n")) {
                                    free(inputLine);
                                    printf("Current clients are:\n");
                                    for (int g = 0; g < CLIENTCAP; g++) {
                                        if (readFromCliFds[g].fd > 0) {

                                            int __fd0 = readFromCliFds[g].fd;
                                            char __cliIpStr[INET_ADDRSTRLEN];
                                            uint16_t __cliPort;
                                            struct sockaddr_in __cliAddr;
                                            memset(&__cliAddr, 0, sizeof(__cliAddr));


                                            b_socket *__pCurUser2 = NULL;
                                            __pCurUser2 = find_b_socket(__fd0);
                                            
                                            if (__pCurUser2 != NULL) {
                                                __cliAddr = __pCurUser2->addr;
                                                inet_ntop(AF_INET, (struct sockaddr *) &__cliAddr.sin_addr.s_addr, __cliIpStr, INET_ADDRSTRLEN);
                                                __cliPort = ntohs(__cliAddr.sin_port);

                                                printf("%s:%u on fd = %d\n", __cliIpStr, __cliPort, __fd0);
                                            }
                                        }
                                    }
                                }
                            }
                            //write(connectFd, inputLine, nread);
                        }
                    }

                    //if (i == 2 && otherFds[i].revents & POLLIN) {
                    

                    
                    //}

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
                        printf("A client disconnected.\n");

                        //memset(&readFromCliFds[i], 0, sizeof(readFromCliFds[i]));
                        readFromCliFds[i].fd = 0;
                        readFromCliFds[i].events = 0;
                        readFromCliFds[i].revents = 0;
                        delete_b_socket(pCurUser);
                        curOnline--;
                        close(cliFd);

                        anm_construct_msg(parcel, PARCELMLEN, authorServer, buffLogs);
                        broadcast(readFromCliFds, CLIENTCAP, parcel, PARCELMLEN);
                        writeft(logFd, buffLogs, authorServer);
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
        if ((t_cur - t_last) > 120) {
                    
                    t_last = t_cur;
                    size_t rstrSize = 5;
                    char *rstr = malloc(rstrSize);
                    memset(rstr, 0, rstrSize);
                    random_base64_string(rstr, rstrSize);
                    char buffMsg[MSGMLEN];
                    memset(buffMsg, 0, MSGMLEN);
                    
                    char *p5 = malloc(PARCELMLEN);
                    snprintf(buffMsg, MSGMLEN, "You are connected to %s. Random string: %s.\n", authorServer, rstr);
                    anm_construct_msg(p5, PARCELMLEN, authorServer, buffMsg);
                    broadcast(readFromCliFds, CLIENTCAP, p5, PARCELMLEN);

                    free(p5);
                    free(rstr);
                }

    }




}
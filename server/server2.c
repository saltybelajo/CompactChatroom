/* Version from: 11.12.2025 */

#include "s_aux.h"



int main(int argc, char **argv) {

    int listenFd, connFd;
    struct sockaddr_in servAddr;

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


    if ((listenFd = getlsocket(inputServIp, inputServPort)) == -1) {        /* initialize the listening socket*/
        exit(EXIT_FAILURE);
    }                  


    struct pollfd pFds[CLIENTCAP];                                   /* set up poll() */
    memset(&pFds, 0, sizeof(pFds));
    
    pFds[0].fd = listenFd;
    pFds[0].events = POLLIN | POLLPRI;
    int curOnline = 0;

    int flags1 = fcntl(listenFd, F_GETFL, 0);
    if (fcntl(listenFd, F_SETFL, flags1 | O_NONBLOCK) == -1) {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }
    

    for ( ; ; ) {

        for (int i = 0; i < CLIENTCAP; i++)
        {
            pFds[i].revents = 0;
        }
        int pollResult = poll(pFds, curOnline + 1, 1000);
//        printf("pollResult: %d, curOnline: %d, curOnline fd: %d\n", pollResult, curOnline, pFds[curOnline].fd);
        if (pollResult < 0) {
            perror("poll error\n");
//            exit(EXIT_FAILURE);
        }
        else if (pollResult == 0) {
            continue;
        }
        else {
            if (pFds[0].revents & POLLIN) {                                                     /* check the listening socket for new connections */

                char cliIpStr[INET_ADDRSTRLEN];
                uint16_t cliPort;
                char buffAux[64];
                struct sockaddr_in cliAddr;
                socklen_t addrSize = sizeof(cliAddr);


                connFd = accept(listenFd, (struct sockaddr *) &cliAddr, &addrSize);
                curOnline++;
                                                                    /* looking for the first free cell in curUsers[]*/
                /* int availIndexInCurUser = -1;
                for (int j = 0; j < CLIENTCAP; j++) {
                    if (curUsers[j].fd == 0) {
                        availIndexInCurUser = j;
                        break;
                    }
                }    
                if (availIndexInCurUser == -1) {
                    perror("No available space found in curUsers[]!\n");
                    exit(EXIT_FAILURE);
                }            */
                                                       /* once a slot has been found, put the creds in there*/
                //curUsers[availIndexInCurUser] = (b_socket) {.fd = connFd, .addr = cliAddr};                             
                //b_socket *pCurUser = &curUsers[availIndexInCurUser]; //malloc(sizeof(curUsers[availIndexInCurUser]));
                add_b_socket(connFd, cliAddr);

                b_socket *pCurUser;
                pCurUser = find_b_socket(connFd);
                
    

                inet_ntop(AF_INET, (struct sockaddr *) &pCurUser->addr.sin_addr.s_addr, cliIpStr, INET_ADDRSTRLEN);
                snprintf(buffLogs, MSGMLEN, "Accepted a connection from: %s:%u. Socket: %i.\n", cliIpStr, ntohs(pCurUser->addr.sin_port), connFd); //change to strcliip cliport
                writeft(logFd, buffLogs, inputServIp);

                int availIndexInpFds = -1;
                for (int j = 0; j < CLIENTCAP; j++) {
                    if (pFds[j].fd == 0) {
                        availIndexInpFds = j;
                        break;
                    }
                } 
                if (availIndexInpFds == -1) {
                    perror("No available space found in pFds[]!\n");
                    exit(EXIT_FAILURE);
                }
                pFds[availIndexInpFds].fd = connFd;
                pFds[availIndexInpFds].events = POLLIN | POLLPRI;
                flags1 = fcntl(connFd, F_GETFL, 0);
                if (fcntl(connFd, F_SETFL, flags1 | O_NONBLOCK) == -1) {
                    perror("fcntl");
                    exit(EXIT_FAILURE);
                }
                
            }


            for (int i = 1; i < CLIENTCAP; i++)                                                 /* check the sockets for incoming messages */   
            {

                if (pFds[i].fd > 0 && pFds[i].revents & POLLIN) //  
                {

                    char buffMsg[MSGMLEN];
                    char cliIpStr[INET_ADDRSTRLEN];
                    char cliActorStr[32];
                    int cliFd = pFds[i].fd;
                    uint16_t cliPort = 0;
                    struct sockaddr_in cliAddr;
                    memset(&cliAddr, 0, sizeof(cliAddr));

                    b_socket *pCurUser;
                    pCurUser = find_b_socket(cliFd);

                    /* int indexInCurUsers = -1;
                    for (int j = 0; j < CLIENTCAP; j++) {                           
                        if (cliFd == curUsers[j].fd) {                             
                            indexInCurUsers = j;
                            break;
                        }
                    }
                    if (indexInCurUsers == -1) {
                        snprintf(buffLogs, MSGMLEN, "Not able to resolve the IP address for the fd=%i.\n", cliFd);
                        perror(buffLogs);
                        exit(EXIT_FAILURE);
                    } */
                    cliAddr = pCurUser->addr;

                    inet_ntop(AF_INET, (struct sockaddr *) &cliAddr.sin_addr.s_addr, cliIpStr, INET_ADDRSTRLEN);
                    cliPort = ntohs(cliAddr.sin_port); 
                                                                /* resolved the IP creds into cliIpStr and cliPort*/

                    int n = read(pFds[i].fd, buffMsg, MSGMLEN);
                    switch (n)
                    {
                    case -1:
                        break;
                    case 0:
                        snprintf(cliActorStr, sizeof(cliActorStr), "%s:%u", cliIpStr, cliPort);
                        snprintf(buffLogs, MSGMLEN, "%s has disconnected.\n", cliActorStr);
                        writeft(logFd, buffLogs, cliActorStr);

                        memset(&pFds[i], 0, sizeof(pFds[i]));
                        delete_b_sock(pCurUser);
                        curOnline--;
                        close(connFd);
                        break;
                    default:
                        buffMsg[MSGMLEN - 1] = '\0';    
                        snprintf(cliActorStr, sizeof(cliActorStr), "%s:%u", cliIpStr, cliPort);
                        writeft(logFd, buffMsg, cliActorStr);
                        break;
                    }
                    
                }
            } 
        }

    }




}
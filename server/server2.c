/* Version from: 11.12.2025 */

#include "s_aux.h"



int main(int argc, char **argv) {

    int listenFd, connFd;
    socklen_t addrSize;
    struct sockaddr_in servAddr, cliAddr;

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

    

    struct pollfd pFds[CLIENTCAP + 1];                                   /* set up poll() */
    memset(&pFds, '\0', sizeof(pFds));
    int rv0;


    if ((listenFd = getlsocket(inputServIp, inputServPort)) == -1) {        /* initialize the listening socket*/
        exit(EXIT_FAILURE);
    }                  


    pFds[0].fd = listenFd;
    pFds[0].events = POLLIN | POLLPRI;
    int curOnline = 0;


    int flags1 = fcntl(listenFd, F_GETFL, 0);
    if (fcntl(listenFd, F_SETFL, flags1 | O_NONBLOCK) == -1) {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }

    b_socket curUsers[CLIENTCAP];

    for ( ; ; ) {
        int pollResult = poll(pFds, curOnline + 1, 500);\
//        printf("pollResult: %d, curOnline: %d, curOnline fd: %d\n", pollResult, curOnline, pFds[curOnline].fd);
        if (pollResult == -1) {
            perror("poll error\n");
//            exit(EXIT_FAILURE);
        }
        else if (pollResult >= 0) {
            if (pFds[0].revents & POLLIN) {                                                     /* check the listening socket for new connections */

                char cliIpStr[INET_ADDRSTRLEN];
                uint16_t cliPort;
                char buffAux[64];


                connFd = accept(listenFd, (struct sockaddr *) &cliAddr, &addrSize);
                curOnline++;
                curUsers[curOnline] = (b_socket) {.fd = connFd, .addr = cliAddr};
                pFds[curOnline].fd = connFd;
                pFds[0].events = POLLIN | POLLPRI;
                flags1 = fcntl(connFd, F_GETFL, 0);
                if (fcntl(connFd, F_SETFL, flags1 | O_NONBLOCK) == -1) {
                    perror("fcntl");
                    exit(EXIT_FAILURE);
                }
                printf("connFd: %i\n", connFd);
                
                inet_ntop(AF_INET, (struct sockaddr *) &curUsers[curOnline].addr.sin_addr.s_addr, cliIpStr, INET_ADDRSTRLEN);
                snprintf(buffLogs, MSGMLEN, "Accepted a connection from: %s:%u. Socket: %i.\n", cliIpStr, ntohs(curUsers[curOnline].addr.sin_port), connFd); //change to strcliip cliport
                writeft(logFd, buffLogs, inputServIp);

            }


            for (int i = 1; i < CLIENTCAP; i++)                                                 /* check the sockets for incoming messages */   
            {

                if (pFds[i].fd > 0) //  && pFds[i].revents & POLLIN
                {

                    char buffMsg[MSGMLEN];
                    char cliIpStr[INET_ADDRSTRLEN];
                    char cliActorStr[32];
                    int cliFd = pFds[i].fd;
                    uint16_t cliPort = 0;

                    for (int j = 0; j < CLIENTCAP + 1; j++) {                          /* having our fd, we now go through the curUser */ 
                        if (cliFd == curUsers[j].fd) {                                 /* array to resolve our sockaddr_in. */
                            inet_ntop(AF_INET, (struct sockaddr *) &curUsers[j].addr.sin_addr.s_addr, cliIpStr, INET_ADDRSTRLEN);
                            cliPort = ntohs(curUsers[curOnline].addr.sin_port);
                            break;
                        }
                    }

                     
                    

                    int n = read(pFds[i].fd, buffMsg, MSGMLEN);
                    
                    switch (n)
                    {
                    case (-1):
                        break;
                    case (0):
                        snprintf(cliActorStr, sizeof(cliActorStr), "%s:%u", cliIpStr, cliPort);
                        snprintf(buffLogs, MSGMLEN, "%s has disconnected.\n", cliActorStr);
                        writeft(logFd, buffLogs, cliActorStr);
                        pFds[i].fd = 0;
                        pFds[i].events = 0;
                        pFds[i].revents = 0;
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
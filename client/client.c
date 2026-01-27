/* Version from Dec 4, 2025 */

#include "c_aux.h"

int main(int argc, char **argv) {

    int listenFd, connectFd;
    struct sockaddr_in servAddr;
    char buffMsg[MSGMLEN];
    memset(buffMsg, 0, MSGMLEN);
    char buffPrc[PARCELMLEN];
    memset(buffPrc, 0, sizeof(buffPrc));
    int tmpFlags;
    char buffLogs[MSGMLEN];


    const int clientCmdPoolSize = 3;
    const char *clientCmdPool[3] = {"/quit\n", "/reconnect\n", "/disconnect\n"};

    int logFd;
    logFd = open("clilog1.txt", O_WRONLY | O_APPEND | O_CREAT | O_TRUNC, 0644);
    assert(logFd > 0);
    snprintf(buffLogs, MSGMLEN, "Logs accessed. Path: ./clilog1.txt\n");            /* logs */
    writeft(logFd, buffLogs, "SERVER");

    char *inputServIp = "127.0.0.1";
    char *cliIpStr = resolve_my_ip_address();
    uint16_t inputServPort = 9877;
    snprintf(buffLogs, MSGMLEN, "Starting. argc = %d.\n", argc);                /* logs */
    writeft(logFd, buffLogs, cliIpStr);

    if(argc == 2) {
        inputServIp = argv[1];
        //inputServPort = *argv[2];
    }
    else
    {
        printf("Usage: %s <ip_address> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* if (argc == 4) {
        if (hash_sdbm(argv[2]) == hash_sdbm("-test")) {
            isTester = 1;
            printf("Client is in tester mode.\n");
        }
    } */

    fflush(stdout);
    fflush(stdin);
    setvbuf(stdout, NULL, _IONBF, 0);
    startup_text();
    printf("Client IP addr:   %s\n", cliIpStr);

    

    memset(&servAddr, '\0', sizeof(servAddr));                                                  /* getting servers actual IP address*/
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(inputServPort);
    if (inet_pton(AF_INET, inputServIp, (struct sockaddr *) &servAddr.sin_addr.s_addr) <= 0) {
        //printf("%s", strerror(errno));
        writeft(logFd, "inet_pton fuckup?\n", "client");
        exit(EXIT_FAILURE);
    }

                                                                                        /* polls */
    struct pollfd otherFds[2];                                                  
    memset(&otherFds, 0, sizeof(otherFds));


    uint8_t otherCount = 0;
    /* creating a connecting socket. i == 0 */
    connectFd = socket(AF_INET, SOCK_STREAM, 0);
    snprintf(buffLogs, MSGMLEN - 1, "Created socket for connection, connectFd=%d\n", connectFd);
    writeft(logFd, buffLogs, cliIpStr);
    /*if (connectFd = socket(AF_INET, SOCK_STREAM, 0) < 0) {
        printf("main(): error creating the connectFd socket.\n");
        exit(EXIT_FAILURE);
    }*/
    int connectAssert = connect(connectFd, (struct sockaddr *) &servAddr, sizeof(servAddr));
    if (connectAssert < 0) {
        printf("main(): error trying to connect with connectFd.\n");
        exit(EXIT_FAILURE);
    }
    tmpFlags = fcntl(connectFd, F_GETFL, 0);
    if (fcntl(connectFd, F_SETFL, tmpFlags | O_NONBLOCK) == -1) {
        write(2, "fcntl failed\n", 14);
        exit(EXIT_FAILURE);
    }
    otherFds[0].fd = connectFd;
    otherFds[0].events = POLLIN | POLLPRI;
    bool isConnected = true;
    otherCount++;


    /* fd for reading from getline(). i == 1 */
    otherFds[1].fd = 0;
    otherFds[1].events = POLLIN | POLLPRI;
    otherCount++;

    
    if (connectFd > 0) {
        printf("Connected to:     %s:%u\n", inputServIp, inputServPort);
    }
    printf("Log path:         %s\n\n", "clilog1.txt");
    

    char *hello = "Hello. I am a client.\n";
    int n2 = sizeof(buffMsg);
    write(connectFd, hello, 23);
    

    time_t timerLastDisconnectMsg = time(NULL);
    int intervalDisconnectMsg = 30;
    int commandHandlerReturn;

    for ( ; ; ) {

        time_t timerCurrent = time(NULL);
        commandHandlerReturn = -1;
        
        
        for (int i = 0; i < sizeof(otherFds)/sizeof(otherFds[0]); i++)
        {
            otherFds[i].revents = 0;
        }


        int otherFdsResult = poll(otherFds, otherCount, 20);                                 /* polls cooking up */
                                                                                                /* otherFds poll */
        if (otherFdsResult < 0) {
            ;
        }
        else if (otherFdsResult == 0) {
            snprintf(buffLogs, MSGMLEN - 1, "otherFdsResult = 0.\n");                /* logs */
            writeft(logFd, buffLogs, cliIpStr);
        }
        else if (otherFdsResult > 0) {

            for (int i = 0; i < sizeof(otherFds)/sizeof(otherFds[0]); i++) {

                if (i == 0 && otherFds[0].revents & POLLIN) {

                    memset(buffPrc, 0, PARCELMLEN);
                    int n0 = read(otherFds[i].fd, buffPrc, PARCELMLEN);

                    snprintf(buffLogs, MSGMLEN - 1, "Have read a message of %d bytes from the server.\n", n0);         /* logs */
                    writeft(logFd, buffLogs, cliIpStr);

                    if (n0 == -1) {
                        ;
                    }
                    else if (n0 == 0) {
                        snprintf(buffLogs, MSGMLEN - 1, "0 byte message received, closing the connected socket, connectFd = %d.\n", connectFd);         /* logs */
                        writeft(logFd, buffLogs, cliIpStr);
                        close(connectFd);
                        isConnected = false;
                        
                    }
                    else if (n0 > 0) {

                        char *recvAuthor = malloc(AUTHORMLEN);
                        memset(recvAuthor, 0, AUTHORMLEN);
                        char *recvPayload = malloc(MSGMLEN);
                        memset(recvPayload, 0, MSGMLEN);

                        anm_deconstruct_msg(buffPrc, recvAuthor, recvPayload);
                        writeft(logFd, recvPayload, recvAuthor);
                        write_to_user(1, recvPayload, recvAuthor);
                        free(recvAuthor);
                        free(recvPayload);

                        
                    }

                    
                }

                if (i == 1 && otherFds[i].revents & POLLIN) {

                    memset(buffMsg, 0, MSGMLEN);
                    int readnlBytes = readnl(otherFds[i].fd, buffMsg, MSGMLEN);
                    
                    if (readnlBytes > 0) {
                        char *buffReadnl = malloc(readnlBytes);
                        strncpy(buffReadnl, buffMsg, readnlBytes);

                        snprintf(buffLogs, MSGMLEN-1, "Have read %d bytes from the terminal.\n", readnlBytes);          /* logs */
                        writeft(logFd, buffLogs, cliIpStr);

                        
                        for(int r = 0; r < clientCmdPoolSize; r++) { // check if any commands have been input in the terminal
                            if (hash_sdbm(buffReadnl) == hash_sdbm(clientCmdPool[r])) {
                                commandHandlerReturn = r;
                                break;
                            }
                        }
                        if (commandHandlerReturn == -1) {
                            write(connectFd, buffReadnl, readnlBytes);

                            snprintf(buffLogs, MSGMLEN - 1, "Wrote %d bytes to the server via connectFd=%d.\n", readnlBytes, connectFd);          /* logs */
                            writeft(logFd, buffLogs, cliIpStr);

                            free(buffReadnl);
                        }   
                    }
                }
            }
        }

        /* the Command Handler */

        if (isConnected == false && (timerCurrent - timerLastDisconnectMsg) > intervalDisconnectMsg) {
            timerLastDisconnectMsg = timerCurrent;
            printf("Looks like you have (been) disconnected. /reconnect to reconnect.\n");
        }   
        switch (commandHandlerReturn)
        {
            case 0:                         /* /quit */

                close(connectFd);
                writeft(logFd, "getline exit\n", cliIpStr);
                exit(EXIT_SUCCESS);

            case 1:                         /* /reconnect */
                
                if (isConnected == true) {
                    printf("Already connected!\n");
                    snprintf(buffLogs, MSGMLEN - 1, "Handler received a /reconnect command, but the client seems to be connected.\n");          /* logs */
                    writeft(logFd, buffLogs, cliIpStr);
                    break;
                }
                connectFd = socket(AF_INET, SOCK_STREAM, 0);
                printf("New connectFd is: %d\n", connectFd);
                if (connectFd == -1) {
                    printf("Failed to create a socket to reconnect.\n");
                    snprintf(buffLogs, MSGMLEN - 1, "Failed to create a socket to reconnect to: %s:%d.\n", inputServIp, inputServPort);          /* logs */
                    writeft(logFd, buffLogs, cliIpStr);
                }
                int connectAssert2 = connect(connectFd, (struct sockaddr *) &servAddr, sizeof(servAddr));
                if (connectAssert2 < 0) {
                    printf("Tried to reconnect but failed.\n");
                    snprintf(buffLogs, MSGMLEN - 1, "Failed to reconnect to: %s:%d.\n", inputServIp, inputServPort);          /* logs */
                    writeft(logFd, buffLogs, cliIpStr);
                }
                else {
                    printf("Reconnected!");
                    isConnected = true;
                }
            case 2:

                close(connectFd);
                isConnected = false;

            default:
                ;
        }
        /*if (isTester == 1) {
            if ((timerCurrent - t_last) > interval) {
                    
                    t_last = timerCurrent;

                    int max = 180;
                    int min = 60;
                    interval = ((double) rand() / (RAND_MAX)) * (max-min+1) + min;
                    memset(buffMsg, 0, MSGMLEN);
                    strncpy(buffMsg, "Client message. Yeah?\n", 23);
                    write(connectFd, buffMsg, MSGMLEN);
            }
        } 
        */
    }


        


}
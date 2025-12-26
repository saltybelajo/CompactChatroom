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

    char *inputServIp = "127.0.0.1";
    char *cliIpStr = resolve_my_ip_address();
    uint16_t inputServPort = 9877;

    char buffLogs[MSGMLEN];

    if(argc < 2) {
        //printf("Usage: %s <IPaddress>.\nServer IP address set to localhost by default.\n", argv[0]);
    }
    else
    {
        inputServIp = argv[1];
    }

    if (argc == 3) {
        if (hash_sdbm(argv[2]) == hash_sdbm("-test")) {
            isTester = 1;
            printf("Client is in tester mode.\n");
        }
    }

    fflush(stdout);
    setvbuf(stdout, NULL, _IONBF, 0);
    startup_text();
    printf("Client IP addr:   %s\n", cliIpStr);

    int logFd;
    logFd = open("clilog1.txt", O_WRONLY | O_APPEND | O_CREAT | O_TRUNC, 0644);
    snprintf(buffLogs, MSGMLEN, "Logs accessed. Path: ./clilog1.txt\n");
    writeft(logFd, buffLogs, cliIpStr);

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
    /*if (connectFd = socket(AF_INET, SOCK_STREAM, 0) < 0) {
        printf("main(): error creating the connectFd socket.\n");
        exit(EXIT_FAILURE);
    }*/
    int m1 = connect(connectFd, (struct sockaddr *) &servAddr, sizeof(servAddr));
    if (m1 < 0) {
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
    isConnected = 1;
    otherCount++;


    /* fd for reading from getline(). i == 1 */
    size_t getlineSize;
    ssize_t nread;
    char *inputLine;
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
    

    time_t t_last = time(NULL);
    int interval = 120;
    for ( ; ; ) {
        time_t t_cur = time(NULL);
        
        
        for (int i = 0; i < sizeof(otherFds)/sizeof(otherFds[0]); i++)
        {
            otherFds[i].revents = 0;
        }


        int otherFdsResult = poll(otherFds, otherCount, 20);                                 /* polls cooking up */
                                                                                                /* otherFds poll */
        if (otherFdsResult < 0) {

        }
        else if (otherFdsResult == 0) {
            // nothing has happened
        }
        else if (otherFdsResult > 0) {

            for (int i = 0; i < sizeof(otherFds)/sizeof(otherFds[0]); i++) {

                if (i == 0 && otherFds[0].revents & POLLIN) {

                    memset(buffPrc, 0, PARCELMLEN);
                    int n = read(otherFds[i].fd, buffPrc, PARCELMLEN);
                    if (n > 0) {

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

                if (i == 1 && otherFds[0].revents & POLLIN) {
                    inputLine = NULL;
                    getlineSize = 0;
                    if (nread = getline(&inputLine, &getlineSize, stdin) > 0) {
                        if (hash_sdbm(inputLine) == hash_sdbm("/quit\n")) {
                            free(inputLine);
                            close(connectFd);
                            writeft(logFd, "getline exit\n", "client");
                            exit(EXIT_SUCCESS);
                    
                        }
                        else {
                            write(connectFd, inputLine, nread);
                        }   
                    }
                }
            }
        }
        /*if (isTester == 1) {
            if ((t_cur - t_last) > interval) {
                    
                    t_last = t_cur;

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
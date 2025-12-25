/* Version from Dec 4, 2025 */

#include "c_aux.h"

int main(int argc, char **argv) {

    int listenFd, connectFd;
    struct sockaddr_in servAddr;

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
            isClientTester = 1;
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
    struct pollfd readServFds[1];                                                  
    memset(&readServFds, 0, sizeof(readServFds));

    struct pollfd readGetlineFds[1];
    memset(&readGetlineFds, 0, sizeof(readGetlineFds));

    connectFd = socket(AF_INET, SOCK_STREAM, 0);

    char *c0 = malloc(64);
    memset(c0, 0, 64);
    c0[0] = '?';
    c0[1] = '\n';
                                                                                        /* connecting to the server */
    char buffMsg[MSGMLEN];
    memset(buffMsg, 0, MSGMLEN);
    char buffPrc[PARCELMLEN];
    connect(connectFd, (struct sockaddr *) &servAddr, sizeof(servAddr)); 
    int tmpFlags = fcntl(connectFd, F_GETFL, 0);
    if (fcntl(connectFd, F_SETFL, tmpFlags | O_NONBLOCK) == -1) {
        write(2, "fcntl failed\n", 14);
        exit(EXIT_FAILURE);
    }
    readServFds[0].fd = connectFd;
    readServFds[0].events = POLLIN | POLLPRI;

    if (connectFd > 0) {
        strncpy(c0, inputServIp, INET_ADDRSTRLEN);
        printf("Connected to:     %s:%u\n", c0, inputServPort);
    }
    printf("Log path:         %s\n\n", "clilog1.txt");
    
    
    //snprintf(buffMsg, sizeof(buffMsg), "Hello! I am a client.\n");
    char *hello = "Hello. I am a client.\n";
    int n2 = sizeof(buffMsg);
    write(connectFd, hello, 23);
    //printf("Connected to: %s:%u\n", inputServIp, inputServPort);
    //printf("Type /quit to shut down.\n");
    

                                                                                        /* the filestream n shi for Getline() */
    size_t size;
    ssize_t nread;
    char *inputLine;
    readGetlineFds[0].fd = 0;
    readGetlineFds[0].events = POLLIN | POLLPRI;

    time_t t_last = time(NULL);
    int interval = 120;
    for ( ; ; ) {

        time_t t_cur = time(NULL);
        inputLine = NULL;
        size = 0;
        
        readServFds[0].revents = 0;                                                         /* set .revents everywhere to 0 */
        readGetlineFds[0].revents = 0;


        int readServFdsResult = poll(readServFds, 1, 20);                                 /* polls cooking up */
        int readGetlineFdsResult = poll(readGetlineFds, 1, 20);


        if (readGetlineFdsResult < 0) {                                                     /* readGetlineFds poll */
            write(2, "readGetlineFds poll error!\n", 28);
        }
        else if (readGetlineFdsResult == 0) {

        }
        else if (readGetlineFdsResult > 0) {

            if ((nread = getline(&inputLine, &size, stdin)) > 0) {
                if (hash_sdbm(inputLine) == hash_sdbm("/quit\n")) {
                    free(inputLine);
                    close(connectFd);
                    writeft(logFd, "getline exit\n", "client");
                    exit(EXIT_SUCCESS);
                    write(connectFd, inputLine, nread);
                }
                    
            }

            
            
        }
                                                                                                /* readServFds poll */
        if (readServFdsResult < 0) {

        }
        else if (readServFdsResult == 0) {

        }
        else if (readServFdsResult > 0) {
            if (readServFds[0].fd > 0 && readServFds[0].revents & POLLIN) {

                memset(buffPrc, 0, PARCELMLEN);
                int n = read(readServFds[0].fd, buffPrc, PARCELMLEN);
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
        }
        if (isClientTester == 1) {
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
        
    }


        


}
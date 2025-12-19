/* Version from Dec 4, 2025 */

#include "c_aux.h"

int main(int argc, char **argv) {

    int listenFd, connectFd;
    struct sockaddr_in servAddr;

    char *inputServIp = "127.0.0.1";
    char *cliIpStr = "192.168.0.19";
    uint16_t inputServPort = 9877;

    char buffLogs[MSGMLEN];

    if(argc < 2) {
        //printf("Usage: %s <IPaddress>.\nServer IP address set to localhost by default.\n", argv[0]);
    }
    else
    {
        inputServIp = argv[1];
    }

    fflush(stdout);
    setvbuf(stdout, NULL, _IONBF, 0);

    int logFd;
    logFd = open("clilog1.txt", O_WRONLY | O_APPEND | O_CREAT | O_TRUNC, 0644);

    snprintf(buffLogs, MSGMLEN, "Logs accessed.\n");
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
    
    
    snprintf(buffMsg, sizeof(buffMsg), "Hello! I am a client.\n", cliIpStr);
    int n2 = sizeof(buffMsg);
    write(connectFd, buffMsg, n2);
    //printf("Connected to: %s:%u\n", inputServIp, inputServPort);
    //printf("Type /quit to shut down.\n");
    

                                                                                        /* the filestream n shi for Getline() */
    size_t size;
    ssize_t nread;
    char *inputLine;
    readGetlineFds[0].fd = 0;
    readGetlineFds[0].events = POLLIN | POLLPRI;

    for ( ; ; ) {

        inputLine = NULL;
        size = 0;
        
        readServFds[0].revents = 0;                                                         /* set .revents everywhere to 0 */
        readGetlineFds[0].revents = 0;


        int readServFdsResult = poll(readServFds, 1, 200);                                 /* polls cooking up */
        int readGetlineFdsResult = poll(readGetlineFds, 1, 200);


        if (readGetlineFdsResult < 0) {                                                     /* readGetlineFds poll */
            write(2, "readGetlineFds poll error!\n", 28);
        }
        else if (readGetlineFdsResult == 0) {

        }
        else if (readGetlineFdsResult > 0) {
            if ((nread = getline(&inputLine, &size, stdin)) != -1) {
                if (sdbm(inputLine) == sdbm("/quit\n")) {
                    free(inputLine);
                    close(connectFd);
                    writeft(logFd, "getline exit\n", "client");
                    exit(EXIT_SUCCESS);
                }
                snprintf(buffMsg, sizeof(buffMsg), "%s", inputLine);
                write(connectFd, buffMsg, MSGMLEN);
            }
        }
                                                                                                /* readServFds poll */
        if (readServFdsResult < 0) {

        }
        else if (readServFdsResult == 0) {

        }
        else if (readServFdsResult > 0) {
            if (readServFds[0].fd > 0 && readServFds[0].revents & POLLIN) {
                int n = read(readServFds[0].fd, buffPrc, PARCELMLEN);
                if (n > 0) {
                    char *recvAuthor = malloc(AUTHORMLEN);
                    char *recvPayload = malloc(MSGMLEN);

                    anm_deconstruct_msg(buffPrc, recvAuthor, recvPayload);
                    writeft(logFd, recvPayload, recvAuthor);
                    free(recvAuthor);
                    free(recvPayload);
                    //writeft()
                }
                
            }
        }

        
    }


        


}
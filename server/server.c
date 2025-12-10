/* Version from: 01.12.2025 */

#include "s_aux.h"

int main(int argc, char **argv) {

    int listen_fd, conn_fd;
    pid_t child_pid;
    socklen_t addr_size;
    struct sockaddr_in cli_addr, serv_addr;
    setvbuf(stdout, NULL, _IONBF, 0);
    

    
    fflush(stdout);

    char *temp0 = malloc(MSGMLEN);
    snprintf(temp0, MSGMLEN, "echo "" > %s", SERVLOGPATH);
    system(temp0);
    free(temp0); 
    int log_fd0;
    log_fd0 = open(SERVLOGPATH, O_WRONLY | O_APPEND | O_CREAT, 0644);

    
    

    char *input_serv_ip = "192.168.0.18";
    unsigned short input_serv_port = 9877;
    writeft(log_fd0, "Starting...\n", input_serv_ip);


    
    


    char msgbuffer[MSGMLEN];

    snprintf(msgbuffer, MSGMLEN, "Server's address: %s:%u.\n", input_serv_ip, input_serv_port);
    writeft(log_fd0, msgbuffer, input_serv_ip);

    
    

    //writeft(log_fd0, "Log file accessed.\n", input_serv_ip);
    


    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("[-] Failure creating a socket.\n");
        exit(EXIT_FAILURE);
    } 


    memset(&serv_addr, '\0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(input_serv_port);
    int s = inet_pton(AF_INET, input_serv_ip, (struct sockaddr *) &serv_addr.sin_addr.s_addr);
    if (s <= 0) {
        if(s == 0)
            printf("Bad input IPv4 address.\n");
        else
            perror("inet_pton");
        exit(EXIT_FAILURE);
    }
    


    /* bind server address to the server sock */
    addr_size = sizeof(serv_addr);
    int n = bind(listen_fd, (struct sockaddr *) &serv_addr, addr_size);
    if (n == -1) {
        printf("%s", strerror(errno));
        exit(EXIT_FAILURE);
    } 

    int pipetopar_fd[2];
    pipe(pipetopar_fd); //creates and opens 2 file descriptors!
    int flags0 = fcntl(pipetopar_fd[0], F_GETFL, 0);
    if (fcntl(pipetopar_fd[0], F_SETFL, flags0 | O_NONBLOCK) == -1) {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }
    

    char *temp1 = malloc(MSGMLEN);
    snprintf(temp1, MSGMLEN, "echo "" > %s", SERVBUFPATH);
    system(temp1);
    free(temp1);

    int buffer_fd;
    buffer_fd = open(SERVBUFPATH, O_WRONLY | O_APPEND | O_CREAT, 0644);
    int flags1 = fcntl(buffer_fd, F_GETFL, 0);
    if (fcntl(buffer_fd, F_SETFL, flags1 | O_NONBLOCK) == -1) {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }

    int flags2 = fcntl(listen_fd, F_GETFL, 0);
    if (fcntl(listen_fd, F_SETFL, flags1 | O_NONBLOCK) == -1) {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }



    char msgstr0[MSGMLEN];
    char bufstr0[MSGMLEN];


    snprintf(msgstr0, MSGMLEN, "Setting the socket to listening state. Listening Port: %u.\n", input_serv_port);
    writeft(log_fd0, msgstr0, input_serv_ip);

    for ( ; ; ) {

        int m1 = read(pipetopar_fd[0], bufstr0, MSGMLEN);
        if (m1 > 0) {
            writeft(log_fd0, bufstr0, input_serv_ip);
        }

        listen(listen_fd, 1024);
        
        
        memset(&cli_addr, '\0', sizeof(cli_addr));
        conn_fd = accept(listen_fd, (struct sockaddr *) &cli_addr, &addr_size);



/*      n2 = read(pipetopar_fd[0], bufstr0, MSGMLEN);
        printf("%i", n2);
        if (n2 > 0) {
            write(buffer_fd, bufstr0, MSGMLEN);
            memset(bufstr0, '\0', MSGMLEN);
        } */

        char str_cli_ip[64];
        uint16_t resv_cli_port;
        memset(str_cli_ip, '\0', sizeof(str_cli_ip));
        
        
        if (conn_fd >= 0) {
            
            if(inet_ntop(AF_INET, (struct sockaddr *) &cli_addr.sin_addr.s_addr, str_cli_ip, sizeof(str_cli_ip)) == NULL)
            {
                perror("inet_ntop");
                exit(EXIT_FAILURE);
            }

            resv_cli_port = ntohs(cli_addr.sin_port) ;
            
            snprintf(msgstr0, MSGMLEN, "%s:%u has joined.\n", str_cli_ip, resv_cli_port);
            writeft(log_fd0, msgstr0, input_serv_ip);


            child_pid = fork();

        }
        else {

            struct timespec ts;

            ts.tv_sec = 0;
            ts.tv_nsec = 5000;
            nanosleep(&ts, &ts);
            continue;
            
        }

        
        

        
        

        if (child_pid == 0) {
            // child process. fd available: conn_fd, pipetopar_fd[1], pipetochi_fd[0]
            close(listen_fd);
            close(pipetopar_fd[0]);
            // close(log_fd0);
            close(buffer_fd);

            char str_cli_ip_child[64];
            strcpy(str_cli_ip_child, str_cli_ip); 
            uint16_t child_resv_cli_port = resv_cli_port;


            char msgstr1[MSGMLEN];
            pid_t mypid = getpid();

            
            snprintf(msgstr1, MSGMLEN, "Child process created. PID: %i. Handles the connection to: %s:%u.\n", mypid, str_cli_ip_child, child_resv_cli_port);
            write(pipetopar_fd[1], msgstr1, MSGMLEN);

            /* snprintf(msgstr1, MSGMLEN, "My PID is %i. I will wait 15 seconds, then terminate.\n", mypid);
            writeft(log_fd0, msgstr1, input_serv_ip);

            sleep(15);

            
             */
            char *get_msg_from_cli = malloc(MSGMLEN);

            do {

                memset(get_msg_from_cli, '\0', MSGMLEN);
                read(conn_fd, get_msg_from_cli, MSGMLEN);
                write(pipetopar_fd[1], get_msg_from_cli, MSGMLEN);
                
            } while(*get_msg_from_cli != '\0');
            
            snprintf(msgstr1, MSGMLEN, "Terminating the child process with PID: %i.\n", mypid);
            writeft(log_fd0, msgstr1, input_serv_ip);

            snprintf(msgstr1, MSGMLEN, "%s:%u has left.\n", str_cli_ip_child, child_resv_cli_port);
            writeft(log_fd0, msgstr1, input_serv_ip);

            free(get_msg_from_cli);


            exit(EXIT_SUCCESS);

        }
        else {
            close(conn_fd);
        }

        
        

        //writeft(log_fd0, "Connection is lost. Shutting down.\n", input_serv_ip);
        //close(log_fd0);
        //exit(EXIT_SUCCESS);
    
    }
    
}
/* Version from: 01.12.2025 */

#include "s_aux.h"

int main(int argc, char **argv) {

    int listen_fd, conn_fd;
    pid_t child_pid;
    socklen_t addr_size;
    struct sockaddr_in cli_addr, serv_addr;
    int pipe0_fds[2];
    setvbuf(stdout, NULL, _IONBF, 0);
    

    
    fflush(stdout);

    FILE *f0;
    f0 = fopen("logs3.txt", "w");
    fclose(f0);

    int log_fd0;
    log_fd0 = open("logs3.txt", O_WRONLY | O_APPEND | O_CREAT, 0644);

    
    
    

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

    pipe(pipe0_fds); //creates and opens 2 file descriptors!

    

    for ( ; ; ) {

        char msgstr0[MSGMLEN];
        listen(listen_fd, 1024);
        snprintf(msgstr0, MSGMLEN, "Socket set to listening state. Listening Port: %u.\n", input_serv_port);
        writeft(log_fd0, msgstr0, input_serv_ip);
        memset(&cli_addr, '\0', sizeof(cli_addr));
        conn_fd = accept(listen_fd, (struct sockaddr *) &cli_addr, &addr_size);
        
        

        char str_cli_ip[64];
        memset(str_cli_ip, '\0', sizeof(str_cli_ip));
        if(inet_ntop(AF_INET, (struct sockaddr *) &cli_addr.sin_addr.s_addr, str_cli_ip, sizeof(str_cli_ip)) == NULL)
        {
            perror("inet_ntop");
            exit(EXIT_FAILURE);
        }

        uint16_t resv_cli_port = ntohs(cli_addr.sin_port) ;
        
        snprintf(msgstr0, MSGMLEN, "Connection accepted from: %s:%u.\n", str_cli_ip, resv_cli_port);
        writeft(log_fd0, msgstr0, input_serv_ip);

        child_pid = fork();

        if (child_pid == 0) {
            // child process. fd's needed: conn_fd for the client, the write end of the pipe to talk to the parent, log fd(?)
            close(listen_fd);
            close(pipe0_fds[0]);

            char str_cli_ip_child[64];
            strcpy(str_cli_ip_child, str_cli_ip); 
            uint16_t child_resv_cli_port = resv_cli_port;

            char msgstr1[MSGMLEN];
            pid_t mypid = getpid();

            
            snprintf(msgstr1, MSGMLEN, "Child process created. PID: %i. Handles the connection to: %s:%u.\n", mypid, str_cli_ip_child, child_resv_cli_port);
            writeft(log_fd0, msgstr1, input_serv_ip);

            /* snprintf(msgstr1, MSGMLEN, "My PID is %i. I will wait 15 seconds, then terminate.\n", mypid);
            writeft(log_fd0, msgstr1, input_serv_ip);

            sleep(15);

            
             */
            char *get_msg_from_cli = malloc(MSGMLEN);

            do {

                memset(get_msg_from_cli, '\0', MSGMLEN);
                read(conn_fd, get_msg_from_cli, MSGMLEN);
                writeft(log_fd0, get_msg_from_cli, str_cli_ip_child);
                
            } while(*get_msg_from_cli != '\0');
            
            snprintf(msgstr1, MSGMLEN, "Terminating the child process with PID: %i.\n", mypid);
            writeft(log_fd0, msgstr1, input_serv_ip);
            
            exit(EXIT_SUCCESS);
        }
        else {
            close(conn_fd);
        }
        sleep(1);

        
        

        //writeft(log_fd0, "Connection is lost. Shutting down.\n", input_serv_ip);
        //close(log_fd0);
        //exit(EXIT_SUCCESS);
    
    }
    
}
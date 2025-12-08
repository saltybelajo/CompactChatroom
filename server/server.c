/* Version from: 01.12.2025 */

#include "s_aux_v01_01.h"

int main(int argc, char **argv)
{
    int listen_fd, estab_fd;
    pid_t child_pid;
    socklen_t addr_size;
    struct sockaddr_in cli_addr, serv_addr;
    setvbuf(stdout, NULL, _IONBF, 0);


    char *input_serv_ip = "192.168.0.18";
    unsigned short input_serv_port = 9877;


    char msgbuffer[MSGMLEN];

    FILE *f0;
    f0 = fopen("logs3.txt", "w");
    fclose(f0);

    int log_fd0;
    log_fd0 = open("logs3.txt", O_WRONLY | O_APPEND | O_CREAT, 0644);


    //writeft(log_fd0, "Log file accessed.\n", input_serv_ip);
    


    if((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("[-] Failure creating a socket.");
        exit(EXIT_FAILURE);
    } 


    memset(&serv_addr, '\0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(input_serv_port);
    int s = inet_pton(AF_INET, input_serv_ip, (struct sockaddr *) &serv_addr.sin_addr.s_addr);
    if(s <= 0)
    {
        if(s == 0)
            printf("Bad input IPv4 address.");
        else
            perror("inet_pton");
        exit(EXIT_FAILURE);
    }
    


    /* bind server address to the server sock */
    addr_size = sizeof(serv_addr);
    int n = bind(listen_fd, (struct sockaddr *) &serv_addr, addr_size);
    if(n == -1)
    {
        printf("%s", strerror(errno));
        exit(EXIT_FAILURE);
    } 


    for( ; ; )
    {
        char msgstr[MSGMLEN];
        listen(listen_fd, 1024);
        snprintf(msgstr, MSGMLEN, "Socket set to listening state. Listening port: %u.\n", input_serv_port);
        writeft(log_fd0, msgstr, input_serv_ip);

        memset(&cli_addr, '\0', sizeof(cli_addr));
        estab_fd = accept(listen_fd, (struct sockaddr *) &cli_addr, &addr_size);
        close(listen_fd);
        

        char str_cli_ip[64];
        memset(str_cli_ip, '\0', sizeof(str_cli_ip));
        if(inet_ntop(AF_INET, (struct sockaddr *) &cli_addr.sin_addr.s_addr, str_cli_ip, sizeof(str_cli_ip)) == NULL)
        {
            perror("inet_ntop");
            exit(EXIT_FAILURE);
        }

        uint16_t uint_cli_port = ntohs(cli_addr.sin_port) ;
        
        snprintf(msgstr, MSGMLEN, "Connection accepted from: %s; Inbound Port: %u. Closing the listening socket...\n", str_cli_ip, uint_cli_port);
        writeft(log_fd0, msgstr, input_serv_ip);


        

        
        char *str_conncheck = "conncheck";
        unsigned int waitseq = 0;
        int h = 1;
        while(1)
        {
            sleep(3);
            waitseq++;
            h = send(estab_fd, str_conncheck, sizeof(str_conncheck), 0);
            
            snprintf(msgstr, MSGMLEN, "Connected to: %s. send() output: %i.\n", str_cli_ip, h);
            writeft(log_fd0, msgstr, input_serv_ip);

            if (h == -1)
            {
                
                writeft(log_fd0, "Connection is lost. Shutting down.\n", input_serv_ip);
                close(log_fd0);
                exit(EXIT_SUCCESS);
            }
        }

   
    }

}

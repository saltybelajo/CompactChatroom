/* Version from Dec 4, 2025 */

#include "c_aux.h"

int main(int argc, char **argv) {

    int listen_fd, conn_fd;
    struct sockaddr_in serv_addr;
    socklen_t addr_size;

    char *serv_ip_str = "127.0.0.1";

    if(argc < 2) {
        printf("Usage: %s <IPaddress>.\nServer IP address set to localhost by default.\n", argv[0]);
    }
    else
    {
        serv_ip_str = argv[1];
    }


    setvbuf(stdout, NULL, _IONBF, 0);

    char *cl_ip_str = "192.168.0.19";
    uint16_t serv_port = 9877;


    int log_fd0;
    log_fd0 = open("clilog1.txt", O_WRONLY | O_APPEND | O_CREAT | O_TRUNC, 0644);


    conn_fd = socket(AF_INET, SOCK_STREAM, 0);


    memset(&serv_addr, '\0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(serv_port);
    if (inet_pton(AF_INET, serv_ip_str, (struct sockaddr *) &serv_addr.sin_addr.s_addr) <= 0) {
        printf("%s", strerror(errno));
        exit(EXIT_FAILURE);
    }


    char msgbuffer[MSGMLEN];
    connect(conn_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    snprintf(msgbuffer, sizeof(msgbuffer), "Hello! I am %s.\n", cl_ip_str);
    write(conn_fd, msgbuffer, sizeof(msgbuffer));
    printf("Type /quit to shut down.\n");


    for ( ; ; ) {
        
        FILE *stream;
        char *input_line = NULL;
        size_t size = 0;
        ssize_t nread;


        stream = fdopen(0, "r");
        if (stream == NULL) {

            perror("fopen");
            exit(EXIT_FAILURE);

        }

        if ((nread = getline(&input_line, &size, stream)) != -1) {
            //writeft(log_fd0, input_line, cl_ip_str);
            snprintf(msgbuffer, sizeof(msgbuffer), "%s", input_line);
            write(conn_fd, msgbuffer, MSGMLEN);
        }

//        input_line = truncate(input_line, MSGMLEN);

        if (sdbm(input_line) == sdbm("/quit\n")) {
            free(input_line);
            fclose(stream);
            close(conn_fd);
            exit(EXIT_SUCCESS);
        }

        


        
        

        
        
        
        
        
        
        
        
        
        /* sleep(15);
        snprintf(msgbuffer, sizeof(msgbuffer), "How is the weather today haha? I'm %s by the way.\n", cl_ip_str);
        send(conn_fd, msgbuffer, sizeof(msgbuffer), 0); */
    }


        


}
/* Version from Dec 4, 2025 */

#include "c_aux_v01_01.h"

int main(int argc, char **argv) {

    int conn_fd;
    struct sockaddr_in serv_addr;
    socklen_t addr_size;


    if(argc != 2){
        printf("Usage: tcpcli <IPaddress>");
        exit(EXIT_FAILURE);
    }


    setvbuf(stdout, NULL, _IONBF, 0);


    char *serv_ip_str = "192.168.0.18";
    char *cl_ip_str = "192.168.0.19";
    uint16_t serv_port = 9877;


    FILE *f_odder;
    f_odder = fopen("clilog1.txt", "w");
    fclose(f_odder);
    int log_fd0;
    log_fd0 = open("clilog1.txt", O_WRONLY | O_APPEND | O_CREAT, 0644);


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
    snprintf(msgbuffer, sizeof(msgbuffer), "Hello! I am %s", cl_ip_str);
    write(conn_fd, msgbuffer, sizeof(msgbuffer));


    for ( ; ; ) {
        
    }


        


}
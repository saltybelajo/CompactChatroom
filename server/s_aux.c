/* Version from: 09.12.2025*/

#include "s_aux.h"

b_socket *curUsers = NULL;

int getlsocket(char *ipAddr, uint16_t port) {

    int fd;
    struct sockaddr_in addr;



    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        return(-1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    int s = inet_pton(AF_INET, ipAddr, (struct sockaddr *) &addr.sin_addr.s_addr);
    if (s <= 0) {
        return(-1);
    }

    size_t size = sizeof(addr);
    int n = bind(fd, (struct sockaddr *) &addr, size);
    if (n == -1) {
        return(-1);
    } 

    if ((n = listen(fd, CLIENTCAP)) == -1) {
        return(-1);
    }


    return(fd);




}

void add_b_socket(int fd, struct sockaddr_in addr) {

    b_socket *sock;

    sock = malloc(sizeof *sock);
    sock->fd = fd;
    sock->addr = (struct sockaddr_in ) addr;
    HASH_ADD_INT(curUsers, fd, (b_socket *) sock);

}

b_socket *find_b_socket(int fd) {

    b_socket *sock;

    HASH_FIND_INT(curUsers, &fd, sock);  /* s: output pointer */
    return sock;

}

void delete_b_sock(b_socket *sock) {

    HASH_DEL(curUsers, sock);  /* user: pointer to deletee */
    free(sock);             /* optional; it's up to you! */

}



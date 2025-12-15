/* Version from: 15.12.2025*/

#include "s_aux.h"

b_socket *curUsers = NULL;



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

void delete_b_socket(b_socket *sock) {

    HASH_DEL(curUsers, sock);  /* user: pointer to deletee */
    free(sock);             /* optional; it's up to you! */

}



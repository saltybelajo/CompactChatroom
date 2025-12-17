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


int anm_create_msg(char *allocd, int size_alloc, char *actor, char *msg) {

    /*int actorMaxBits = 5;
    int msgMaxBits = 9;

    if ((strnlen(actor, 32) > pow(2, actorMaxBits) - 1 ) || 
        (strnlen(msg, MSGMLEN) > pow(2, msgMaxBits) - 1) ||
        (*actor == 0)) 
    {
        return(-1);
    }  */

    int actorBits = log2(closest_pow2(strnlen(actor, AUTHORMLEN)));
    int msgBits = log2(closest_pow2(strnlen(msg, MSGMLEN)));

    char *paddedActor = malloc(actorBits);
    memset(paddedActor, 0, sizeof(paddedActor));
    strncpy(paddedActor, actor, sizeof(paddedActor) - 1);
    char *paddedMsg = malloc(msgBits);
    memset(paddedMsg, 0, sizeof(paddedMsg));
    strncpy(paddedMsg, msg, sizeof(paddedMsg) - 1);


    int flagBytesSize = 2;
    int flagBytes[flagBytesSize];                                          
    flagBytes[0] = actorBits;
    flagBytes[1] = msgBits;

    snprintf(allocd, size_alloc, "%d%d%s%s", flagBytes[0], flagBytes[1], paddedActor, paddedMsg);
    return(1);

}



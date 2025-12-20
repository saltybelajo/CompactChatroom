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


int anm_construct_msg(char *allocd, int size_alloc, char *author, char *payload) {

    int authorSize = strnlen(author, AUTHORMLEN) + 1;
    int payloadSize = strnlen(payload, MSGMLEN) + 1;
    if (authorSize > AUTHORMLEN ||                                                  /* check if the sizes are within standards */
        payloadSize > MSGMLEN ||
        *author == 0) 
    {
        return(-1);
    }
                                                                /* the additional bytes are the message size byte, and two \0 */
    uint8_t nOfAdditBytes = 3;                                                          
    uint8_t messageSize = nOfAdditBytes + authorSize + payloadSize;
    if (size_alloc < messageSize) {
        return(-1);
    }

    char *message = malloc(messageSize);
    message[0] = (char) messageSize;
    snprintf(message + 1, messageSize - 1, "%s^%s", author, payload);
    strncpy(allocd, message, size_alloc);
    return(1);

}


int broadcast(struct pollfd *fds, size_t fdsSize, char *parcel, size_t parcelSize) {
    int sendCounter = 0;
    for (int i = 0; i < fdsSize; i++) {
        if (fds[i].fd > 0 && fds[i].revents & POLLOUT) {
            int n0 = write(fds[i].fd, parcel, parcelSize);
            printf("%d bytes message sent.\n", n0);
            sendCounter++;
        }
        
    }
    return sendCounter;
}

void startup_text() {
    char *s0 = "CompactChatroom-Server v1.0 on Linux.\n";
    printf("%s", s0);
    printf("\n");

    uint32_t len0 = strnlen(s0, 1024);
    char *s1 = malloc(len0 - 1);
    memset(s1, '#', len0 - 1);
    printf("%s\n\n", s1);

}



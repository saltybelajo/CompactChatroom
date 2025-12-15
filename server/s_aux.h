#ifndef _S_AUX_H
#define _S_AUX_H


#include "../common/common.h"
#define SERVLOGPATH "../logs/serverlog1.txt"
#define SERVBUFPATH "../buffers/buffer1.txt"
#define CLIENTCAP 16
#include <poll.h>
#define TIMEOUT 3
#include "uthash.h"

int getlsocket(char *ipAddr, uint16_t port);

typedef struct b_socket {
    int fd;
    struct sockaddr_in addr;
    UT_hash_handle hh;
} b_socket ;

extern b_socket *curUsers;

void add_b_socket(int fd, struct sockaddr_in addr);
b_socket *find_b_socket(int fd);
void delete_b_sock(b_socket *sock);

#endif
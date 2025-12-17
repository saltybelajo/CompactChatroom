#ifndef _S_AUX_H
#define _S_AUX_H


#include "../common/common.h"
#define SERVLOGPATH "../logs/serverlog1.txt"
#define SERVBUFPATH "../buffers/buffer1.txt"

#define TIMEOUT 3

typedef struct b_socket {
    int fd;
    struct sockaddr_in addr;
    UT_hash_handle hh;
} b_socket ;

extern b_socket *curUsers;

void add_b_socket(int fd, struct sockaddr_in addr);
b_socket *find_b_socket(int fd);
void delete_b_socket(b_socket *sock);
int anm_create_msg(char *allocd, int size_alloc, char *actor, char *msg);

#endif
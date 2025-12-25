#ifndef _S_AUX_H
#define _S_AUX_H


#include "../common/common.h"
#define SERVLOGPATH "../logs/serverlog1.txt"
#define SERVBUFPATH "../buffers/buffer1.txt"

#define TIMEOUT 3

extern int isServerTester;

typedef struct b_socket {
    int fd;
    struct sockaddr_in addr;
    UT_hash_handle hh;
} b_socket ;

extern b_socket *curUsers;

void add_b_socket(int fd, struct sockaddr_in addr);

b_socket *find_b_socket(int fd);

void delete_b_socket(b_socket *sock);

int anm_construct_msg(char *allocd, int size_alloc, char *author, char *payload);

int broadcast(struct pollfd *fds, size_t fdsSize, char *parcel, size_t parcelSize);

void startup_text();

int get_server_specs(char *ip, int ipSize, uint16_t *port, const char * const pathToJson);

void random_base64_string(char *string, size_t stringSize);

#endif
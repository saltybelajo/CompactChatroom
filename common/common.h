#ifndef _COMMON_H
#define _COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include "uthash.h"
#include <poll.h>
#include <math.h>

#define MSGMLEN 512
#define AUTHORMLEN 32
#define CLIENTCAP 16

int writeft(int fd, char *msgbuf, char *actor);

unsigned long sdbm(char *str);

int getlsocket(char *ipAddr, uint16_t port);
// char *truncate(char *str, unsigned int size);

typedef struct actorMessage {
    char *actor;
    char *message;
} actorMessage;

unsigned long closest_pow2(int v);

#endif
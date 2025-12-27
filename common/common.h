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
#include <../common/uthash/src/uthash.h>
#include <poll.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <dirent.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <../common/cJSON/cJSON.h>
#include <sys/mman.h>

#define MSGMLEN 127
#define PARCELMLEN 255
#define AUTHORMLEN 31
#define CLIENTCAP 127

int writeft(int fd, char *msgbuf, char *author);

unsigned long hash_sdbm(char *str);

int getlsocket(char *ipAddr, uint16_t port);
// char *truncate(char *str, unsigned int size);

typedef struct authorMessage {
    char *author;
    char *message;
} authorMessage;

unsigned long closest_pow2(int v);

int write_to_user(int fd, char *msgbuf, char *author);

char *resolve_my_ip_address();

int readnl(int __fd, char *__buff, ssize_t __buffMaxSize);

#endif

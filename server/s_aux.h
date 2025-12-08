#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>


#define MSGMLEN 512

int writeft(int fd, char *msgbuf, char *actor);
CC=gcc
CFLAGS=-I.
DEPS = s_aux.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: server2 server

server2: server2.o s_aux.o ../common/common.o
	$(CC) -o server2 server2.o s_aux.o ../common/common.o

server: server.o s_aux.o ../common/common.o
	$(CC) -o server server.o s_aux.o ../common/common.o

clean:
	rm server.o server2.o s_aux.o ../common/common.o server server2


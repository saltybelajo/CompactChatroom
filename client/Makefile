CC=gcc
CFLAGS=-I.
DEPS = c_aux.h


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)


server: client.o c_aux.o ../common/common.o
	$(CC) -o client client.o c_aux.o ../common/common.o


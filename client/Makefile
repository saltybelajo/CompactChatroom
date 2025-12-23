CC=gcc
CFLAGS=-I.
DEPS = c_aux.h


%.o: %.c $(DEPS)
	$(CC) -c -ggdb -o $@ $< $(CFLAGS)


client: client.o c_aux.o ../common/common.o
	$(CC) -o client client.o c_aux.o ../common/common.o -lm -L../common/cJSON -l:libcjson.a


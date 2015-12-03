CC=gcc
CFLAGS=-fopenmp -g -std=gnu99 -pthread
LDFLAGS=-fopenmp

all: MAS hello

MAS: MAS.o

hello: hello.o

clean:
	rm -f *.o MAS hello

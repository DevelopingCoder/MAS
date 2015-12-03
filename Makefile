CC=gcc
CFLAGS=-fopenmp -g -std=gnu99 -pthread
LDFLAGS=-fopenmp

all: MAS

MAS: MAS.o

clean:
	rm -f *.o MAS

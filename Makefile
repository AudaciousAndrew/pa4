CC=clang
CFLAGS=-g -std=c99 -Wall -pedantic -Llib64 -lruntime

all: 
	$(CC) $(CFLAGS) main.c ipc.c proc.c queue.c cs.c -o pa4

clean:
	rm main
	rm *.log
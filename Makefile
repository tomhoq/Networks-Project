CC = gcc
CFLAGS = -w

.PHONY: all clean

all: userr server

userr: user/user.c
	$(CC) $(CFLAGS) -o ./userr user/user.c

server: AS/server.c
	$(CC) $(CFLAGS) -o ./server AS/server.c

clean:
	rm -f ./userr ./server

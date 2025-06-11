CC=gcc
ICDIRS=-I
OPT=-O1
CFLAGS=-Wall -Wextra -g $(INCDIRS) $(OPT) -lssl -lcrypto

CFILES=src/main.c src/server.c src/ssl.c
CFILES2=src/client.c
BINARY=bin/server
BINARY2=bin/client
SSL=ssl/cert.pem ssl/key.pem


all: $(BINARY)

$(BINARY):
	$(CC) $(CFLAGS) $(CFILES) -o $@
	$(CC) $(CFLAGS) $(CFILES2) -o $(BINARY2)

run:
	$(BINARY) $(SSL)
clean:
	rm -f $(BINARY)
	rm -f $(BINARY2)

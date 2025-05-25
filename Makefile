CC=gcc
ICDIRS=-I
OPT=-O1
CFLAGS=-Wall -Wextra -g $(INCDIRS) $(OPT)

CFILES=src/server.c
CFILES2=src/client.c
BINARY=bin/server
BINARY2=bin/client

all: $(BINARY)

$(BINARY):
	$(CC) $(CFLAGS) $(CFILES) -o $@

$(BINARY2):
	$(CC) $(CFLAGS) $(CFILES2) -o $@

run:
	$(BINARY)

clean:
	rm -f $(BINARY)

del:
	rm -f $(BINARY2)

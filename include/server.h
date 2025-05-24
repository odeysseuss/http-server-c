#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <netdb.h>
#include <unistd.h>

#define PORT "3490"
#define BACKLOG 10

void sigchld_handler();
void *get_in_addr(struct sockaddr *sa);

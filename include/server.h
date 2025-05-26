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
#include <poll.h>

#define PORT "3490"
#define BACKLOG 10
#define INITIAL_FD_SIZE 5

void sigchld_handler();
void *get_in_addr(struct sockaddr *sa);
int sendall(int fd, char *buf, int *len);
void add_to_pfds(struct pollfd *pfds[], int newfd, int *fd_count, int *fd_size);
void del_from_fds(struct pollfd pfds[], int i, int *fd_count);

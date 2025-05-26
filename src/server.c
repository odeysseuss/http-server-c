#include "../include/server.h"

void sigchld_handler() {
    int saved_errno = errno;
    while(waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    } else {
        return &(((struct sockaddr_in6 *)sa)->sin6_addr);
    }
}

int sendall(int fd, char *buf, int *len) {
    int total = 0;
    int bytes_left = *len;
    int n;

    while(total < *len) {
        n = send(fd, buf+total, bytes_left, 0);
        if (n == -1) {
            break;
        }
        total += n;
        bytes_left -= n;
    }

    *len = total;
    return (n == -1 ? -1 : 0);
}

void add_to_pfds(struct pollfd *pfds[], int newfd, int *fd_count, int *fd_size) {
    if (*fd_count == *fd_size) {
        *fd_size *= 2;
        *pfds = realloc(*pfds, sizeof(**pfds) * (*fd_size));
    }

    (*pfds)[*fd_count].fd = newfd;
    (*pfds)[*fd_count].events = POLLIN;
    (*pfds)[*fd_count].revents = 0;

    (*fd_count)++;
}

void del_from_pfds(struct pollfd pfds[], int i, int *fd_count) {
    pfds[i] = pfds[*fd_count-1];
    (*fd_count)--;
}

int main() {
    int sockfd, acceptfd;
    struct addrinfo hints, *servinfo, *p;
    int yes = 1;
    int rv;
    struct sigaction sig;
    struct sockaddr_storage conn_addrinfo;
    socklen_t sin_size;
    char s[INET6_ADDRSTRLEN];
    
    // Poll variables
    struct pollfd *pfds = malloc(sizeof(struct pollfd) * INITIAL_FD_SIZE);
    int fd_count = 0;
    int fd_size = INITIAL_FD_SIZE;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my ip

    rv = getaddrinfo(NULL, PORT, &hints, &servinfo);
    if (rv != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through the results, bind to the first one
    for(p = servinfo; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1) {
            perror("server: socket");
            continue;
        }

        int sock_opt = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
        if (sock_opt == -1) {
            perror("setsockopt");
            exit(1);
        }

        int sock_bind = bind(sockfd, p->ai_addr, p->ai_addrlen);
        if (sock_bind == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo);

    if (p == NULL) {
        fprintf(stderr, "server failed to bind\n");
        exit(1);
    }

    int sock_listen = listen(sockfd, BACKLOG);
    if (sock_listen == -1) {
        perror("server: sock_listen");
        exit(1);
    }

    sig.sa_handler = sigchld_handler; // read dead processes
    sigemptyset(&sig.sa_mask);
    sig.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sig, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    // Add the server socket to the poll set
    add_to_pfds(&pfds, sockfd, &fd_count, &fd_size);

    printf("server waiting for connection\n");

    while(1) {
        int poll_count = poll(pfds, fd_count, -1);

        if (poll_count == -1) {
            perror("poll");
            exit(1);
        }

        for (int i = 0; i < fd_count; i++) {
            if (pfds[i].revents == 0)
                continue;

            if (pfds[i].revents & POLLIN) {
                if (pfds[i].fd == sockfd) {
                    // Handle new connection
                    sin_size = sizeof(conn_addrinfo);
                    acceptfd = accept(sockfd, (struct sockaddr *)&conn_addrinfo, &sin_size);
                    if (acceptfd == -1) {
                        perror("accept");
                        continue;
                    }

                    inet_ntop(conn_addrinfo.ss_family,
                            get_in_addr((struct sockaddr *)&conn_addrinfo),
                            s, sizeof(s));
                    printf("server got connection from %s\n", s);

                    // Add the new connection to our poll set
                    add_to_pfds(&pfds, acceptfd, &fd_count, &fd_size);

                    // --- Send response immediately after accepting ---
                    char *response = "THE INDOMINABLE HUMAN SPIRIT!\r\n";
                    int response_len = strlen(response);
                    int sock_send = sendall(acceptfd, response, &response_len);
                    if (sock_send == -1) {
                        perror("send");
                    } else {
                        printf("Sent %d bytes to client\n", sock_send);
                    }
                    // --- (Optional: Close connection after sending) ---
                    // close(acceptfd);
                    // del_from_pfds(pfds, fd_count - 1, &fd_count);
                }
                else {
                    // (Optional: If you still want to handle client data later)
                    char buf[256];
                    int len = recv(pfds[i].fd, buf, sizeof(buf), 0);
                    if (len <= 0) {
                        if (len == 0) {
                            printf("socket %d hung up\n", pfds[i].fd);
                        } else {
                            perror("recv");
                        }
                        close(pfds[i].fd);
                        del_from_pfds(pfds, i, &fd_count);
                        i--;
                    }
                }
            }
            else if (pfds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
                printf("Error on socket %d\n", pfds[i].fd);
                close(pfds[i].fd);
                del_from_pfds(pfds, i, &fd_count);
                i--;
            }
        }
    }

    // clean up
    for (int i = 0; i < fd_count; i++) {
        if (pfds[i].fd >= 0)
            close(pfds[i].fd);
    }
    free(pfds);

    return 0;
}

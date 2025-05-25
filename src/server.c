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

int main() {
    int sockfd, acceptfd;
    struct addrinfo hints, *servinfo, *p;
    int yes = 1;
    int rv;
    struct sigaction sig;
    struct sockaddr_storage conn_addrinfo;
    socklen_t sin_size;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my ip

    rv = getaddrinfo(NULL, PORT, &hints, &servinfo);
    if (rv != 0) {
        fprintf(stderr, "getaddeinfo: %s\n", gai_strerror(rv));
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

    printf("server waiting for connection\n");

    while(1) {
        sin_size = sizeof(conn_addrinfo);
        acceptfd = accept(sockfd, (struct sockaddr *)&conn_addrinfo, &sin_size);
        if (acceptfd == -1) {
            perror("server: accept");
            exit(1);
        }

        inet_ntop(conn_addrinfo.ss_family, get_in_addr((struct sockaddr *)&conn_addrinfo), s, sizeof(s));
        printf("server got connection from %s\n", s);

        if (!fork()) {
            close(sockfd);
            char *buf = "THE INDOMINABLE HUMAN SPIRIT!\n";
            int buf_len = strlen(buf);
            int sock_send = sendall(acceptfd, buf, &buf_len);
            if (sock_send == -1) {
                perror("server: send");
            }
            // shutdown(acceptfd, 2); // same as close 0->receive 1->send
            // dont know why shutdown gave an error, will try to find why
            close(acceptfd);
            exit(0);
        }
        close(acceptfd);
    }
    return 0;
}

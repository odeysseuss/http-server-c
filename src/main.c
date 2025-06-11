#include "../include/server.h"
#include "../include/ssl.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <certificate.pem> <private.key>\n", argv[0]);
        return 1;
    }

    const char *cert_path = argv[1];
    const char *key_path = argv[2];

    // Initialize OpenSSL
    init_openssl();
    ssl_ctx = create_context();
    configure_context(ssl_ctx, cert_path, key_path);

    int sockfd = setup_server_socket();
    if (sockfd == -1) {
        fprintf(stderr, "Failed to start server\n");
        return 1;
    }

    // Set up signal handler
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    // Get local IP address
    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    struct hostent *host = gethostbyname(hostname);

    printf("HTTPS Server running at:\n");
    printf("Local:  https://localhost:%s\n", PORT);
    printf("LAN:    https://%s:%s\n", inet_ntoa(*(struct in_addr*)host->h_addr), PORT);
    printf("Remote: https://103.214.201.216:%s\n", PORT);
    printf("Serving files from ./static/\n");

    while (1) {
        struct sockaddr_storage their_addr;
        socklen_t sin_size = sizeof(their_addr);
        char s[INET6_ADDRSTRLEN];

        int client_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (client_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
                 get_in_addr((struct sockaddr *)&their_addr),
                 s, sizeof(s));
        printf("Connection from %s\n", s);

        if (!fork()) { // Child process
            close(sockfd);
            handle_client_data(client_fd);
            exit(0);
        }
        close(client_fd);
    }

    close(sockfd);
    SSL_CTX_free(ssl_ctx);
    cleanup_openssl();
    return 0;
}

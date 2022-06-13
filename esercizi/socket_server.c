#include <strings.h>
#include "stdlib.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include "unistd.h"
#include "stdio.h"
#include "signal.h"

int socketfd;
int socket_accept_fd;

void sig_tem_hand() {
    close(socketfd);
    close(socket_accept_fd);
    exit(123);
}

int main() {
    struct sockaddr_in sockaddrIn;
    struct sockaddr_in sockaddrInClient;
    int client_addr_dimen;

    bzero((char *) &sockaddrIn, sizeof(sockaddrIn));
    bzero((char *) &sockaddrInClient, sizeof(sockaddrInClient));

    signal(SIGTERM, sig_tem_hand);
    //CHILD - SERVER PART
    sockaddrIn.sin_port = htons((u_int16_t) 3801);
    sockaddrIn.sin_family = AF_INET;
    sockaddrIn.sin_addr.s_addr = INADDR_ANY;

    socketfd = socket(PF_INET, SOCK_STREAM, 6);
    if (socketfd >= 0) {
        if (bind(socketfd, (struct sockaddr *) &sockaddrIn, sizeof(sockaddrIn)) < 0) {
            perror("Error in binding");
            close(socketfd);
            exit(-1);
        }
        if (listen(socketfd, 1) < 0) {
            close(socketfd);
            exit(-1);
        }
        if((socket_accept_fd = accept(socketfd, (struct sockaddr *) &sockaddrInClient, (unsigned *) &client_addr_dimen)) >= 0) {
            int nread;
            char buf[120];
            while ((nread = read(socket_accept_fd, buf, sizeof(buf))) > 0) {
                write(STDOUT_FILENO, buf, nread);
            }
            close(socket_accept_fd);
            printf("Server is done\n");
            exit(0);
        }
    } else {
        perror("Cannot open socket");
        close(socket_accept_fd);
        close(socketfd);
        exit(3);
    }//---------------------------------------------------------------------------------
}
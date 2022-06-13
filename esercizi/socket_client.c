#include "netinet/in.h"
#include "arpa/inet.h"
#include "unistd.h"
#include "stdio.h"
#include "signal.h"
#include "stdlib.h"

int socketfd;

void sig_tem_hand() {
    close(socketfd);
    exit(123);
}

int main()
{
    struct sockaddr_in sockaddrInConnect;
    char buf[120];

    signal(SIGTERM, sig_tem_hand);

    printf("Message to send:\n");
    gets(buf);

    socketfd = socket(PF_INET, SOCK_STREAM, 6);
    if (socketfd < 0) {
        perror("Error opening socket");
        exit(-1);
    }

    //Creare struttura per indirizzo server
    sockaddrInConnect.sin_port = htons((u_int16_t) 3800);
    sockaddrInConnect.sin_addr.s_addr = inet_addr("127.0.0.1");
    sockaddrInConnect.sin_family = AF_INET;

    if ((connect(socketfd, &sockaddrInConnect, sizeof(sockaddrInConnect))) < 0) {
        perror("Cannot connect to server");
        exit(-1);
    }

    int nread;
    while (nread = read(socketfd, buf, sizeof(buf)) > 0) {
        printf("Read %d bytes\n", nread);
        printf("Content \"%s\"\n", buf);
    }

    close(socketfd);
}
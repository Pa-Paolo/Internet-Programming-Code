#include "netinet/in.h"
#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "signal.h"
#include "arpa/inet.h"

int sock;

void close_hand()
{
    close(sock);
}

int main()
{
    signal(SIGTERM,close_hand);

    struct sockaddr_in sockaddrTarget;

    sockaddrTarget.sin_port = htons((u_int16_t) 3800);
    sockaddrTarget.sin_addr.s_addr = inet_addr("127.0.0.1");
    sockaddrTarget.sin_family = AF_INET;

    sock = socket(PF_INET,SOCK_DGRAM,17);
    if(sock<0)
    {
        perror("Error in creating socket");
        close_hand();
        exit(1);
    }

    char buf[256];
    for (int i = 0; i < 20; ++i) {
        printf("Content to send:\n");
        gets(buf);
        sendto(sock,buf, sizeof(buf),0,&sockaddrTarget, sizeof(sockaddrTarget));
    }

    close_hand();
}
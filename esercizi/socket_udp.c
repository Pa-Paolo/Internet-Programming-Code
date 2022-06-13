#include "netinet/in.h"
#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "signal.h"

int sock;

void close_hand()
{
    close(sock);
}

int main() {

    int type = AF_INET;
    char buf[256];

    signal(SIGTERM,close_hand);

    struct sockaddr_in sockaddrIn;
    struct sockaddr_in sockaddrClient;
    int client_lenght;

    sockaddrIn.sin_addr.s_addr = INADDR_ANY;
    sockaddrIn.sin_port = htons((u_int16_t) 3800);
    sockaddrIn.sin_family = AF_INET;

    sock = socket(PF_INET,SOCK_DGRAM,17);
    if(sock<0)
    {
        perror("Error on socket creation");
        exit(1);
    }

    if(bind(sock,&sockaddrIn, sizeof(sockaddrIn))<0) {
        perror("Error on socket binding");
        close_hand();
        exit(1);
    }

    int n;
    while((n=recvfrom(sock,buf, sizeof(buf),0,&sockaddrClient,&client_lenght))>0)
    {
        printf("Read %d bytes\n",n);
        printf("Content \"%s\"\n",buf);
        fflush(stdout);
    }

    close_hand();
}
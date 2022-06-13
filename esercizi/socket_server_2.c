#include <strings.h>
#include "stdlib.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "unistd.h"
#include "stdio.h"
#include "signal.h"

int socke;
int socketAcc;

void sig_tem_hand() {
    exit(123);
}

int main() {
    signal(SIGTERM,sig_tem_hand);

    struct sockaddr_in sockaddrIn;
    struct sockaddr_in sockAddrAcc;
    int ndim;
    char buf[80];

    bzero((char *) &sockaddrIn, sizeof(sockaddrIn));

    sockaddrIn.sin_port = htons((u_int16_t)5200);
    sockaddrIn.sin_addr.s_addr = INADDR_ANY;
    sockaddrIn.sin_family = AF_INET;

    //printf("Message to send:\n");
    //gets(buf);

    //int type = SOCK_STREAM;            /* A TCP socket is desired */
    //int protocol = 6;
    //socke = socket(PF_INET, type, protocol);
    if((socke=socket(PF_INET,SOCK_STREAM,6))<0)
    {
        perror("Creation of socket");
        close(socke);
        exit(1);
    }

    printf("Value of socket: %d\n",socke);

    if(bind(socke,(struct sockaddr *) &sockaddrIn, sizeof(sockaddrIn))<0) {
        perror("Bind of socket");
        close(socke);
        exit(1);
    }

    if(listen(socke,1)<0) {
        perror("Listen of socket");
        close(socke);
        exit(1);
    }

    if((socketAcc=accept(socke,(struct sockaddr *) &sockAddrAcc,(unsigned *) &ndim))<0){
        perror("Accept error");
        close(socke);
        close(socketAcc);
        exit(1);
    } else
    {
        int n;
        //write(socketAcc,buf,sizeof(buf));
        //close(socke);
        printf("Accepted, waiting for input\n");
        fflush(stdout);

        while ((n = read(socketAcc, buf, sizeof(buf))) > 0) {
            printf("REC\n");
            fflush(stdout);
            write(STDOUT_FILENO, buf, n);
        }
    }

    exit(0);
}
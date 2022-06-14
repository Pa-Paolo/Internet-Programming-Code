#include "signal.h"
#include "unistd.h"
#include "stdlib.h"
#include "netinet/in.h"
#include "stdio.h"
#include "strings.h"
#include "arpa/inet.h"


int sock=-1;


void handler_close(){
    close(sock);
    exit(SIGTERM);
}

int main(){
    char buf[120];

    signal(SIGTERM,handler_close);

    struct sockaddr_in sockaddrIn;
    struct sockaddr_in sockaddrInClient;
    bzero(&sockaddrIn,sizeof(sockaddrIn));

    printf("Insert port:\n");
    gets(buf);
    int port = atoi(buf);
    sockaddrIn.sin_port = htons((u_int16_t) port);

    printf("Insert adress of server:\n");
    gets(buf);
    sockaddrIn.sin_addr.s_addr = inet_addr(buf);
    printf("Server address: %s\n", inet_ntoa(sockaddrIn.sin_addr));
    sockaddrIn.sin_family = AF_INET;

    sock = socket(PF_INET,SOCK_STREAM,6);
    if(sock<0)
    {
        perror("Error in creating socket");
        exit(1);
    }

    if(connect(sock,&sockaddrIn,sizeof(sockaddrIn))<0)
    {
        perror("Error in connecting to server");
        handler_close();
    }

    char httpReq[120];
    sprintf(httpReq,"GET google.it/mail HTTP/1.1");
    printf("Size of req: %d\n", sizeof(httpReq));

    write(sock,httpReq, sizeof(httpReq));

    int n;
    char resp[1250];
    while((n= read(sock,resp, sizeof(resp)))>0)
    {
        printf("Received %d bytes\n",n);
        printf("Response \n\"%s\"\n",resp);
        fflush(stdout);
    }
    printf("Closing client\n");

    close(sock);
    exit(0);
}
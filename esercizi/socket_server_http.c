#include "stdio.h"
#include "stdlib.h"
#include "netinet/in.h"
#include "unistd.h"
#include "arpa/inet.h"

int sock = -1;
int childSock = -1;

void hand_close() {
    close(sock);
    close(childSock);
    exit(1);
}

int main() {
    char buf[120];
    printf("Insert Port:\n");
    gets(buf);
    int port = atoi(buf);

    struct sockaddr_in sockaddrIn;
    struct sockaddr_in sockaddrReq;
    int sizeOfReqAddr;
    sockaddrIn.sin_port = htons((u_int16_t) port);
    sockaddrIn.sin_addr.s_addr = INADDR_ANY;
    sockaddrIn.sin_family = AF_INET;

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Error in creation of socket");
        hand_close();
    }

    if (bind(sock, &sockaddrIn, sizeof(sockaddrIn)) < 0) {
        perror("Error in binding");
        hand_close();
    }

    if (listen(sock, 2) < 0) {
        perror("Error in listening");
        hand_close();
    }

    while ((childSock = accept(sock, &sockaddrReq, &sizeOfReqAddr)) > 0) {
        if(fork()==0) {
            close(sock);
            int n;
            if ((n = read(childSock, buf, sizeof(buf))) > 0) {
                printf("Read %d bytes", n);
                printf("\n%s\n", buf);
            }
            printf("Read all the buffer\n");

            char head[120] = "HTTP/1.1 200 OK\ncontent-type: TEXT\n\n";
            write(childSock, head, sizeof(head));

            char *html;
            html = "<!DOCTYPE html>\n"
                   "<html>\n"
                   "<head>\n"
                   "    <title>Title of the document</title>\n"
                   "</head>\n"
                   "\n"
                   "<body>\n"
                   "The content of the document......\n"
                   "</body>\n"
                   "\n"
                   "</html>";
            write(childSock, html, sizeof(html));
            break;
        } else{
            close(childSock);
        }
    }

    if (childSock < 0)perror("Error in accepting");
    printf("Child socket was: %d\n", childSock);

    close(sock);
    close(childSock);
    exit(0);
}
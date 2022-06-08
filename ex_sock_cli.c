#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int
main(int argc, char **argv) {
    int port;
    int protocol;
    char buf[80];
    char ipcli[80];
    int s, type;            /* Socket descriptor and socket type    */
    int fd;
    int n;
    struct sockaddr_in cli;        /* Endpoint address of the client  */

    printf("Server IP: ");
    gets(ipcli); //reads a line from stdin and stores it into the string pointed to by str
    printf("Port to wait for calls: ");
    gets(buf); //reads a line from stdin and stores it into the string pointed to by str
    port = atoi(buf); //Converte da string a intero
    if (port < 1024 || port > 65535) {
        printf("Port number out of range\n");
        exit(1);
    }


    bzero((char *) &cli, sizeof(cli));    /* clear the structure, svuota il contenuto della struttura cli
 *  Questo perchè altrimenti trovo dei valori casuali al suo interno, meglio sapere cosa aspettarsi, più che altro se dovessi
 *  lavorare con dei puntatori si rischia di andare a puntare dei valori fuori dalla memoria che portano a crash*/
    cli.sin_family = AF_INET; //In pratica ipv4
    cli.sin_port = htons((u_short) port);    /* Assure the big endianity! perchè nella struttura devono essere memorizzati
 * in questo formato*/

    if ((cli.sin_addr.s_addr = inet_addr(ipcli)) ==
        INADDR_NONE) { //inet_addr converte dalla dotted form (127.99.12.1) alla inet form
        printf("Invalid IP address\n");
        exit(1);
    }

    printf("File to send: "); //Se inserisco "-" al posto del path per un file apre lo standard input (vedi if successivo),
    gets(buf); //Legge input e lo mette in buf
    if (strcmp(buf, "-") == 0)
        fd = STDIN_FILENO;
    else if ((fd = open(buf, O_RDONLY)) <
             0) { //Apre in lettura il file oppure lo standard input a seconda della scelta precedente
        perror(buf);
        exit(1);
    }

    type = SOCK_STREAM;            /* A TCP socket is desired */
    protocol = 6;                /* TCP requires 6 */
    s = socket(PF_INET, type, protocol);  //Create the endpoint and locate the file descriptor
    if (s < 0) {
        perror("Can't create socket ");
        exit(1);
    }

    /* If no bind is called, autobind will be executed by connect sys call */
    if ((connect(s, (struct sockaddr *) &cli, sizeof(cli))) < 0) { //PER il client non serve fare il BIND ma
        //si può fare se si vuole per specificare la porta. Altrimenti lo fa da solo prendendo una porta casuale come input.
        perror("Connect: ");
        close(s);
        exit(1);
    }

    /* now the server is connected */
    //Leggo e rimando quello che leggo. (Finchè il fd non viene chiuso?)
    while ((n = read(fd, buf, sizeof(buf))) > 0) {
        write(s, buf, n);
    }

    printf("Client is done\n");
    close(s);
    exit(0);
}



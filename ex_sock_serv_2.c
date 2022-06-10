#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int s = -1;

void
do_quit(int unused) //Signal handler che è chiamato quando chiamiamo per esempio ctrl+c
{
    if (s >= 0) //Controlla se un socket è aperto e lo chiudo.
        close(s);
    printf("Server terminated by user\n");
    exit(0);
}

#ifdef ALTERNATIVE_SERVER
void
do_server(int s, struct sockaddr_in cli, int n)
{
int len;
int port;
int m;
char buf[100];

    printf("Server launched pid=%d\n", getpid());
    port=ntohs(cli.sin_port);
    m=sprintf(buf, "%d> ", port);
    printf("Client is %s on port %d\n",inet_ntoa(cli.sin_addr),port);

    while(1) {
        if((len=read(s, &buf[m], sizeof(buf)-m)) <= 0)
            break;
        write(STDOUT_FILENO, buf, len+m);
        }

    printf("Client closed the connection, port=%d\n", port);
    close(s);
    exit(0);
}
#else

void
do_server(int s, struct sockaddr_in cli, int n) {
    int len;
    int port;
    int m;
    char buf[100];

    printf("Server launched pid=%d\n", getpid());
    port = ntohs(cli.sin_port); //Per convertire il BIG ENDIAN in quello specifico della macchina nella quale lavoro.
    printf("Client is %s on port %d\n", inet_ntoa(cli.sin_addr),
           port); //inet_ntoa=ritorna la notazione puntata dell'indirizzo in BIG ENDIAN

    while (1) {
        if ((len = read(s, buf, sizeof(buf))) <= 0)
            break;
        write(STDOUT_FILENO, buf, len);
    }

    printf("Client closed the connection, port=%d\n", port);
    close(s);
    exit(0);
}

#endif

int
main(int argc, char **argv) {
    int port;
    int protocol;
    char buf[80];
    int type;            /* Socket descriptor and socket type    */
    int cs;
    int n;
    struct sockaddr_in sin;        /* An Internet endpoint address  */
    struct sockaddr_in cli;        /* Endpoint address of the client  */

    printf("Port to wait for calls: ");
    gets(buf);
    port = atoi(buf);
    if (port < 1024 || port > 65535) {
        printf("Port number out of range\n");
        exit(1);
    }

    bzero((char *) &sin, sizeof(sin));    /* clear the structure mette a zero , ogni otteto a zero, il numero di zeri viene detto dal secondo argomento
    *  Metto a zero tutta la struttura sin. E' meglio farlo in modo che la struttura sia completamente pulita*/
    sin.sin_family = AF_INET; //La famiglia del socket
    sin.sin_addr.s_addr = INADDR_ANY;    /* Any IP can contact the server, non ci sono restrizioni sull'interfaccia dalla quale il server può ascoltare */
    sin.sin_port = htons((u_short) port); /* assure the big endianity ! Per specificare la porta alla quale mi connetto */

    type = SOCK_STREAM; /* A TCP socket is desired */
    protocol = 6; //C'è solo un protocollo per la famiglia AF_INET quindi non è importante specificare che si tratti di TCP. C'è un solo tipo di SOCK_STREAM che è il TCP.
    s = socket(PF_INET, type, protocol);
    if (s < 0) {
        perror("Can't create socket ");
        exit(1);
    }

    if (bind(s, (struct sockaddr *) &sin, sizeof(sin)) <
        0) { //Il secondo argomento è l'indirizzo alla quale voglio connettermi (Passo il puntatore alla struttura by referance)
        printf("Can't bind the port no. %d\n", port);
        close(s);
        exit(1);
    }
    if (listen(s, 1) < 0) { //Creo una coda alla quale il tcp ascolta, e la lunghezza della della coda
        printf("Can't listen\n");
        close(s);
        exit(1);
    }

    signal(SIGTERM, do_quit);
    signal(SIGQUIT, do_quit);
    signal(SIGINT, do_quit);

    /* Now wait for an incoming connection request */
    while (1) {
        if ((cs = accept(s, (struct sockaddr *) &cli, (unsigned *) &n)) < 0) { //ritorna un nuovo file descriptor
            perror("Accept: ");
            break;
        }

        /* A client is arrived ... now serve it */
        /* now fork a child to handle the connection, CREO un child process per servire il client che ha fatto scattare l'accept.
         * In modo che il parent process possa continuare ad accettare connessioni. In pratica delega ai child il compito di rispondere
         * alle richieste. Ogni accept che avviene passo di qua e creo un processo figlio che serve la richiesta, il parent
         * non entra dentro l'IF e quindi non chiama il metodo do_server().
         */
        if (fork() ==0) {
            //Il child e solo il child (0) chiude il socket, il numero di child dipende dai client connected, se ho 5 client connessi
            //allora avrò 5 children plus the parent (6 processi)
            close(s);
            //CHIUDERE IL SOCKET NEL CHILD NON SIGNIFICA CHIUDERLO NEL PARENT, nel kernel c'è una tabella che tiene conto di quanti programmi
            //stanno usando un certo oggetto quindi quando faccio la fork il contatore aumenta e chiudendolo solo nel figlio
            //rimane comunque qualcuno che lo sta usando. Quando chiudo  il socket decremento il contatore di 1, se il
            //contatore arriva a zero distrugge l'oggetto altrimenti lo lascia li.
            do_server(cs, cli, n);
        }
        close(cs);
    }

    sleep(1)
    do_quit(0);
}



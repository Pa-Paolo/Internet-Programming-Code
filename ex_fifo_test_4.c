#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/errno.h>
#include <signal.h>
#include <sys/select.h>

#define FIFO_NAME "fifo_server"
#define FIFO_PERM (O_RDWR | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

int fd_server = -1;

void
sigtermhandler(int unused) {
    printf("\nSignal received\n");
    if (fd_server >= 0) {
        close(fd_server);
        unlink(FIFO_NAME);
    }
    exit(0);
}

int
main(int argc, char **argv) {
    fd_set rfds, rfds_save;
    struct timeval tv;
    char rxbuf[100];
    ssize_t nread;
    int i;
    int retval;

    signal(SIGTERM, sigtermhandler);
    signal(SIGQUIT, sigtermhandler);
    signal(SIGTERM, sigtermhandler);
    signal(SIGINT, sigtermhandler);

    printf("Server running in foreground, pid=%d\n", getpid());
    umask(0);
    if (mkfifo(FIFO_NAME, FIFO_PERM) == -1) {
        if (errno != EEXIST) {
            perror("error making fifo ");
            exit(1);
        }
    }


    /* here the child process */
    // LA open è non blocking, ritorna anche se non c'è nulla ancora
    if ((fd_server = open(FIFO_NAME, O_RDONLY | O_NONBLOCK)) < 0) {
        perror("Open fifo for reading");
        exit(1);
    }
    printf("Fifo opened fd=%d\n", fd_server);


    /* select will wait up to five seconds. */
    tv.tv_sec = 5; //Il numero massimo di secondi (timeout)
    tv.tv_usec = 0; //Microsecondi aggiuntivi ai secondi precedenti.

    FD_ZERO(&rfds); //rfds = read file descriptor set. Puliamo il read file descriptior set
    FD_SET(fd_server,
           &rfds); //Includo il file descriptor del PIPE e lo includo nel file descriptor set in modo da monitorarlo.
    // Da ora in poi monitoro il PIPE
    FD_SET(STDIN_FILENO, &rfds); //Standard input file descriptor. Lo includo nel file descriptor set.
    rfds_save = rfds; //Salvo il file descriptor set in una variabile aggiuntiva.

    //Metto dieci perchè diciamo che è sufficiente in questo caso un massimo di 10 file descriptor.
    //Il time out permette di ritornare ogni 5 secondi. La e commerciale serve perchè per le strutture voglio passare il puntatore
    //in modo da non copiare la struttura così risparmio risorse.
    //ATTENZIONE LA SELECT VA A MODIFICARE IL SET E RIMANGONO SOLO I FD CHE SONO PRONTI AD ESSERE LETTI QUINDI
    //POI MI BASTA CONTROLLARE CON FD_ISSET SE E' PRESENTE NEL SET. SE E' PRESENTE NEL SET ALLORA E' STATO MODIFICATO
    //IL CONTENUTO PUNTATO DAL FILE DESCRIPTOR.
    while ((retval = select(10, &rfds, NULL, NULL, &tv)) >= 0) {
        if (retval == 0) {
            printf("Timeout expired\n");
            tv.tv_sec = 5;
            tv.tv_usec = 0;
        }
        //Se non sono passati 5 secondi allora probabilmente è successo qualcosa.
        else {
            //Questa funzione mi permette di sapere se è successo qualcosa nella FIFO.
            //FD_ISSET controlla che effettivamente ci sia ancora il FD di cui siamo interessati
            if (FD_ISSET(fd_server, &rfds)) {
                //Se è presente allora leggo il contenuto,
                nread = read(fd_server, rxbuf, sizeof(rxbuf));
                if (nread == 0) {
                    printf("Read an EOF\n");
                    /* The write side closed the pipe
                     * so, we have to reopen it */
                    close(fd_server);
                    if ((fd_server = open(FIFO_NAME, O_RDONLY | O_NONBLOCK)) < 0) {
                        perror("reopen fifo");
                        exit(1);
                    } else
                        printf("Fifo reopened fd=%d\n", fd_server);

                } else {
                    printf("Read %zd chars from the pipe: ", nread);
                    rxbuf[nread] = 0;
                    for (i = 0; i < nread; i++)
                        rxbuf[i] = toupper(rxbuf[i]);
                    printf("%s", rxbuf);
                    fflush(stdout);
                }
            }
            if (FD_ISSET(STDIN_FILENO, &rfds)) {
                //Qualcosa è successo nella keyboard
                nread = read(STDIN_FILENO, rxbuf, sizeof(rxbuf));
                if (nread == 0) {
                    printf("Read an EOF\n");
                    break;
                } else {
                    printf("Read %zd chars from the keyboard: ", nread);
                    rxbuf[nread] = 0;
                    for (i = 0; i < nread; i++)
                        rxbuf[i] = toupper(rxbuf[i]);
                    printf("%s", rxbuf);
                    fflush(stdout);
                }
            }
        }
        rfds = rfds_save;
    }

    if (retval < 0)
        printf("select error ... server exiting\n");
    close(fd_server);
    unlink(FIFO_NAME);
    exit(0);
}

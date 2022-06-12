#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/errno.h>
#include "signal.h"

#define FIFO_NAME "fifo_server"
#define FIFO_PERM (O_RDWR | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

void close_handler()
{
    printf("Server exiting\n");
    unlink(FIFO_NAME);
    exit(0);
}

int main(int argc, char **argv) {
    int pid;
    int fd_server;
    char rxbuf[100];
    ssize_t nread;
    int i;

    signal(SIGTERM,close_handler); //Aggiunto close handler in modo che quando si uccide il processo con il
    //segnale SIGTERM il file della named pipe venga eliminato

    printf("Example 2 of a fifo that runs in background\n");
    fflush(stdout);

    /*Questo programma fa partire un child che legge in continuazione da una named pipe e la tiene aperta (tramite l'if
     * all'interno del ciclo while) */

    pid = fork();
    if (pid==0) {
        /* parent process exits and let the child to do the bad work! */
        exit(0);
    }

    printf("CHILD: pid is %d\n", pid);
    fflush(stdout);

    umask(0);
    if (mkfifo(FIFO_NAME, FIFO_PERM) == -1) {
        if (errno != EEXIST) {
            perror("error making fifo ");
            exit(1);
        }
    }


    /* here the child process */
    if ((fd_server = open(FIFO_NAME, O_RDONLY)) < 0) {
        perror("Open fifo for reading");
        exit(1);
    }

    while ((nread = read(fd_server, rxbuf, sizeof(rxbuf))) >= 0) {
        if (nread == 0) {
            if ((fd_server = open(FIFO_NAME, O_RDONLY)) < 0) {
                perror("Open fifo for reading");
                break;
            }
            printf("Read an EOF: fifo reopend\n");
            continue;
        }
        printf("Read %zd from the pipe: ", nread);
        rxbuf[nread] = 0;
        for (i = 0; i < nread; i++)
            rxbuf[i] = toupper(rxbuf[i]);
        printf("%s", rxbuf);
    }

    close(fd_server);
    close_handler();
}

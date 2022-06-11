#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int
main(int argc, char **argv) {
    int pfd[2];
    ssize_t nread, nwrite;

    char buf[100];

    printf("Example 1 of pipes\n");
    fflush(stdout); //Forzo la stampa su terminale perchè altrimenti potrebbe non stamparla subito, ma per esempio,
    //Alla fine dell'eseguzione.

    /*
     * For  output streams, fflush() forces a write of all user-space buffered
     * data for the given output or update stream via the stream's  underlying
     * write function.
     */

    //Requesting the pipe, if an error occurs the program exit
    if (pipe(pfd) == -1) {
        perror("Pipe creation ");
        exit(1);
    }

    /* At first, write into the pipe */
    nwrite = write(pfd[1], "Hello world ... ", 16);
    nwrite += write(pfd[1], "pipe test\n", 10);
    printf("%zd bytes written into the pipe\n", nwrite);

    /* Now read from the pipe */
    nread = read(pfd[0], buf, sizeof(buf));
    printf("Read %zd from the pipe\n", nread);
    if (nread > 0)
        printf("Buffer content: %s", buf);
    exit(0);

    /*
     * Non ha molto senso che un programma scriva e poi legga da una pipe perchè lo potrebbe fare attraverso la propria
     * memoria senza chiedere una pipe al kernel. Il caso normale è quando ci sono due programmi che leggono e scrivono
     * sulla pipe.
     */
}

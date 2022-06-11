#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int
main(int argc, char **argv) {
    int pfd[2];
    pid_t pid;
    int sleepf = 0;

    //IN QUESTO ESEMPIO USIAMO LA FORK PER CREARE UN PROCESSO FIGLIO CHE VADA A DIALOGARE CON IL PADRE TRAMITE UNA PIPE

    printf("Example 2 of pipes\n");
    fflush(stdout); //Forzo la stampa su terminale perchè altrimenti potrebbe non stamparla subito, ma per esempio,
    //alla fine dell'eseguzione.

    //MI SEMBRA NON COMMENTATO A LEZIONE
    //Vado a leggere gli argomenti passati all'eseguibile se ce ne sono più di 1, questo perchè è sempre presente il nome
    //del file come primo argomento. Se l'argomento passato è "-s" allora imposto sleepf=1 che cambierà il comportamento
    //successivo del programma. Per passare un argomento al programma, sul terminale, basta aggiungerlo dopo il nome del
    //file eseguibile. Es. "kill -USR1 1349" , kill è l'eseguibile che stiamo facendo partire e sia "-USR1" e "1349" sono
    //dei parametri passati all'eseguibile. argv[0] = "kill", argv[1] = "-USR1", argv[2]= "1349".
    //Per questo eseguibile -> "./nome_eseguibile -s" e imposta sleepf=1.
    if (argc > 1) {
        if (!strcmp(argv[1], "-s"))
            sleepf = 1;
    }

    if (pipe(pfd) == -1) {
        perror("Error in Pipe creation ");
        exit(1);
    }

    //Se il valore ritornato dalla funzione fork è uguale a zero allora si tratta del processo figlio.
    //Se invece ha un valore diverso da zero significa che è il padre che ha ricevuto il pid del figlio.
    if ((pid = fork()) == 0) {
        char rxbuf[100];
        ssize_t nread;
        int i;

        /* here the child process starts:
         * the child has inhered the file descriptors
         * of the parent and can use them, thus the
         * child can connect to the pipe.
         *
         * We are interested in using the output of the pipe,
         * so we can close the input end
         */
        printf("Child: process created by fork\n");
        //Chiudo il file descriptor per scrivere, nel processo figlio voglio solo leggere e mi basta pdf[0]
        close(pfd[1]);

        //La read è bloccante finchè non c'è qualcosa ferma il processo figlio. Esco fuori dal while quando viene chiuso
        //il file descriptor della pipe per scrivere dal padre e la pipe viene svuotata.
        while ((nread = read(pfd[0], rxbuf, sizeof(rxbuf))) > 0) {
            printf("Child: Read %zd from the pipe\n", nread);
            rxbuf[nread] = 0;
            printf("Child: Buffer content: \n%s", rxbuf);
        }
        printf("Child: Process terminated");
        fflush(stdout);
        close(pfd[0]);
        exit(0);
    }
    else
    {
        char txbuf[100];
        int i;
        FILE *fd;
        if ((fd = fdopen(pfd[1], "w")) == NULL) {
            perror("Error in writing filedes");
            close(pfd[1]);
            exit(1);
        }
        /* Here the parent process continues its execution */
        /* Write into something into the pipe */
        fprintf(fd, "Hello world\n");
        for (i = 1; i <= 5; i++) {
            if (sleepf) { //Se sleepf=1 allora forzo l'output e mi fermo per un secondo.
                fflush(fd);
                sleep(1);
                //Se non mi fermo probabilmente il child leggerà tutto in una volta,
                //invece che una stampa per volta.
            }
            fprintf(fd, "Line %d\n", i);
        }

        printf("Parent: closing files descriptors...\n");
        fflush(stdout);
        fclose(fd);
        close(pfd[1]);
        exit(0);
    }

}

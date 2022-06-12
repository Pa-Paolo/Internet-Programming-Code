#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int
main(int argc, char **argv) {
    int pfd[2];
    pid_t pid1, pid2;
    int sleepf = 0;

    printf("Example 3 of pipes started\n");
    fflush(stdout);

    if (pipe(pfd) == -1) {
        perror("Pipe creation ");
        exit(1);
    }


    if ((pid1 = fork()) == 0) {
        /* here the child process starts:
         * the child has inhered the file descriptors
         * of the father and can use them, thus the
         * child can connect to the pipe.
         *
         * We are interested to use the output of the pipe,
         * so we can close the input end
         */
        printf("CHILD: First child process created by fork\n");
        if (dup2(pfd[0], STDIN_FILENO) == -1) {
            /* Lo standard input del processo non è più la tastiera ma l'output del pipe!
             * Ora il child process è in grado di leggere dal pipe
             * */
            perror("dup2 error");
            exit(1);
        }
        close(pfd[1]); //Posso chiuderlo (forse perchè non mi serve)
        close(pfd[0]); //Posso chiuderlo perchè l'output del pipe ora è associato allo standard input dell'eseguibile.
        /* Possiamo chiudere il file descriptor perchè ora è referenziato dallo standard input.
         *
         * Replace the current image process with a new process,
         * specifically "cat"
         */
        execlp("cat", "cat", NULL); //Il comando cat è caricato in memoria e sostituisce il processo attuale
        /* Il mio processo diventa il processo cat. Permette di convertire un processo in un nuovo processo.
         * Il cat process riceve come input lo standard input (che però è stato sostituito con l'output della pipe) e lo
         * stampa nello standard output.
         *
         * The following two instructions should not be reached
         * if all is ok!
         * ... but just in case!
         */
        printf("execlp failed\n");
        exit(1);
    } else {
        /* Here the PARENT process continues its execution
         * and create a SECOND CHILDREN
         */
        if ((pid2 = fork()) == 0) {
            /* Here the second child begins its executuion */
            printf("Second child process created by fork\n");

            close(pfd[0]);
            if (dup2(pfd[1], STDOUT_FILENO) == -1) {
                /*
                 * L'input della pipe viene collegato allo standard output quindi quando scrivo nello standard output
                 * alla fine scrivo nel PIPE.
                 */
                perror("dup2 error in 2nd child");
                exit(1);
            }
            close(pfd[0]);
            close(pfd[1]);
            /* Replace the current image process with
             * a new process, specifically "who"
              */
            execlp("who", "who", NULL); //Trasforma il mio processo nel processo che indica quali utenti sono
            //connessi nel mio sistema.
            /* the following two instructions should not
             * be reached if all is ok!
              * ... but just in case!
              */
            printf("execlp failed\n");
            exit(1);
        }

        /* Still the parent */
        /* simply waits for the children to conclude their work */
        close(pfd[0]);
        close(pfd[1]);
        waitpid(pid1, NULL, 0);
        waitpid(pid2, NULL, 0);
        exit(0);
    }

}

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>

#define NULLSHM        ((void *) -1)
#define SHMPATH        "./shmpath"
#define PERM_FILE       (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

int *
getshmaddr(void) {
    key_t key;
    int shmid, *p;

    close(open(SHMPATH, O_WRONLY | O_CREAT, 0)); //Creo un file senza scrivere nulla quindi lo chiudo subito
    if ((key = ftok(SHMPATH, 1)) == -1) { //Creo una chiave usando il file appena creato
        perror(SHMPATH);
        exit(1);
    }
    if ((shmid = shmget(key, sizeof(int), IPC_CREAT | PERM_FILE)) ==
        -1) { //Se non esiste creo la shared memory di dimensione di un intero (4byte di solito)
        perror("shmget");
        exit(1);
    }
    p = shmat(shmid, NULL, 0); //Attacco la memoria virtuale nel kernel space identificata da shmid all'adress space
    //del processo chiamante la funzione (questo).
    if ((void *) p ==
        NULLSHM) { //In unix tutti i pointer deve essere un numero pari e -1 è un numero dispari che indica errore
        fprintf(stderr, "Invalid pointer to share memory\n");
        exit(1);
    }

    return p;
}

int
main(int argc, char **argv) {
    pid_t pid;
    //CHILD
    if ((pid = fork()) == 0) {
        int *p, prev = 0; //Il valore di p per il parent e per il child sono generalmenti diversi.

        p = getshmaddr(); //Chiamo la funzione sopra che mi restituisce il puntatore alla memoria condivisa
        while (*p != 99) //Perchè 99? Perchè nel padre scelgo che quando scrivo questo valore (nello spazio puntato dal puntatore) il figlio debba uscire
            //dal loop e quindi fermarsi.
            if (prev != *p) {
                printf("child saw %d\n", *p);
                prev = *p;
            }
        printf("child is done\n");

    } else { //PARENT PROCESS
        int *p;

        p = getshmaddr(); //Anche nel padre prendo il puntatore alla memoria condivisa che come scelto è della dimensione di un intero.
        //Continuo a scriverci finchè il valore puntato non raggiunge il valore 4.
        for (*p = 1; *p < 4; (*p)++) //Aumento l'intero di 1 ad ogni ciclo
            sleep(1); //Aspetto un secondo

        *p = 99; //Scelgo il valore 99 per dire al figlio che deve terminare
    }

    //ATTENZIONE: Il programma se viene eseguito più volte non restituisce sempre lo stesso risultato!
    //Questo acccade perchè nel ciclo while, a volte succede che, quando il while controlla il valore di *p esso è ancora diverso
    //da 99 quindi il codice successivo viene eseguito, ma, nel frattempo succede che il valore puntato da *p viene modificato e quindi
    //lo stampo
    exit(0);
}

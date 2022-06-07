#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>

#define NULLSHM		((void *) -1)
#define SHMPATH		"./shmpath"
#define PERM_FILE       (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

int *
getshmaddr(void)
{
key_t key;
int shmid, *p;

	close(open(SHMPATH, O_WRONLY | O_CREAT, 0)); //Creo un file senza scrivere nulla quindi lo chiudo subito
	if((key=ftok(SHMPATH, 1)) == -1) { //Creo una chiave usando il file appena creato
		perror(SHMPATH);
		exit(1);
		}
	if((shmid = shmget(key, sizeof(int), IPC_CREAT | PERM_FILE)) == -1) { //Se non esiste creo la shared memory di dimensione di un intero (4byte di solito)
		perror("shmget");
		exit(1);
		}
	p = shmat(shmid, NULL, 0);
	if((void *)p == NULLSHM) { //In unix tutti i pointer deve essere un numero pari e -1 è un numero dispari che indica errore
		fprintf(stderr, "Invalid pointer to share memory\n");
		exit(1);
		}

	return p;
}

int
main(int argc, char **argv)
{
pid_t pid;
    //CHILD
    if ((pid = fork()) == 0) {
        int *p, prev = 0; //Il valore di p per il parent e per il child sono generalmenti diversi.

        p = getshmaddr(); 
        while (*p != 99)
            if (prev != *p) {
                printf("child saw %d\n", *p);
                prev = *p;
            	}

        printf("child is done\n");
	}
	else { //PARENT
	int *p;

        p = getshmaddr(); 
        for (*p = 1; *p < 4; (*p)++)
            sleep(1);

        *p = 99;
    	}

    exit(0);
}

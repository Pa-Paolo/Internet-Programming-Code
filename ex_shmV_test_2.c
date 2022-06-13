#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <semaphore.h>

#define NULLSHM        ((void *) -1)
#define SHMPATH        "./shmpath"
#define PERM_FILE       (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define SEM1_NAME    "SemaphoreR"
#define SEM2_NAME    "SemaphoreW"

int *
getshmaddr(void) {
    key_t key;
    int shmid, *p;

    close(open(SHMPATH, O_WRONLY | O_CREAT, 0));
    if ((key = ftok(SHMPATH, 1)) == -1) {
        perror(SHMPATH);
        exit(1);
    }
    if ((shmid = shmget(key, sizeof(int), IPC_CREAT | PERM_FILE)) == -1) {
        perror("shmget");
        exit(1);
    }
    p = shmat(shmid, NULL, 0);
    if ((void *) p == NULLSHM) {
        fprintf(stderr, "Invalid pointer to share memory\n");
        exit(1);
    }

    return p;
}

int
main(int argc, char **argv) {
    pid_t pid;

    if ((pid = fork()) == 0) {
        int *p;
        int n;

        sem_t *csemR, *csemW; //A differenza del caso precedente dichiariamo due semafori

        /* Child process */
        //Siamo nel child process, per usare i semafori ci serve aprirli.
        p = getshmaddr();
        if ((csemR = sem_open(SEM1_NAME, O_CREAT, PERM_FILE, 0)) == SEM_FAILED) {
            perror("Child sem1 open");
            exit(1);
        }
        if ((csemW = sem_open(SEM2_NAME, O_CREAT, PERM_FILE, 0)) == SEM_FAILED) {
            perror("Child sem2 open");
            exit(1);
        }

        while (1) {
            sem_wait(csemR); //Non vado a leggere il valore di *p finchè il semaforo non è libero. Se qualcuno sta usando la
            //zona di memoria allora sem_wait aspetta finchè il valore di semaforo non viene incrementato e poi lo decrementa lui
            //siccome prende accesso alla zona di memoria.
            n = *p;add_executable(Es_socket2 esercizi/socket_server_2.c)
            sem_post(csemW); //Sem post aumenta il valore del semaforo rendendo libero l'accesso a un eventuale processo che sta aspettando
            //per poter accedere/scrivere la memoria. Una volta che copio il valore in "n" allora non mi importa se qualcun altro la leggerà o
            //modificherà siccome io ho già preso il valore che mi interessava.
            if (n == 99) break;
            printf("child saw %d\n", n);
        }

        printf("child is done\n");
        sem_close(csemR); //Una volta che il loop è finito chiudo l'accesso ai semafori che sono nel kernel space.
        sem_close(csemW);
        exit(0);
    } else {
        int *p;
        int i;
        sem_t *semR;
        sem_t *semW;

        semR = sem_open(SEM1_NAME, O_CREAT, PERM_FILE, 0);
        if (semR == SEM_FAILED) {
            perror(SEM1_NAME);
            exit(1);
        }
        semW = sem_open(SEM2_NAME, O_CREAT, PERM_FILE, 0);
        if (semW == SEM_FAILED) {
            perror(SEM2_NAME);
            exit(1);
        }

        /* Parent process */
        //Il procedimento è molto simile a quello fatto per il child fino a questo punto. Ora il parent vuole far si che
        //Mentre lui scrive il valore dentro alla memoria condivisa (puntata da *p) nessun altro possa leggere o scrivere
        //(gli altri devono comunque implementare il corrispondente semaforo, con lo stesso nome)
        p = getshmaddr();
        *p = 0; //INIZIALIZZO IL VALORE NELLA MEMORIA CONDIVISA, IL SEMAFORO E' BLOCCANTE DI DEFAULT QUINDI SONO SICURO
        //CHE IL CHILD NON POSSA LEGGERE PER ORA.
        sem_post(semR); //INCREMENTO INIZIALE COSI' CHE SI POSSA INIZIARE AD USARE LA MEMORIA CONDIVISA CON I MECCANISMI
        //sem_wait e sem_post.

        for (i = 1; i < 4; i++) {
            sem_wait(semW); //Prendo possesso della memoria condivisa
            *p = i; //Scrivo sulla memoria condivisa
            sem_post(semR); //Libero la memoria condivisa
            sleep(2);
        }

        //Ripeto la scrittura ma questa volta metto il valore 99 che quando letto dal child lo fa uscire dal loop e terminare
        sem_wait(semW);
        *p = 99;
        sem_post(semR);

        sem_close(semR);
        sem_close(semW);
        sem_unlink(SEM1_NAME);
        sem_unlink(SEM2_NAME);
        exit(0);
    }
}

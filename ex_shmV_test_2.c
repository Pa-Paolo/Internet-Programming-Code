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

        sem_t *csemR, *csemW;

        /* Child process */
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
            sem_wait(csemR);
            n = *p;
            sem_post(csemW);
            if (n == 99) break;
            printf("child saw %d\n", n);
        }

        printf("child is done\n");
        sem_close(csemR);
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
        p = getshmaddr();
        *p = 0;
        sem_post(semR);

        for (i = 1; i < 4; i++) {
            sem_wait(semW);
            *p = i;
            sem_post(semR);
            sleep(2);
        }

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

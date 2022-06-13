#include "unistd.h"
#include "stdlib.h"
#include "stdio.h"
#include "ctype.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "signal.h"

#define FIFO_NAME "fifo_pb2022"
#define FIFO_PERM (O_RDWR | S_IRWXU)

void sig_term_hand() {
    unlink(FIFO_NAME);
    exit(SIGTERM);
}

int main() {
    int pid;
    int fd;
    char buf[120];
    FILE *file;
    fd_set fileds;
    struct timeval tv;

    signal(SIGTERM,sig_term_hand);

    if (mkfifo(FIFO_NAME, FIFO_PERM) == -1) {
        perror("Error in creating FIFO");
        exit(-1);
    }
    umask(0);

    if (pid = fork() != 0) {
        //PARENT
        printf("PARENT: Child opened at %d\n", pid);
        fd = open(FIFO_NAME, O_WRONLY); //Bloccante finchè non c'è qualcuno che apre la stream
        if (fd < 0) {
            perror("Father: error in opening fifo");
            exit(-1);
        }

        gets(buf);
        write(fd, buf, sizeof(buf));

        file = fdopen(fd, "w");
        if (file == NULL) {
            perror("Father - error in opening file");
            exit(-1);
        }

        int k = 0;
        while (k != 69) {
            k++;
            printf("Insert a number:\n");
            gets(buf);
            k = atoi(buf);
            fprintf(file, "%s", buf);
            fflush(file);
        }
    } else {
        //CHILD
        fd = open(FIFO_NAME, O_RDONLY); //Bloccante finchè non c'è qualcuno che apre la stream
        if (fd < 0) {
            perror("Child: error in opening fifo");
            exit(-1);
        }

        FD_ZERO(&fileds);
        FD_SET(fd, &fileds);
        fd_set copyfds = fileds;

        int ret_num;
        tv.tv_sec = 15;
        tv.tv_usec = 0;
        while ((ret_num = select(10, &fileds, NULL, NULL, &tv)) > 0) {
            if(FD_ISSET(fd,&fileds)) {
                printf("Child: waiting for content.\n");
                int nread;
                nread = read(fd, buf, sizeof(buf));
                printf("Read %d bytes from buffer\n", nread);
                //buf[nread] = 0;
                printf("Child: read \"%s\"\n", buf);
                fflush(stdout);
            }
            fileds = copyfds;
        }
    }

    unlink(FIFO_NAME);
    fclose(file);
    close(fd);
    exit(0);
}
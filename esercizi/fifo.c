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

void sig_term_hand()
{
    unlink(FIFO_NAME);
    exit(SIGTERM);
}

int main() {
    int pid;
    int filedesc;
    char buf[120];
    FILE *fd;
    signal(SIGTERM,sig_term_hand);

    umask(0);

    if (mkfifo(FIFO_NAME, FIFO_PERM) == -1) {
        perror("Error creating FIFOs\n");
        exit(-1);
    }
    printf("FIFO Created\n");
    fflush(stdout);


    if ((pid=fork())==0) {
        //CHILD
        if((filedesc = open(FIFO_NAME, O_WRONLY)) < 0) {
            perror("Error in fd open\n");
            exit(1);
        }

        printf("Starting. \n");
        fflush(stdout);

        if ((fd = fdopen(filedesc, "w")) == NULL) {
            perror("writing filedes");
            close(filedesc);
            exit(1);
        }
        /* Write something into the fifo */
        fprintf(fd, "Hello world\n");
        for (int i = 1; i <= 35; i++) {
            fflush(fd);
            if(i%2==0)sleep(1);
            fprintf(fd, "Line %d\n", i);
        }
        fclose(fd);
        close(filedesc);
        unlink(FIFO_NAME);
    } else {
        //PARENT
        if((filedesc = open(FIFO_NAME, O_RDONLY)) < 0) {
            perror("Error in fd open\n");
            exit(1);
        }

        int nread;
        while ((nread=read(filedesc, buf, sizeof(buf))) >0 ){
            printf("Read %d bytes from buffer\n", nread);
            buf[nread]=0;
            printf("%s\n",buf);
        }

        waitpid(pid,NULL,0);
    }


    exit(0);
}
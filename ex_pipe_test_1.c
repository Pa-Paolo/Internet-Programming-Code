#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int
main(int argc, char **argv) {
    int pfd[2];
    ssize_t nread, nwrite;

    char buf[100];

    printf("Example 1 of pipes\n");
    fflush(stdout);

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
}

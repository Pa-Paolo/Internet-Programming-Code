#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int 
main (int argc, char **argv)
{
int pfd[2];
pid_t pid;
int sleepf=0;

	printf("Example 2 of pipes\n");
	fflush(stdout);
	if(argc > 1) {
		if(!strcmp(argv[1], "-s"))
			sleepf=1;
		}

	if(pipe(pfd) == -1) {
		perror("Pipe creation ");
		exit(1);
		}


    //Se il valore ritornato dalla funzione fork è uguale a zero allora si tratta del processo figlio.
    //Se invece ha un valore diverso da zero significa che è il padre che ha ricevuto il pid del figlio.
	if((pid = fork()) == 0) {
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
		printf("Child process created by fork\n");
		close (pfd[1]);
		while((nread=read(pfd[0], rxbuf, sizeof(rxbuf))) > 0) {
			printf("Read %zd from the pipe\n", nread);
			rxbuf[nread]=0;
			printf("Buffer content: %s\n", rxbuf);
			}
		close (pfd[0]);
		exit(0);
		}
	else {
		char txbuf[100];
		int i;
		FILE *fd;
		if((fd=fdopen(pfd[1], "w")) == NULL) {
			perror("writing filedes");
			close(pfd[1]);
			exit(1);
			}
		/* Here the parent process continues its execution */
		/* Write into something into the pipe */
		fprintf(fd, "Hello world\n");
		for(i=1; i <= 5; i++) {
			if(sleepf) {
				fflush(fd);
				sleep(1);
				}
			fprintf(fd, "Line %d\n", i);
			}

		fclose(fd);
		close(pfd[1]);
		exit(0);
		}

}

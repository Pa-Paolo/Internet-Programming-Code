#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/errno.h>
#include <sys/wait.h>

#define FIFO_NAME "fifo_test"
#define FIFO_PERM (O_RDWR | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH) //con questa definizione chiunque può scrivere e leggere.

int 
main (int argc, char **argv)
{
pid_t pid;
int i;
int sleepf=0;
	
	printf("Example 1 of a fifo (aka named pipe)\n");
	fflush(stdout);
	if(argc > 1) {
		if(!strcmp(argv[1], "-s"))
			sleepf=1;
		}

	umask(0);
	if(mkfifo(FIFO_NAME, FIFO_PERM) == -1) {
		if(errno != EEXIST) {
			perror("making fifo ");
			exit(1);
			}
		}


	if((pid = fork()) == 0) {
		int fd_server;
		char rxbuf[100];
		ssize_t nread;
		int i;

		/* here starts the child process: 
		 * Wwe are interested to use the output of the fifo,
		 * so we open it in READ_ONLY mode
		 */
		printf("CHILD: Child process created by fork\n");
		if((fd_server = open(FIFO_NAME, O_RDONLY)) < 0) {
            /*
             * Il figlio apre un pipe, apro l'oggetto referenziato in modalità read only
             */
			perror("Open fifo for reading");
			exit(1);
			}
		while((nread=read(fd_server, rxbuf, sizeof(rxbuf))) > 0) {
            //Quando la read ritorna zero o negativo probabilmente la pipe è stata chiusa.
			printf("CHILD: Read %zd from the pipe\n", nread);
			rxbuf[nread]=0;
			printf("CHILD: Buffer content: \n%s", rxbuf);
        }
		close (fd_server);
		exit(0);
		}
	else {
		int fd_client;
		int i;
		FILE *fd;

		/* here the parent process continues its execution
		 * Il parent apre il named pipe in modalità sola scrittura.
		 * */
		if((fd_client = open(FIFO_NAME, O_WRONLY)) < 0) {
			perror("Open fifo for writing");
			exit(1);
			}
		if((fd=fdopen(fd_client, "w")) == NULL) {
			perror("writing filedes");
			close(fd_client);
			exit(1);
			}
		/* Write something into the fifo */
		fprintf(fd, "Hello world\n");
		for(i=1; i <= 5; i++) {
			if(sleepf) {
				fflush(fd);
				sleep(1);
				}
			fprintf(fd, "Line %d\n", i);
        }

		fclose(fd);
		close(fd_client);
		/* patiently wait for the child to die */
		waitpid(pid, NULL, 0);
		unlink(FIFO_NAME); //Se non lo facciamo l'endpoint(file) continua a esistere. Se commento questa istruzione
        //Non cancello la named pipe. Se nel terminale faccio 'ls' noto che c'è un file speciale. Se riavvio il sistema
        //dovrebbero sparire i special file e in caso posso cancellarlo dal terminale con il comenado rm.
		exit(0);
		}

}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/errno.h>
#include <time.h>
#include <sys/select.h>

#define FIFO_NAME "fifo_server"
#define FIFO_PERM (O_RDWR | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

int 
main (int argc, char **argv)
{
fd_set rfds;
struct timeval tv;
int fd_server;
char rxbuf[100];
ssize_t nread;
int i;
int retval;
	
	printf("Example 2 of a fifo: the program runs in background\n");
	fflush(stdout);

	if(fork()) {
		/* parent process exits and let the child to do the bad work! */
		exit(0);
		}		

	umask(0);
	if(mkfifo(FIFO_NAME, FIFO_PERM) == -1) {
		if(errno != EEXIST) {
			perror("error making fifo ");
			exit(1);
			}
		}


	/* here the child process */ 
	if((fd_server = open(FIFO_NAME, O_RDONLY | O_NONBLOCK)) < 0) {
		perror("Open fifo for reading");
		exit(1);
		}

	/* select will wait up to five seconds. */
        tv.tv_sec = 5;
        tv.tv_usec = 0;

	FD_ZERO(&rfds);
        FD_SET(fd_server, &rfds);

	while((retval = select(10, &rfds, NULL, NULL, &tv)) >= 0) {
		if(retval == 0) {
			printf("Timeout expired\n");
			continue;
			}
		if(FD_ISSET(fd_server, &rfds)) {
			nread=read(fd_server, rxbuf, sizeof(rxbuf));
			if(nread == 0) {
				printf("Read an EOF\n");
				}
			else {
				printf("Read %zd chars from the pipe: ", nread);
				rxbuf[nread]=0;
				for(i=0; i < nread; i++)
					rxbuf[i] = toupper(rxbuf[i]);
				printf("%s", rxbuf);
				}
			}
		FD_ZERO(&rfds);
        	FD_SET(fd_server, &rfds);
		}

	if(retval < 0)
		perror("select error ... server exiting");
	close (fd_server);
	unlink(FIFO_NAME);
	exit(0);
}

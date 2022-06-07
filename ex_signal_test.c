#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void 
usr1handle(int unused)
{
	printf("User1 signal received\n");
}

void
sigTermHandle(int unused)
{
    printf("Sigterm signal received\n");
}

int main (int argc, char **argv)
{
    int i;
	/* At first, we change the disposition of USR1
	 * signal and specify our defined handler
	 */
	signal(SIGUSR1, usr1handle);
    signal(SIGTERM, sigTermHandle);
	printf("Test program launched: PID=%d\n",getpid());

	/* then we continues our normal execution */
	for(i=1; i < 120; i++) {
		printf("%d\n", i);
		sleep(1);
		}
	exit(1);
}

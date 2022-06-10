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
	/* At first, we change the disposition of USR1 signal and specify our defined handler. In pratica imposto il
	 * metodo usr1handle come metodo che deve occuparsi del signal USR1. Quando quel signal viene chiamato tramite
	 * il comando "kill -USR1 pid_del_processo" il metodo usr1handle decide cosa fare, in questo caso stampa semplicemente
	 * una stringa.
	 * Per il segnale KILL viene assegnato un altro handler.
	 */
	signal(SIGUSR1, usr1handle);
    signal(SIGTERM, sigTermHandle);
	printf("Test program launched: PID=%d\n",getpid());
    //Stampo il PID così da poter sapere a che mandare il signal.

	/* then we continues our normal execution
	 * Ovvero per 120 secondi faccio un loop inutile solo per stare ad aspettare che arrivi un signal.
	 */
	for(i=1; i < 120; i++) {
		printf("%d\n", i);
		sleep(1);
		}
	exit(1);
}

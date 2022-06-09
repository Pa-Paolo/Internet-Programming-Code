#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <netinet/in.h>

int
main(int argc, char **argv)
{
int port;
int protocol;
char buf[80];
int    s, type;			/* Socket descriptor and socket type    */
int  cs;
int n;
struct sockaddr_in sin;		/* An Internet endpoint address  */
struct sockaddr_in cli;		/* Endpoint address of the client  */

	printf("Port to wait for calls: ");
	gets(buf);
	port = atoi(buf);
	if(port < 1024 || port > 65535) {
		printf("Port number out of range\n");
		exit(1);
		}

	bzero((char *)&sin, sizeof(sin));	/* clear the structure */
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;	/* Any IP can contact the server */
	sin.sin_port = htons((u_short)port); 	/* assure the big endianity ! La funzione htons fa la conversione */

	type= SOCK_STREAM;			/* A TCP socket is desired */
	protocol = 6;
	s = socket(PF_INET, type, protocol); //Crea il socket, definisce il tipo di socket che poi deve essere legato
    //All'indirizzo dalla bind. Infatti qua si definisce solo dominio, tipo e protocollo ma non un indirizzo e nemmeno una porta.
	if(s < 0) {
		perror("Can't create socket ");
		exit(1);
		}

	if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) { //Effettua la bind ovvero lega il socket all'indirizzo,
        //porta. Prima di adesso non sapevo su che porta ascoltare. Come indirizzo IP se ne può specificare uno altrimenti, come in
        //questo caso, si può mettere INADDR_ANY per far si che si ascolti su tutti gli IP della macchina sulla porta specificata.
		printf("Can't bind the port no. %d\n", port);
		close(s);
		exit(1);
		}
	if(listen(s, 1) < 0) { //La listen marchia il socket come passivo ovvero che deve ascoltare le connessioni in arrivo e
        //non generarne di nuove. Specifica quindi la volontà di voler ascoltare delle richieste e tramite il secondo parametro
        //impongo quanto può essere lunga la coda delle richieste.
		printf("Can't listen\n");
		close(s);
		exit(1);
		}
	/* Now wait for an incoming connection request */
    //L'accept è bloccante, questo significa che il programma si ferma a questo punto finchè non c'è una connessione in coda,
    //generata da un altro socket esterno, che deve essere servita. C'è anche la possibilità di definire il socket come NON
    //BLOCCANTE e quindi fare in modo che l'accept vada aventi anche se non ci sono connessioni da servire.
	if((cs = accept(s, (struct sockaddr *)&cli, (unsigned *)&n)) < 0) {
		perror("Accept: ");
		close(s);
		exit(1);
		}

	/* A client is arrived ... now serve it */
    //In questo caso l'accept era bloccante e se sono arrivato qua significa che c'è stata una connessione in arrivo ed è stata
    //accettata. Posso andare a leggere cosa è stato invitato dal client tramite il file descriptor ottenuto dalla call precedente.
	close(s);
	while((n = read(cs, buf, sizeof(buf))) > 0) {
		write(STDOUT_FILENO, buf, n);
		}

	printf("Server is done\n");
	close(cs);
	exit(0);

}



#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int
main(int argc, char **argv)
{
int port;
int protocol;
char buf[80];
char ipcli[80];
int  s, type;			/* Socket descriptor and socket type    */
int  fd;
int n;
struct sockaddr_in cli;		/* Endpoint address of the client  */

	printf("Server IP: ");
	gets(ipcli);
	printf("Port to wait for calls: ");
	gets(buf);
	port = atoi(buf);
	if(port < 1024 || port > 65535) {
		printf("Port number out of range\n");
		exit(1);
		}


	bzero((char *)&cli, sizeof(cli));	/* clear the structure */
	cli.sin_family = AF_INET;
	cli.sin_port = htons((u_short)port); 	/* assure the big endianity ! */

	if((cli.sin_addr.s_addr = inet_addr(ipcli)) == INADDR_NONE) { //inet_addr converte dalla dotted alla inet
		printf("Invalid IP address\n");
		exit(1);
		}

	printf("File to send: "); //Se inserisco "-" apre lo standard input,
	gets(buf);
	if(strcmp(buf, "-") == 0) 
		fd=STDIN_FILENO;
	else if((fd=open(buf, O_RDONLY)) < 0) {
		perror(buf);
		exit(1);
		}

	type=SOCK_STREAM;			/* A TCP socket is desired */
	protocol = 6;				/* TCP requires 6 */
	s = socket(PF_INET, type, protocol);
	if(s < 0) {
		perror("Can't create socket ");
		exit(1);
		}

	/* If no bind is called, autobind will be executed by connect sys call */
	if((connect(s, (struct sockaddr *)&cli, sizeof(cli))) < 0) { //PER il client non serve fare il BIND ma si può fare se si vuole per specificare la porta. Altrimenti lo fa da
        //solo prendendo una porta casuale come input
		perror("Connect: ");
		close(s);
		exit(1);
		}

	/* now the server is connected */
	while((n = read(fd, buf, sizeof(buf))) > 0) {
		write(s, buf, n);
		}

	printf("Client is done\n");
	close(s);
	exit(0);
}



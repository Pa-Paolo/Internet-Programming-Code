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
	sin.sin_port = htons((u_short)port); 	/* assure the big endianity ! */

	type= SOCK_STREAM;			/* A TCP socket is desired */
	protocol = 6;
	s = socket(PF_INET, type, protocol);
	if(s < 0) {
		perror("Can't create socket ");
		exit(1);
		}

	if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
		printf("Can't bind the port no. %d\n", port);
		close(s);
		exit(1);
		}
	if(listen(s, 1) < 0) {
		printf("Can't listen\n");
		close(s);
		exit(1);
		}
	/* Now wait for an incoming connection request */
	if((cs = accept(s, (struct sockaddr *)&cli, (unsigned *)&n)) < 0) {
		perror("Accept: ");
		close(s);
		exit(1);
		}

	/* A client is arrived ... now serve it */
	close(s);
	while((n = read(cs, buf, sizeof(buf))) > 0) {
		write(STDOUT_FILENO, buf, n);
		}

	printf("Server is done\n");
	close(cs);
	exit(0);

}



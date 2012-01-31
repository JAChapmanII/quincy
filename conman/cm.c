#include <stdio.h>
#include <stdlib.h>

#include "ircsock.h"

#define DEFAULT_SERVER "irc.freenode.net"
#define DEFAULT_NICK   "quincy"
#define DEFAULT_PORT   6667
#define DEFAULT_CHAN   "#zebra"

int main(int argc, char **argv) {
	char *server = (argc > 1) ? argv[1] : DEFAULT_SERVER,
		*nick = (argc > 2) ? argv[2] : DEFAULT_NICK,
		*chan = (argc > 3) ? argv[3] : DEFAULT_CHAN;
	int port = DEFAULT_PORT;
	if(argc > 4) {
		port = atoi(argv[4]);
		port = (port == 0) ? DEFAULT_PORT : port;
	}

	printf("Creating isock...\n");
	IRCSock *isock = ircsock_create(server, port, nick);
	if(isock == NULL) {
		fprintf(stderr, "main: could not create isock\n");
		return 1;
	}
	printf("Connecting %s@%s:%d...\n", nick, server, port);
	if(ircsock_connect(isock) != 0) {
		fprintf(stderr, "main: isock could not connect\n");
		return 2;
	}
	printf("Joining %s...\n", chan);
	if(ircsock_join(isock, chan)) {
		fprintf(stderr, "main: isock could not join\n");
		return 3;
	}
	printf("Sending pmsg...\n");
	ircsock_pmsg(isock, "#zebra", "Hello, there!");
	for(int i = 0; i < 3; ++i)
		usleep(1000*1000);

	printf("Quiting...\n");
	ircsock_quit(isock);
	usleep(1000*1000);
	ircsock_free(isock);
	return 0;
}


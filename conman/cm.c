#include <stdio.h>

#include "ircsock.h"

int main(int argc, char **argv) {
	printf("Creating isock...\n");
	IRCSock *isock = ircsock_create("irc.freenode.net", 6667, "quincy");
	if(isock == NULL) {
		fprintf(stderr, "main: could not create isock\n");
		return 1;
	}
	printf("Connecting isock...\n");
	if(ircsock_connect(isock) != 0) {
		fprintf(stderr, "main: isock could not connect\n");
		return 2;
	}
	printf("Joining isock...\n");
	if(ircsock_join(isock, "#zebra")) {
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


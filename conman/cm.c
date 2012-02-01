#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include "ircsock.h"

#define DEFAULT_SERVER "irc.freenode.net"
#define DEFAULT_NICK   "quincy"
#define DEFAULT_PORT    6667
#define DEFAULT_CHAN   "#zebra"
#define DEFAULT_BINARY "quincy"

int closePipe(int *fds);
int fileExists(char *fileName);
int subprocessPipe(char *binary, char **argv, int *fds);

int closePipe(int *fds) { // {{{
	int f1 = close(fds[0]), f2 = close(fds[2]);
	return f1 || f2;
} // }}}
int fileExists(char *fileName) { // {{{
	FILE *bin = fopen(fileName, "rb");
	if(bin == NULL)
		return 0;
	fclose(bin);
	return 1;
} // }}}

int subprocessPipe(char *binary, char **argv, int *fds) { // {{{
	// if the binary doesn't exist, abort
	if(!fileExists(binary))
		return -1;
	argv[0] = binary;

	int left[2] = { 0 }, right[2] = { 0 };
	// if we can't create the left pipe, abort
	int fail = pipe(left);
	if(fail)
		return fail;
	// if we can't create the right pipe, abort
	fail = pipe(right);
	if(fail) {
		closePipe(left);
		return fail;
	}

	// if we can't fork, abort
	pid_t pid = fork();
	if(pid == -1) {
		closePipe(left);
		closePipe(right);
		return -2;
	}

	// if we're the child, execv the binary
	if(pid == 0) {
		dup2(left[0], 0);
		closePipe(left);

		dup2(right[1], 1);
		closePipe(right);

		execv(binary, argv);
		return -99;
	}

	// if we're main, close uneeded ends and copy fds
	close(left[0]);
	close(right[1]);
	fds[0] = right[0];
	fds[1] = left[1];
	return 0;
} // }}}

int main(int argc, char **argv) {
	char *server = (argc > 1) ? argv[1] : DEFAULT_SERVER,
		*nick = (argc > 2) ? argv[2] : DEFAULT_NICK,
		*chan = (argc > 3) ? argv[3] : DEFAULT_CHAN,
		*binary = (argc > 4) ? argv[4] : DEFAULT_BINARY;
	int port = DEFAULT_PORT;
	if(argc > 5) {
		port = atoi(argv[5]);
		port = (port == 0) ? DEFAULT_PORT : port;
	}
	if(!fileExists(binary)) {
		fprintf(stderr, "main: %s: file does not exist\n", binary);
		return 1;
	}

	printf("Creating isock...\n");
	IRCSock *isock = ircsock_create(server, port, nick);
	if(isock == NULL) {
		fprintf(stderr, "main: could not create isock\n");
		return 2;
	}
	printf("Connecting %s@%s:%d...\n", nick, server, port);
	if(ircsock_connect(isock) != 0) {
		fprintf(stderr, "main: isock could not connect\n");
		return 3;
	}
	printf("Joining %s...\n", chan);
	if(ircsock_join(isock, chan)) {
		fprintf(stderr, "main: isock could not join\n");
		return 4;
	}

	int fds[2] = { 0 };
	if(subprocessPipe(binary, argv, fds) != 0) {
		fprintf(stderr, "main: couldn't create subprocess pipe to: %s\n", binary);
		return 5;
	}
	FILE *in = fdopen(fds[0], "r"), *out = fdopen(fds[1], "w");

	char buf[4096] = { 0 };
	while(!feof(in)) {
		char *str = ircsock_read(isock);
		if(str != NULL) {
			if((str[0] == 'P') && (str[1] == 'I') &&
				(str[2] == 'N') && (str[3] == 'G') &&
				(str[4] == ' ') && (str[5] == ':')) {
				str[1] = 'O';
				ircsock_send(isock, str);
				free(str);
				str = NULL;
			}
		}
		if(str != NULL) {
			fprintf(out, "%s\n", str);
			free(str);
		}

		if(fgets(buf, 4096 - 1, in) == buf)
			printf("%s", buf);
	}
	closePipe(fds);



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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "ircsock.h"
#include "util.h"
#include "conf.h"

#define BUF_SIZE 4096

int main(int argc, char **argv) {
	conf_parseArguments(argv, argc);
	conf_read(NULL);
	char *server = conf_server(), *nick = conf_nick(), *chan = conf_chan(),
		*binary = conf_binary();
	int port = conf_port();

	if(!util_exists(binary)) {
		fprintf(stderr, "main: %s: file does not exist\n", binary);
		return 1;
	}

	printf("Creating subprocess...\n");
	int fds[2] = { 0 };
	if(util_subprocessPipe(binary, argv, fds) != 0) {
		fprintf(stderr, "main: couldn't create subprocess pipe to: %s\n", binary);
		return 5;
	}
	usleep(10000);
	util_setNonBlocking(fds[0]);
	if(errno)
		perror("main");
	FILE *out = fdopen(fds[1], "w");

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

	char buf[BUF_SIZE] = { 0 };
	int done = 0;
	while(!done) {
		int didSomething = 0;

		char *str = NULL;
		while((str = ircsock_read(isock)) != NULL) {
			didSomething = 1;
			if((str[0] == 'P') && (str[1] == 'I') &&
				(str[2] == 'N') && (str[3] == 'G') &&
				(str[4] == ' ') && (str[5] == ':')) {
				str[1] = 'O';
				ircsock_send(isock, str);
				free(str);
				str = NULL;
			}

			if(str != NULL) {
				fprintf(out, "%s\n", str);
				fflush(out);
				free(str);
			}
		}

		size_t blen = strlen(buf);
		ssize_t ramount = read(fds[0], buf + blen, BUF_SIZE - blen);
		if(ramount < 0) {
			if(!((errno == EAGAIN) || (errno == EWOULDBLOCK)))
				perror("ircsock_read");
		} else if(ramount == 0) {
			; // TODO: handle EOF from subproc
		} else {
			didSomething = 1;
			char *line = NULL;
			while((line = util_fetch(buf, BUF_SIZE, "\n")) != NULL) {
				ircsock_send(isock, line);
				free(line);
			}
		}

		if(!didSomething)
			usleep(1000);
	}
	util_closePipe(fds);

	printf("Quiting...\n");
	ircsock_quit(isock);
	usleep(1000*1000);
	ircsock_free(isock);
	return 0;
}


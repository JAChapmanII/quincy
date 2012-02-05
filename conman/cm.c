#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "ircsock.h"
#include "conf.h"

#define BUF_SIZE 4096

int closePipe(int *fds);
int fileExists(char *fileName);
int subprocessPipe(char *binary, char **argv, int *fds);

int setNonBlocking(int fd);
int readReady(int fd);
char *fetch(char *buf, size_t bufSize, char *split);

int closePipe(int *fds) { // {{{
	int f1 = close(fds[0]), f2 = close(fds[1]);
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
		close(0);
		dup2(left[0], 0);
		closePipe(left);

		close(1);
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

int setNonBlocking(int fd) { // {{{
	int ss = fcntl(fd, F_GETFL, 0);
	return fcntl(fd, F_SETFL, ss | O_NONBLOCK);
} // }}}
int readReady(int fd) { // {{{
	fprintf(stderr, "readReady: start\n");
	fd_set rset;
	FD_ZERO(&rset);
	FD_SET(fd, &rset);

	fprintf(stderr, "readReady: end\n");
	int count = select(fd + 1, &rset, NULL, NULL, NULL);
	if(count < 0) {
		perror("readReady");
		return 0;
	}
	fprintf(stderr, "readReady: back\n");
	return FD_ISSET(fd, &rset);
} // }}}
char *fetch(char *buf, size_t bufSize, char *split) { // {{{
	char *lb = strstr(buf, split);
	if(lb == NULL)
		return NULL;

	size_t llen = lb - buf;
	char *line = calloc(llen + 1, 1);
	if(!line) {
		fprintf(stderr, "fetch: calloc failure\n");
		return NULL;
	}

	strncpy(line, buf, llen);
	line[llen] = '\0';

	size_t offset = 0, splen = strlen(split);
	while(offset + splen + llen < bufSize) {
		buf[offset] = buf[offset + llen + splen];
		offset++;
	}
	while(offset < bufSize)
		buf[offset++] = '\0';

	return line;
} // }}}

int main(int argc, char **argv) {
	conf_parseArguments(argv, argc);
	conf_read(NULL);
	char *server = conf_server(), *nick = conf_nick(), *chan = conf_chan(),
		*binary = conf_binary();
	int port = conf_port();

	if(!fileExists(binary)) {
		fprintf(stderr, "main: %s: file does not exist\n", binary);
		return 1;
	}

	printf("Creating subprocess...\n");
	int fds[2] = { 0 };
	if(subprocessPipe(binary, argv, fds) != 0) {
		fprintf(stderr, "main: couldn't create subprocess pipe to: %s\n", binary);
		return 5;
	}
	usleep(10000);
	setNonBlocking(fds[0]);
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
			while((line = fetch(buf, BUF_SIZE, "\n")) != NULL) {
				ircsock_send(isock, line);
				free(line);
			}
		}

		if(!didSomething)
			usleep(1000);
	}
	closePipe(fds);

	printf("Quiting...\n");
	ircsock_quit(isock);
	usleep(1000*1000);
	ircsock_free(isock);
	return 0;
}


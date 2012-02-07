#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ircsock.h"
#include "subprocess.h"
#include "util.h"
#include "conf.h"

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

	int done = 0;
	Subprocess *subproc = NULL;
	FILE *out = NULL;
	printf("Entering main loop...\n");
	while(!done) {
		int didSomething = 0;

		if(subproc == NULL) {
			printf("Creating subprocess...\n");
			subproc = subprocess_create(binary, argv + 1, argc - 1);
			if(!subproc) {
				fprintf(stderr, "main: couldn't create subprocess object\n");
				return 5;
			}
			if(subprocess_run(subproc) != 0) {
				fprintf(stderr, "main: couldn't invoke subprocess: %s\n", binary);
				return 6;
			}
			out = subprocess_wfile(subproc);
			didSomething = 1;
			usleep(10000);
		}

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

		char *line = NULL;
		while((line = bufreader_read(subproc->br)) != NULL) {
			ircsock_send(isock, line);
			free(line);
		}
		if(subproc->br->eof) {
			printf("main: subproc has returned EOF\n");
			fclose(out);
			subprocess_free(subproc);
			subproc = NULL;
		}

		if(!didSomething)
			usleep(1000);
	}
	util_closePipe(subproc->pipe);

	printf("Quiting...\n");
	ircsock_quit(isock);
	usleep(1000*1000);
	ircsock_free(isock);
	return 0;
}


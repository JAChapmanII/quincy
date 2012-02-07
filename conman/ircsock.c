#include "ircsock.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

// Size of character buffers
#define BCSIZE 1024
// Size of line buffers
#define BLSIZE 4096

/* Function used to get address information from a domain and port
 *
 * Returns a valid addrinfo *, or
 * 	NULL on failure, prints error to stderr
 */
struct addrinfo *ircsock_lookupDomain(IRCSock *ircsock);

IRCSock *ircsock_create(char *host, int port, char *nick) { /*{{{*/
	IRCSock *ircsock = malloc(sizeof(IRCSock));
	if(!ircsock)
		return NULL;

	ircsock->host = NULL;
	ircsock->nick = NULL;
	ircsock->chan = NULL;

	ircsock->host = malloc(strlen(host) + 1);
	if(!ircsock->host) {
		ircsock_free(ircsock);
		return NULL;
	}
	strcpy(ircsock->host, host);

	ircsock->port = port;
	ircsock->domain = -1;

	ircsock->nick = malloc(strlen(nick) + 1);
	if(!ircsock->nick) {
		ircsock_free(ircsock);
		return NULL;
	}
	strcpy(ircsock->nick, nick);

	ircsock->socket = -1;

	memset(ircsock->buf, '\0', IRCSOCK_BUF_SIZE);
	memset(ircsock->wbuf, '\0', IRCSOCK_BUF_SIZE);

	return ircsock;
} /*}}}*/

/* TODO: handle disconnecting */
void ircsock_free(IRCSock *ircsock) { /*{{{*/
	if(!ircsock)
		return;
	free(ircsock->host);
	free(ircsock->nick);
	free(ircsock->chan);
	free(ircsock);
} /*}}}*/

struct addrinfo *ircsock_lookupDomain(IRCSock *ircsock) { // {{{
	// if we don't yet have a socket, there is obviously a problem
	if(ircsock->socket == -1) {
		fprintf(stderr, "ircsock_lookupDomain: socket is nonexistant\n");
		return NULL;
	}

	// get the string version of our port number
	char sport[BCSIZE];
	snprintf(sport, BCSIZE, "%d", ircsock->port);

	// try to get the address info
	struct addrinfo *result;
	int error = getaddrinfo(ircsock->host, sport, NULL, &result);

	// if we failed, report the error and abort
	if(error) {
		fprintf(stderr, "ircsock_lookupDomain: failed lookup domain: %d\n", error);
		return NULL;
	}

	return result;
} // }}}

int ircsock_connect(IRCSock *ircsock) { // {{{
	// attempt to create socket
	ircsock->socket = socket(AF_INET, SOCK_STREAM, 0);
	if(ircsock->socket == -1) {
		fprintf(stderr, "ircsock_connect: failed to create socket\n");
		return 1;
	}

	// lookup address info for host
	struct addrinfo *result = ircsock_lookupDomain(ircsock);
	if(!result)
		return 2;

	// connect to host
	int error = connect(ircsock->socket, result->ai_addr, result->ai_addrlen);
	if(error == -1) {
		perror("ircsock_connect");
		return 3;
	}
	freeaddrinfo(result);

	// set socket to non-blocking mode
	int ss = fcntl(ircsock->socket, F_GETFL, 0);
	fcntl(ircsock->socket, F_SETFL, ss | O_NONBLOCK);

	// allocate space for NICK IRC command
	char *nickc = malloc(16 + strlen(ircsock->nick));
	if(nickc == NULL) {
		fprintf(stderr, "ircsock_connect: malloc for nickc failed\n");
		return IRCSOCK_MERROR;
	}

	// allocate space for USER IRC command
	char *userc = malloc(16 + strlen(ircsock->nick) * 2);
	if(userc == NULL) {
		fprintf(stderr, "ircsock_connect: malloc for userc failed\n");
		free(nickc);
		return IRCSOCK_MERROR;
	}

	// construct NICK command
	strcpy(nickc, "NICK ");
	strcat(nickc, ircsock->nick);

	// construct USER command
	strcpy(userc, "USER ");
	strcat(userc, ircsock->nick);
	strcat(userc, " j j :");
	strcat(userc, ircsock->nick);

	// send NICK followed by USER
	ircsock_send(ircsock, nickc);
	usleep(10000);
	ircsock_send(ircsock, userc);
	usleep(10000);

	// loop until we receive and error or the connected-go-ahead
	int done = 0;
	while(!done) {
		char *str = ircsock_read(ircsock);
		// if we recieved a string
		if(str != NULL) {
			// if we see nick in use, abort
			if(strstr(str, " 433 ")) {
				fprintf(stderr, "ircsock_connect: nick in use!\n");
				return 433;
			}
			// if we recieve the nick invalid message, abort
			if(strstr(str, " 432 ")) {
				fprintf(stderr, "ircsock_connect: nick contains illegal charaters\n");
				return 432;
			}
			// if we see the end of motd code, we're in
			if(strstr(str, " 376 ")) {
				free(str);
				return 0;
			}
			// respond to PINGs
			if((str[0] == 'P') && (str[1] == 'I') &&
				(str[2] == 'N') && (str[3] == 'G') &&
				(str[4] == ' ') && (str[5] == ':')) {
				str[1] = 'O';
				ircsock_send(ircsock, str);
			}
			free(str);
		} else {
			// if we didn't recieve a string, wait a bit
			usleep(1000);
		}
	}

	// error out (we shouldn't ever get here)
	return -1;
} // }}}

char *ircsock_read(IRCSock *ircsock) { // {{{
	size_t blen = strlen(ircsock->buf);
	char *line = util_fetch(ircsock->buf, IRCSOCK_BUF_SIZE, "\r\n");
	if(line != NULL)
		return line;

	ssize_t ramount = read(ircsock->socket, ircsock->buf + blen,
			IRCSOCK_BUF_SIZE - blen);
	if((ramount < 0) && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
		return NULL;
	if(ramount < 0) {
		perror("ircsock_read");
		return NULL;
	}

	line = util_fetch(ircsock->buf, IRCSOCK_BUF_SIZE, "\r\n");
	return line;
} // }}}

ssize_t ircsock_send(IRCSock *ircsock, char *str) { // {{{
	size_t slen = strlen(str), wblen = strlen(ircsock->wbuf);
	if(wblen + slen + 2 >= IRCSOCK_BUF_SIZE) {
		fprintf(stderr, "ircsock_send: cannot cat str to wbuf, no space\n");
		return -1;
	}

	strcat(ircsock->wbuf, str);
	strcat(ircsock->wbuf, "\r\n");
	wblen += slen + 2;

	ssize_t wamount = write(ircsock->socket, ircsock->wbuf, wblen);
	if((wamount < 0) && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
		return 0;

	if(wamount < 0) {
		perror("ircsock_send");
		return wamount;
	} else if(wamount > 0) {
		size_t offset = 0;
		while(offset + wamount < IRCSOCK_BUF_SIZE) {
			ircsock->wbuf[offset] = ircsock->wbuf[offset + wamount];
			offset++;
		}
		while(offset < IRCSOCK_BUF_SIZE)
			ircsock->wbuf[offset++] = '\0';
	}

	return wamount;
} // }}}

ssize_t ircsock_pmsg(IRCSock *ircsock, char *target, char *msg) { /*{{{*/
	char *buf = malloc(strlen(msg) + strlen("PRIVMSG ") + strlen(target) + 3);
	if(!buf) {
		fprintf(stderr, "ircsock_pmsg: malloc failed\n");
		return IRCSOCK_MERROR;
	}
	strcpy(buf, "PRIVMSG ");
	strcat(buf, target);
	strcat(buf, " :");
	strcat(buf, msg);
	return ircsock_send(ircsock, buf);
} /*}}}*/

int ircsock_join(IRCSock *ircsock, char *chan) { /*{{{*/
	if(ircsock->chan != NULL)
		free(ircsock->chan);

	ircsock->chan = calloc(strlen(chan) + 1, 1);
	if(!ircsock->chan) {
		return 5;
	}
	strcpy(ircsock->chan, chan);

	/* these use 16 as it should cover needed space for 4 letter command, a
	 * space, and extra arguments besides nick/chan */
	char *joinc = malloc(16 + strlen(ircsock->chan));

	if(!joinc) {
		fprintf(stderr, "ircsock_join: malloc failed\n");
		return IRCSOCK_MERROR;
	}

	strcpy(joinc, "JOIN ");
	strcat(joinc, ircsock->chan);

	usleep(100000);

	ircsock_send(ircsock, joinc);

	int done = 0;
	while(!done) {
		char *str = ircsock_read(ircsock);
		if(str != NULL) {
			if(strstr(str, " 332 ") || strstr(str, " JOIN ")) {
				free(str);
				return 0;
			}
			if((str[0] == 'P') && (str[1] == 'I') &&
				(str[2] == 'N') && (str[3] == 'G') &&
				(str[4] == ' ') && (str[5] == ':')) {
				str[1] = 'O';
				ircsock_send(ircsock, str);
			}
			free(str);
		} else {
			// if we didn't recieve a string, wait a bit
			usleep(1000);
		}
	}

	return -1;
} /*}}}*/

int ircsock_quit(IRCSock *ircsock) { /*{{{*/
	return ircsock_send(ircsock, "QUIT");
} /*}}}*/


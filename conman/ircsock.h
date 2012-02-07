#ifndef IRCSOCK_H
#define IRCSOCK_H

#include <sys/types.h>
#include "bufreader.h"

#define IRCSOCK_MERROR   -2
#define IRCSOCK_BUF_SIZE 4096

/* IRCSock is a structure used to maintain a bidirectional connection to an
 * IRC server. It provides facilities to resolve the domain name, connect the
 * internal socket, joining a server and room and sending and recieving data.
 *
 * It will eventually handle more errors and the majority of the IRC protocol,
 * but currently it only handles failing to join due to the username being in
 * use.
 */
typedef struct {
	char *host;
	int port;
	int domain;
	int socket;

	char *nick;

	char *chan;

	BufReader *br;
	char wbuf[IRCSOCK_BUF_SIZE];
} IRCSock;

/* Create an IRCSock, copies all data into an IRCSock
 *
 * Returns pointer to new IRCSock, or
 * 	NULL on failure
 */
IRCSock *ircsock_create(char *host, int port, char *nick);

/* Frees the memory associated with an IRCSock.
 *
 * Note: Does not disconnect properly from connected socket
 */
void ircsock_free(IRCSock *ircsock);

/* Connect an IRCSock to its remote host
 *
 * Returns 0, or
 * 	an error value on failure
 */
int ircsock_connect(IRCSock *ircsock);

/* Does a nonblocking read on the socket.
 *
 * Returns a null-terminated string the user is required to free, or
 * 	NULL if there was no data to be returned
 */
char *ircsock_read(IRCSock *ircsock);

/* Write data to socket, ending with required \r\n
 *
 * Returns amount written (expect 2 + strlen(str)), or
 * 	0 on delayed write
 * 	-1 on write error (sets errno)
 */
ssize_t ircsock_send(IRCSock *ircsock, char *str);

/* Formats a PRIVMSG command using target and message, then sends it
 *
 * Returns amount sent (expect strlen("PRIVMSG " ~ target ~ " :" ~ msg) + 2), or
 * 	0 on delayed write
 * 	-1 on write error (sets errno)
 * 	IRCSOCK_MERROR on malloc error
 */
ssize_t ircsock_pmsg(IRCSock *ircsock, char *target, char *msg);

/* Blocks until join to server/room completes or fails
 *
 * Returns 0 on success, or
 * 	IRCSOCK_MERROR on failed malloc (prints to stderr)
 * 	other value -- on corresponding IRC response code
 */
int ircsock_join(IRCSock *ircsock, char *chan);

/* Sends QUIT command through socket
 *
 * Returns 6 on success (strlen("QUIT") + 2), or
 * 	0 on delayed write
 * 	-1 on failure (sets errno)
 */
int ircsock_quit(IRCSock *ircsock);

#endif /* IRCSOCK_H */

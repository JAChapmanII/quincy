#ifndef BUFREADER_H
#define BUFREADER_H

#include <sys/types.h>

// BufReader provides buffered read support from a file descriptor.
typedef struct {
	int fd;
	char *split;
	size_t bufSize;
	char *buf;
	size_t blen;
	int eof;
} BufReader;

// Construct a BufReader with a given signal string and buffer size
BufReader *bufreader_create(int fd, char *split, size_t bufSize);
// Free memory associated with a BufReader and close the backing fd
void bufreader_free(BufReader *br);

/* Perform a nonblocking read on the underlying fd.
 *
 * If the split string was found in the buffer (or one was read), this returns
 * a valid stirng owned by the caller.
 */
char *bufreader_read(BufReader *br);

#endif // BUFREADER_H

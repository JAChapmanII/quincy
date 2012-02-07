#ifndef BUFREADER_H
#define BUFREADER_H

#include <sys/types.h>

typedef struct {
	int fd;
	char *split;
	size_t bufSize;
	char *buf;
	size_t blen;
	int eof;
} BufReader;

BufReader *bufreader_create(int fd, char *split, size_t bufSize);
void bufreader_free(BufReader *br);

char *bufreader_read(BufReader *br);

#endif // BUFREADER_H

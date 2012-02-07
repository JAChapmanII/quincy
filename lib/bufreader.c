#include "bufreader.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include "util.h"

#define MIN_BUF_SIZE 1024

BufReader *bufreader_create(int fd, char *split, size_t bufSize) {
	if(!split || (strlen(split) < 1))
		return NULL;
	BufReader *br = malloc(sizeof(BufReader));
	if(br == NULL)
		return NULL;
	br->bufSize = (bufSize < MIN_BUF_SIZE) ? MIN_BUF_SIZE : bufSize;
	br->buf = calloc(br->bufSize, 1);
	if(br->buf == NULL) {
		bufreader_free(br);
		return NULL;
	}
	br->blen = 0;

	br->split = strdup(split);
	if(br->split == NULL) {
		bufreader_free(br);
		return NULL;
	}

	br->eof = 0;
	br->fd = fd;
	util_setNonBlocking(br->fd);
	return br;
}
void bufreader_free(BufReader *br) {
	if(!br)
		return;
	if(br->buf)
		free(br->buf);
	if(br->split)
		free(br->split);
	close(br->fd);
	free(br);
}

char *bufreader_read(BufReader *br) {
	// TODO: persistent?
	br->blen = strlen(br->buf);

	char *line = util_fetch(br->buf, br->bufSize, br->split);
	if(line != NULL)
		return line;
	if(br->eof)
		return NULL;

	ssize_t ramount = read(br->fd, br->buf + br->blen, br->bufSize - br->blen);
	if((ramount < 0) && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
		return NULL;
	if(ramount == 0) {
		if(!util_endsWith(br->buf, br->split)) {
			fprintf(stderr, "bufreader_read: EOF reached, end(buf) != split\n");
			if(br->blen + strlen(br->split) >= br->bufSize) {
				fprintf(stderr, "bufreader_read: but no room to append split\n");
				return NULL;
			}
			strcat(br->buf, br->split);
		}
		br->eof = 1;
		return NULL;
	}
	if(ramount < 0) {
		perror("subprocess_read");
		return NULL;
	}

	line = util_fetch(br->buf, br->bufSize, br->split);
	return line;
}


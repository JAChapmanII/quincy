#include "util.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

size_t util_writeSizeT(FILE *outFile, size_t toWrite) { // {{{
	if(!outFile)
		return 0;

	uint8_t buf[sizeof(size_t)] = { 0 };
	for(size_t i = 0; i < sizeof(size_t); ++i)
		buf[i] = (toWrite >> (8*i)) & 0xff;

	return fwrite(buf, 1, sizeof(size_t), outFile);
} // }}}
size_t util_readSizeT(FILE *inFile) { // {{{
	uint8_t buf[sizeof(size_t)] = { 0 };
	size_t read = fread(buf, 1, sizeof(size_t), inFile);
	if(read != sizeof(size_t))
		return 0;

	size_t result = 0;
	for(size_t i = sizeof(size_t); i > 0; --i)
		result = (result << 8) + buf[i - 1];

	return result;
} // }}}

int util_closePipe(int *fds) { // {{{
	int f1 = close(fds[0]), f2 = close(fds[1]);
	return f1 || f2;
} // }}}
int util_exists(const char *fileName) { // {{{
	FILE *bin = fopen(fileName, "rb");
	if(bin == NULL)
		return 0;
	fclose(bin);
	return 1;
} // }}}

int util_setNonBlocking(int fd) { // {{{
	int ss = fcntl(fd, F_GETFL, 0);
	return fcntl(fd, F_SETFL, ss | O_NONBLOCK);
} // }}}

char *util_fetch(char *buf, size_t bufSize, char *split) { // {{{
	char *lb = strstr(buf, split);
	if(lb == NULL)
		return NULL;

	size_t llen = lb - buf;
	char *line = calloc(llen + 1, 1);
	if(!line) {
		fprintf(stderr, "util_fetch: calloc failure\n");
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

int util_startsWith(char *str, char *begin) { // {{{
	if(!str || !begin)
		return 0;
	if(strlen(begin) == 0)
		return 1;
	if(strlen(str) == 0)
		return 0;
	return (strncmp(str, begin, strlen(begin)) == 0);
} // }}}

char *util_substr(char *str, size_t beg, size_t len) { // {{{
	if((str == NULL) || (len == 0))
		return NULL;
	ssize_t slen = strlen(str);
	if((slen - (ssize_t)(len + beg) < 0) || ((ssize_t)beg >= slen))
		return NULL;

	char *sub = calloc(len, 1);
	strncpy(sub, str + beg, len);

	sub[len] = '\0';
	return sub;
} // }}}
char *util_strend(char *str, size_t beg) { // {{{
	if(str == NULL)
		return NULL;
	ssize_t slen = strlen(str);
	if((ssize_t)beg >= slen)
		return NULL;
	return util_substr(str, beg, slen - beg);
} // }}}


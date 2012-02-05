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

int util_subprocessPipe(const char *binary, char **argv, int *fds) { // {{{
	// if the binary doesn't exist, abort
	if(!util_exists(binary))
		return -1;
	argv[0] = (char *)binary;

	int left[2] = { 0 }, right[2] = { 0 };
	// if we can't create the left pipe, abort
	int fail = pipe(left);
	if(fail)
		return fail;
	// if we can't create the right pipe, abort
	fail = pipe(right);
	if(fail) {
		util_closePipe(left);
		return fail;
	}

	// if we can't fork, abort
	pid_t pid = fork();
	if(pid == -1) {
		util_closePipe(left);
		util_closePipe(right);
		return -2;
	}

	// if we're the child, execv the binary
	if(pid == 0) {
		dup2(left[0], 0);
		util_closePipe(left);

		dup2(right[1], 1);
		util_closePipe(right);

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


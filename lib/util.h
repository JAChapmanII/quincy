#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdint.h>

size_t util_writeSizeT(FILE *outFile, size_t toWrite);
size_t util_readSizeT(FILE *inFile);

int util_closePipe(int *fds);
int util_exists(const char *fileName);
int util_subprocessPipe(const char *binary, char **argv, int *fds);

int util_setNonBlocking(int fd);
int util_blockreadReady(int fd);

char *util_fetch(char *buf, size_t bufSize, char *split);

#endif // UTIL_H

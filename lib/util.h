#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdint.h>

// Write a size_t to a file in a endian independant way
size_t util_writeSizeT(FILE *outFile, size_t toWrite);
// Read a size_t from a file in a endian independant way
size_t util_readSizeT(FILE *inFile);

// Close both ends of the passed pipe
int util_closePipe(int *fds);
// Check to see if a file exists (that is, if we can open it for reading)
int util_exists(const char *fileName);

// Set a file descriptor to non blocking mode
int util_setNonBlocking(int fd);

/* Extract a string from the specified buf
 * 	buf: the string buffer
 * 	bufSize: the size of the buffer
 * 	split: the string to split the buf on
 *
 * If split is found in buf, then the part leading up to split is returned as
 * a string (owned by caller). The rest of the buf (minus the split string) is
 * then moved back, padding the end of buf with null bytes.
 */
char *util_fetch(char *buf, size_t bufSize, char *split);

// Returns true if a given string starts with another given string
int util_startsWith(char *str, char *begin);
// Returns true if a given string ends with another given string
int util_endsWith(char *str, char *end);

// Returns a copy of a substring
char *util_substr(char *str, size_t beg, size_t len);
// Returns a copy of the end portion of a string
char *util_strend(char *str, size_t beg);

#endif // UTIL_H

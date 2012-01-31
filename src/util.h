#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdint.h>

size_t writeSizeT(FILE *outFile, size_t toWrite);
size_t readSizeT(FILE *inFile);

#endif // UTIL_H

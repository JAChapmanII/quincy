#include "util.h"

size_t writeSizeT(FILE *outFile, size_t toWrite) { // {{{
	if(!outFile)
		return 0;

	uint8_t buf[sizeof(size_t)] = { 0 };
	for(size_t i = 0; i < sizeof(size_t); ++i)
		buf[i] = (toWrite >> (8*i)) & 0xff;

	return fwrite(buf, 1, sizeof(size_t), outFile);
} // }}}

size_t readSizeT(FILE *inFile) { // {{{
	uint8_t buf[sizeof(size_t)] = { 0 };
	size_t read = fread(buf, 1, sizeof(size_t), inFile);
	if(read != sizeof(size_t))
		return 0;

	size_t result = 0;
	for(size_t i = sizeof(size_t); i > 0; --i)
		result = (result << 8) + buf[i - 1];

	return result;
} // }}}


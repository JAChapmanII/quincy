#include "status.h"
#include <stdio.h>

int moduleError(int statusCode, int errorCode) {
	switch(statusCode) {
		case EINIT_FAILURE:
			fprintf(stderr, "module: error: init failure [%d]\n", errorCode);
			break;
		case ENULL_NAMES:
			fprintf(stderr, "module: error: moduleNames returned NULL\n");
			break;
		case ENO_MODULES:
			fprintf(stderr, "module: error: no supported modules\n");
			break;
		case ENULL_REGEX:
			fprintf(stderr, "module: error: moduleRegex returned NULL\n");
			break;
		case EMODNEREGEX:
			fprintf(stderr, "module: error: regex count not equal to module count\n");
			break;
		case EEXIT_FAILURE:
			fprintf(stderr, "module: error: cleanup failed [%d]\n", errorCode);
			break;
		default:
			fprintf(stderr, "module: error: succes? [%d, %d]\n",
					statusCode, errorCode);
			break;
	}
	return statusCode;
}


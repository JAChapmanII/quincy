#include "base.h"
#include <stdlib.h>

static char *m_names[] = { NULL };
static char *m_regex[] = { NULL };

int moduleInit(void) {
	return 0;
}
int moduleCleanup(void) {
	return 0;
}

char **moduleNames(void) {
	return m_names;
}
char **moduleRegex(void) {
	return m_regex;
}
char *moduleHelp(int module) {
	switch(module) {
		default:
			return "";
	}
}

char *dispatch(int module) {
	switch(module) {
		default:
			return "";
	}
}


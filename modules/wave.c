#include "base.h"
#include <stdlib.h>
#include <time.h>

static char *m_names[] = {       "wave", NULL };
static char *m_regex[] = { "^(o/|\\o)$", NULL };

int moduleInit(void) {
	srand(time(NULL));
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
			return "Waves to people";
	}
}
char *dispatch(int module) {
	switch(module) {
		default:
			if(rand() % 2)
				return "\\o";
			else
				return "o/";
	}
}


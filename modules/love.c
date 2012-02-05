#include "base.h"
#include <stdlib.h>

#define BUF_SIZE 4096

static char *m_names[] = { "love", NULL };
static char *m_regex[] = {   "<3", NULL };

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
			return "Provide artificial love";
	}
}

char *dispatch(int module) {
	switch(module) {
		default:
			if(strlen(userNick) + strlen(": <3") >= BUF_SIZE)
				return "<3";
			char buf[BUF_SIZE] = { 0 };
			strcpy(buf, userNick);
			strcat(buf, ": <3");
			return buf;
	}
}


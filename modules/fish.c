#include "base.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BUF_SIZE 4096

static char *m_names[] = {   "fish",   "fishes", NULL };
static char *m_regex[] = { "^fish$", "^fishes$", NULL };

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
			return "Breath life into the channel with some fish";
	}
}

char *dispatch(int module) {
	switch(module) {
		default:
			if(module == 0) {
				if(rand() % 2)
					return "<><";
				else
					return "><>";
			}
			int fcount = rand() % 5 + 2;
			char *buf = calloc(BUF_SIZE, 1);
			if(!buf)
				return "<>< <><";
			for(int f = 0; f < fcount; ++f) {
				if(rand() % 2)
					strcat(buf, "<><");
				else
					strcat(buf, "><>");
				int sc = rand() % 3;
				for(int s = 0; s < sc; ++s)
					strcat(buf, " ");
			}
			return buf;
	}
}


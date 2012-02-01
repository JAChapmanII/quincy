#include <stdio.h>
#include <string.h>

#include "vmap.h"
#include "simap.h"
#include "ismap.h"

#define BUF_SIZE 4096

int main(int argc, char **argv) {
	fprintf(stderr, "quincy: 0.0a\n");

	VMap *vmap = vmap_create();
	if(!vmap) {
		fprintf(stderr, "main: failed to create vmap\n");
		return -1;
	}

	char buf[BUF_SIZE] = { 0 };
	while(!feof(stdin)) {
		if(fgets(buf, BUF_SIZE, stdin) == buf) {
			if(strstr(buf, "PRIVMSG")) {
				printf("PRIVMSG #zebra :Hello!\n");
				fflush(stdout);
			}
			memset(buf, '\0', BUF_SIZE);
		} else
			usleep(1000);
	}
	fprintf(stderr, "quincy: quiting\n");

	return 0;
}


#include <string.h>
#include <stdio.h>
#include <pcre.h>
#include "base.h"
#include "status.h"

/* This is a predifined main that should be useful for potential modules. The
 * module binaries are called by the quincy binary, so the arguments will come
 * frome it.
 *
 * The first argument is taken to be a particular function:
 * 	names: return a list of function names available here
 * 	regex: return a list of corresponding regex
 * 	help: return the help string for a particular module
 * 		the second argument here is the function name
 * 	test: try to compile all regex objects to see if any errors arise
 * 	dispatch: run a particular module function
 * 		2nd argument: user nick
 * 		3rd argument: user host mask
 * 		4th argument: target of PRIVMSG
 * 		5th argument: message portion of PRIVMSG
 */

int main(int argc, char **argv) {
	if(argc < 2)
		return 0;

	int initFailure = moduleInit();
	if(initFailure) {
		return moduleError(EINIT_FAILURE, initFailure);
	}

	int moduleCount = 0;
	char **names = moduleNames();
	if(names == NULL) {
		return moduleError(ENULL_NAMES, 0);
	}
	for(moduleCount = 0; names[moduleCount] != NULL; ++moduleCount)
		; // count modules
	if(moduleCount < 1) {
		return moduleError(ENO_MODULES, 0);
	}

	int regexCount = 0;
	char **regex = moduleRegex();
	if(regex == NULL) {
		return moduleError(ENULL_REGEX, 0);
	}
	for(regexCount = 0; regex[regexCount] != NULL; ++regexCount)
		; // count regex
	if(regexCount != moduleCount) {
		return moduleError(EMODNEREGEX, 0);
	}

	char *command = argv[1];
	if(strcmp(command, "names") == 0) {
		for(int module = 0; module < moduleCount; ++module)
			printf("%d: %s\n", module, names[module]);
	} else if(strcmp(command, "regex") == 0) {
		for(int module = 0; module < regexCount; ++module)
			printf("%d: %s\n", module, regex[module]);
	} else if(strcmp(command, "help") == 0) {
		if(argc != 3) {
			fprintf(stderr, "module: error: help takes module name\n");
			return 1;
		}
		char *module = argv[2];
		int modnum = 0;
		for(modnum = 0; modnum < moduleCount; ++modnum)
			if(strcmp(module, names[modnum]) == 0)
				break;
		char *help = moduleHelp(modnum);
		if((help == NULL) || (strlen(help) == 0)) {
			printf("%s: no help available\n", module);
		} else {
			printf("%s: %s\n", module, help);
		}
	} else if(strcmp(command, "test") == 0) {
		for(int module = 0; module < regexCount; ++module) {
			const char *errorMessage = NULL;
			int errorOffset = 0, retval = 0;
			pcre *re = pcre_compile(regex[module], 0,
					&errorMessage, &errorOffset, NULL);
			if(re == NULL) {
				fprintf(stderr, "module: regex error \"%s\"@%d: %s\n",
						regex[module], errorOffset, errorMessage);
				retval = EREGEX_ERROR;
			}
			if(retval == 0) {
				printf("module: no regex errors\n");
			}
			return retval;
		}
	} else if(strcmp(command, "dispatch") == 0) {
		if(argc != 6) {
			fprintf(stderr, "module: error: dispatch takes preparsed message\n");
			return 2;
		}
		userNick  = argv[2];
		userHMask = argv[3];
		target    = argv[4];
		message   = argv[5];

		for(int module = 0; module < regexCount; ++module) {
			const char *errorMessage = NULL;
			int errorOffset = 0;
			pcre *re = pcre_compile(regex[module], 0,
					&errorMessage, &errorOffset, NULL);
			if(re == NULL) {
				fprintf(stderr, "module: error compiling regex, %d: %s\n",
						errorOffset, errorMessage);
				return EREGEX_ERROR;
			}
			// TODO: try match
			// TODO: show output, stop
		}
	}

	int cleanupFailure = moduleCleanup();
	if(cleanupFailure) {
		return moduleError(EEXIT_FAILURE, cleanupFailure);
	}
	return SUCCESS;
}


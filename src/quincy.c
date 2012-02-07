#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pcre.h>

#include "vmap.h"
#include "conf.h"
#include "util.h"
#include "module.h"

#define BUF_SIZE 4096
#define MAX_SUBSTR 16
#define OVEC_COUNT (MAX_SUBSTR * 3)

char **pcre_match(pcre *regex, char *str, int *regres);
void freeMatchStrings(char **strings);

int main(int argc, char **argv) {
	conf_parseArguments(argv, argc);
	conf_read(NULL);
	char *nick = conf_nick(), *chan = conf_chan();

	fprintf(stderr, "quincy: 0.0a\n");
	fprintf(stderr, "quincy: { %s, %s }\n", nick, chan);

	VMap *confMap = conf_map();
	if(confMap == NULL) {
		confMap = vmap_create();
		if(confMap == NULL) {
			fprintf(stderr, "quincy: unable to create vmap\n");
			return 1;
		}
	}

	const char *errorMessage = NULL;
	int errorOffset = 0;

	VMap_Node *vmapn = vmap_find(confMap, "irc.pmsg");
	if(vmapn == NULL) {
		fprintf(stderr, "quincy: unable to find pmsg regex\n");
		return 2;
	}
	pcre *pmsg = pcre_compile(vmapn->val, 0,
			&errorMessage, &errorOffset, NULL);
	if(pmsg == NULL) {
		fprintf(stderr, "quincy: error creating pmsg pcre object: %d: %s\n",
				errorOffset, errorMessage);
		return 3;
	}

	vmapn = vmap_find(confMap, "irc.join");
	if(vmapn == NULL) {
		fprintf(stderr, "quincy: unable to find join regex\n");
		return 2;
	}
	pcre *join = pcre_compile(vmapn->val, 0,
			&errorMessage, &errorOffset, NULL);
	if(join == NULL) {
		fprintf(stderr, "quincy: error creating pmsg pcre object: %d: %s\n",
				errorOffset, errorMessage);
		return 3;
	}

	vmapn = vmap_find(confMap, "irc.quit");
	if(vmapn == NULL) {
		fprintf(stderr, "quincy: unable to find quit regex\n");
		return 2;
	}
	pcre *quit = pcre_compile(vmapn->val, 0,
			&errorMessage, &errorOffset, NULL);
	if(join == NULL) {
		fprintf(stderr, "quincy: error creating quit pcre object: %d: %s\n",
				errorOffset, errorMessage);
		return 3;
	}

	ModuleList *modules = modulelist_create();
	if(!modules) {
		fprintf(stderr, "quincy: unable to create module list\n");
		return 4;
	}
	VMap_Iterator *conf_it = vmapi_create();
	if(!conf_it) {
		fprintf(stderr, "quincy: unable to create conf_it\n");
		return 5;
	}

	char *moddir = NULL;
	VMap_Node *mdNode = vmap_find(confMap, "quincy.moduledir");
	if(mdNode != NULL)
		moddir = mdNode->val;

	for(vmapi_front(conf_it, confMap); conf_it->type != IT_END;
			vmapi_next(conf_it)) {
		if(!util_startsWith(conf_it->current->key, "modules."))
			continue;

		char *mname = util_strend(conf_it->current->key, strlen("modules."));
		Module *mod = module_create(mname, conf_it->current->val);
		if(mod == NULL) {
			fprintf(stderr, "quincy: couldn't create module: %s\n", mname);
			free(mname);
			continue;
		}
		free(mname);
		if(module_load(mod, moddir) != 0)
			modulelist_add(modules, mod);
		else
			module_free(mod);
	}

	time_t responseTimes[3] = { 0 };
	char buf[BUF_SIZE] = { 0 };
	while(!feof(stdin)) {
		memset(buf, '\0', BUF_SIZE);
		if(fgets(buf, BUF_SIZE, stdin) == buf) {
			int matchres = 0;
			char **strs = pcre_match(pmsg, buf, &matchres);
			if(matchres > 0) {
				for(int i = 0; i < matchres; ++i)
					fprintf(stderr, "%d: %s\n", i, strs[i]);
				printf("PRIVMSG #zebra :Hello!\n");
				fflush(stdout);
			}
			if(strs != NULL)
				freeMatchStrings(strs);
		} else
			usleep(1000);
	}
	fprintf(stderr, "quincy: quiting\n");

	modulelist_free(modules);
	return 0;
}

char **pcre_match(pcre *regex, char *str, int *regres) { // {{{
	if(!regex || !str)
		return NULL;
	int ovector[OVEC_COUNT] = { 0 };
	int matchres = pcre_exec(regex, NULL, str, strlen(str), 0, 0,
			ovector, OVEC_COUNT);
	if(matchres == 0) {
		fprintf(stderr, "pcre_match: ovector not large enough\n");
		if(regres != NULL)
			*regres = -1;
		return NULL;
	}
	if(matchres < 0) {
		switch(matchres) {
			case PCRE_ERROR_NOMATCH:
				if(regres != NULL)
					*regres = 0;
				return NULL;
			default:
				if(regres != NULL)
					*regres = -2;
				fprintf(stderr, "pcre_match: error %d\n", matchres);
				return NULL;
		}
	}
	if(regres != NULL)
		*regres = matchres;
	char **substrings = calloc(sizeof(char *), matchres + 1);
	if(!substrings) {
		fprintf(stderr, "pcre_match: substrings array alloc failure\n");
		return NULL;
	}
	for(int i = 0; i < matchres; ++i) {
		substrings[i] = util_substr(str, ovector[2*i],
				ovector[2*i + 1] - ovector[2*i]);
		if(substrings[i] == NULL) {
			fprintf(stderr, "pcre_match: substring alloc failure\n");
			for(int j = 0; j < i; ++j)
				free(substrings[j]);
			free(substrings);
			return NULL;
		}
	}
	return substrings;
} // }}}
void freeMatchStrings(char **strings) { // {{{
	if(!strings)
		return;
	for(int i = 0; strings[i] != NULL; ++i)
		free(strings[i]);
	free(strings);
} // }}}


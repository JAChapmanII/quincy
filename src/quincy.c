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

int pcre_matches(pcre *regex, char *str);
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

	vmapn = vmap_find(confMap, "quincy.restart");
	if(vmapn == NULL) {
		fprintf(stderr, "quincy: unable to find restart regex\n");
		return 2;
	}
	pcre *restart = pcre_compile(vmapn->val, 0,
			&errorMessage, &errorOffset, NULL);
	if(restart == NULL) {
		fprintf(stderr, "quincy: error creating restart pcre object: %d: %s\n",
				errorOffset, errorMessage);
		return 3;
	}

	vmapn = vmap_find(confMap, "quincy.reload");
	if(vmapn == NULL) {
		fprintf(stderr, "quincy: unable to find reload regex\n");
		return 2;
	}
	pcre *reload = pcre_compile(vmapn->val, 0,
			&errorMessage, &errorOffset, NULL);
	if(reload == NULL) {
		fprintf(stderr, "quincy: error creating reload pcre object: %d: %s\n",
				errorOffset, errorMessage);
		return 3;
	}

	// TODO: the last several blocks are equivalent, essentiall...
	vmapn = vmap_find(confMap, "quincy.command");
	if(vmapn == NULL) {
		fprintf(stderr, "quincy: unable to find command regex\n");
		return 2;
	}
	pcre *command = pcre_compile(vmapn->val, 0,
			&errorMessage, &errorOffset, NULL);
	if(command == NULL) {
		fprintf(stderr, "quincy: error creating command pcre object: %d: %s\n",
				errorOffset, errorMessage);
		return 3;
	}

	vmapn = vmap_find(confMap, "core.owner");
	if(vmapn == NULL) {
		fprintf(stderr, "quincy: unable to find owner\n");
		return 2;
	}
	char *owner = vmapn->val;
	fprintf(stderr, "quincy: owner = %s\n", owner);

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
		fprintf(stderr, "quincy: loading %s\n", mname);
		free(mname);
		module_load(mod, moddir); // TODO: anything about == 0?
		modulelist_add(modules, mod);
	}

	time_t responseTimes[3] = { 0 };
	char buf[BUF_SIZE] = { 0 };
	while(!feof(stdin)) {
		memset(buf, '\0', BUF_SIZE);
		if(fgets(buf, BUF_SIZE, stdin) == buf) {
			int matchres = 0;
			char **strs = pcre_match(pmsg, buf, &matchres);
			if(matchres > 0) {
				fprintf(stderr, "@%s, %s: %s\n", strs[3], strs[1], strs[4]);
				if(strcmp(owner, strs[1]) == 0) {
					if(pcre_matches(restart, strs[4])) {
						fprintf(stderr, "quincy: restarting\n");
						return 77;
					}
					int rmatchres = 0;
					char **rstrs = pcre_match(reload, strs[4], &rmatchres);
					if(rmatchres > 0) {
						int found = 0;
						ModuleList *ml = NULL;
						for(ml = modules; ml && ml->this; ml = ml->next) {
							if(strcmp(ml->this->name, rstrs[1]) == 0) {
								found = 1;
								break;
							}
						}
						if(found) {
							char *mname = strdup(ml->this->name),
								*uargs = strdup(ml->this->uargs);
							if(!mname || !uargs) {
								fprintf(stderr, "quincy: mname|uargs strdup failed\n");
								return -1;
							}
							module_free(ml->this);

							ml->this = module_create(mname, uargs);
							if(ml->this == NULL) {
								fprintf(stderr,
										"quincy: couldn't recreate module: %s\n", mname);
								return -2;
							}
							free(mname);
							free(uargs);
							if(module_load(ml->this, moddir) > 0)
								printf("PRIVMSG %s :%s: module %s reloaded\n",
										chan, owner, rstrs[1]);
							else
								printf("PRIVMSG %s :%s: module %s reload failed\n",
										chan, owner, rstrs[1]);
						} else {
							printf("PRIVMSG %s :%s: module not found\n", chan, owner);
						}
					}
					if(rstrs != NULL)
						freeMatchStrings(rstrs);
				}

				int cmatchres = 0;
				char **cstrs = pcre_match(command, strs[4], &cmatchres);
				char *comstring = strs[4];
				int matchesCommand = 0;
				if(cmatchres > 0) {
					matchesCommand = 1;
					comstring = cstrs[2];
				}

				ModuleList *ml = NULL;
				for(ml = modules; ml && ml->this; ml = ml->next) {
					Module *m = ml->this;
					if(strstr(m->uargs, "specific") && !matchesCommand)
						continue;
					for(int r = 0; r < m->loaded; ++r) {
						int mmatchres = 0;
						char **mstrs = pcre_match(m->regex[r], comstring, &mmatchres);
						if(mmatchres > 0) {
							char *res = module_exec(m, strs + 1, r);
							if(res != NULL) {
								printf("PRIVMSG %s :%s\n", chan, res);
								free(res);
							}
						}
						if(mstrs != NULL)
							freeMatchStrings(mstrs);
					}
				}

				if(cstrs != NULL)
					freeMatchStrings(cstrs);
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

int pcre_matches(pcre *regex, char *str) { // {{{
	if(!regex || !str)
		return 0;
	int ovector[OVEC_COUNT] = { 0 };
	int matchres = pcre_exec(regex, NULL, str, strlen(str), 0, 0,
			ovector, OVEC_COUNT);
	if(matchres == 0) {
		fprintf(stderr, "pcre_match: ovector not large enough\n");
		return 0;
	}
	if(matchres < 0) {
		switch(matchres) {
			case PCRE_ERROR_NOMATCH:
				return 0;
			default:
				fprintf(stderr, "pcre_match: error %d\n", matchres);
				return 0;
		}
	}
	return 1;
} // }}}
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


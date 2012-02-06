#ifndef MODULE_H
#define MODULE_H

#include <pcre.h>

typedef struct {
	char *name;
	char *lscope;
	int loaded;
	char **m_names;
	char **m_regex;
	pcre **regex;
} Module;

typedef struct ModuleList {
	Module *this;
	struct ModuleList *next;
} ModuleList;

Module *module_create(char *name, char *lscope);
void module_free(Module *module);
int module_load(Module *module);

ModuleList *modulelist_create();
void modulelist_free(ModuleList *modules);

#endif // MODULE_H

#ifndef MODULE_H
#define MODULE_H

#include <pcre.h>

typedef struct {
	char *name;
	char *uargs;
	char *binary;
	int loaded;
	char **m_names;
	char **m_regex;
	pcre **regex;
} Module;

typedef struct ModuleList {
	Module *this;
	struct ModuleList *next;
} ModuleList;

Module *module_create(char *name, char *uargs);
void module_free(Module *module);
int module_load(Module *module, char *moddir);
char *module_exec(Module *module, char **args, int idx);

ModuleList *modulelist_create();
void modulelist_free(ModuleList *modules);
void modulelist_add(ModuleList *modules, Module *module);

#endif // MODULE_H

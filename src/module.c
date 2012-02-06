#include "module.h"
#include <stdlib.h>
#include <string.h>

Module *module_create(char *name, char *lscope) { // {{{
	if(!name || !lscope)
		return NULL;
	Module *module = malloc(sizeof(Module));
	if(!module)
		return NULL;
	module->name = calloc(strlen(name) + 1, 1);
	module->lscope = calloc(strlen(lscope) + 1, 1);
	if(!module->name || !module->lscope) {
		module_free(module);
		return NULL;
	}
	strcpy(module->name, name);
	strcpy(module->lscope, lscope);
	module->loaded = 0;
	module->m_names = NULL;
	module->m_regex = NULL;
	module->regex = NULL;
	return module;
} // }}}
void module_free(Module *module) { // {{{
	if(!module)
		return;
	if(module->name)
		free(module->name);
	if(module->lscope)
		free(module->lscope);
	if(module->loaded > 0) {
		for(int i = 0; i < module->loaded; ++i) {
			free(module->m_names[i]);
			free(module->m_regex[i]);
			if(module->regex[i])
				pcre_free(module->regex[i]);
		}
		free(module->m_names);
		free(module->m_regex);
		free(module->regex);
	}
	free(module);
} // }}}

int module_load(Module *module) {
	return 0;
}

ModuleList *modulelist_create() {
	ModuleList *modules = malloc(sizeof(ModuleList));
	if(modules == NULL)
		return NULL;
	modules->this = NULL;
	modules->next = NULL;
}
void modulelist_free(ModuleList *modules) {
	if(!modules)
		return;
	modulelist_free(modules->next);
	module_free(modules->this);
	free(modules);
}


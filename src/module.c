#include "module.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Module *module_create(char *name, char *uargs) { // {{{
	if(!name || !uargs)
		return NULL;
	Module *module = malloc(sizeof(Module));
	if(!module)
		return NULL;
	module->name = calloc(strlen(name) + 1, 1);
	module->uargs = calloc(strlen(uargs) + 1, 1);
	if(!module->name || !module->uargs) {
		module_free(module);
		return NULL;
	}
	strcpy(module->name, name);
	strcpy(module->uargs, uargs);
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
	if(module->uargs)
		free(module->uargs);
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
	return modules;
}
void modulelist_free(ModuleList *modules) {
	if(!modules)
		return;
	modulelist_free(modules->next);
	module_free(modules->this);
	free(modules);
}
void modulelist_add(ModuleList *modules, Module *module) {
	if(modules->this == NULL) {
		modules->this = module;
		return;
	}
	if(modules->next == NULL) {
		modules->next = modulelist_create();
		if(modules->next == NULL) {
			fprintf(stderr, "modulelist_add: couldn't create another entry!\n");
			module_free(module);
			return;
		}
	}
	modulelist_add(modules->next, module);
}


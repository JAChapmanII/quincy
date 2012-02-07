#include "module.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "util.h"
#include "subprocess.h"

#define BUF_SIZE 4096

char **module_fetchNames(Module *module);
char **module_fetchRegex(Module *module);

Module *module_create(char *name, char *uargs) { // {{{
	if(!name || !uargs)
		return NULL;
	Module *module = malloc(sizeof(Module));
	if(!module)
		return NULL;
	module->name = strdup(name);
	module->uargs = strdup(uargs);
	if(!module->name || !module->uargs) {
		module_free(module);
		return NULL;
	}
	module->binary = NULL;
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
	if(module->binary)
		free(module->binary);
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

// TODO: reduce these, they share almost entirely the same code
char **module_fetchNames(Module *module) { // {{{
	char *namesArgV[3] = { "names" };
	Subprocess *sp = subprocess_create(module->binary, namesArgV, 1);
	if(sp == NULL) {
		fprintf(stderr, "module_fetchNames: couldn't create subprocess\n");
		return NULL;
	}
	if(subprocess_run(sp) != 0) {
		fprintf(stderr, "module_fetchNames: could not run subproc\n");
		subprocess_free(sp);
		return NULL;
	}
	bufreader_setBlocking(sp->br);
	char *line = subprocess_read(sp);
	if(line == NULL) {
		subprocess_free(sp);
		return NULL;
	}
	int nameCount = atoi(line);
	free(line);
	if(nameCount < 1) {
		subprocess_free(sp);
		return NULL;
	}
	char **names = calloc(sizeof(char *), nameCount + 1);
	for(int i = 0; i < nameCount; ++i) {
		names[i] = subprocess_read(sp);
		if(names[i] == NULL) {
			for(int j = 0; j < nameCount; ++j)
				free(names[j]);
			free(names);
			subprocess_free(sp);
			return NULL;
		}
	}
	subprocess_free(sp);
	return names;
} // }}}
char **module_fetchRegex(Module *module) { // {{{
	char *regexArgV[3] = { "regex" };
	Subprocess *sp = subprocess_create(module->binary, regexArgV, 1);
	if(sp == NULL) {
		fprintf(stderr, "module_fetchRegex: couldn't create subprocess\n");
		return NULL;
	}
	if(subprocess_run(sp) != 0) {
		fprintf(stderr, "module_fetchRegex: could not run subproc\n");
		subprocess_free(sp);
		return NULL;
	}
	bufreader_setBlocking(sp->br);
	char *line = subprocess_read(sp);
	if(line == NULL) {
		subprocess_free(sp);
		return NULL;
	}
	int nameCount = atoi(line);
	free(line);
	if(nameCount < 1) {
		subprocess_free(sp);
		return NULL;
	}
	char **regex = calloc(sizeof(char *), nameCount + 1);
	for(int i = 0; i < nameCount; ++i) {
		regex[i] = subprocess_read(sp);
		if(regex[i] == NULL) {
			for(int j = 0; j < nameCount; ++j)
				free(regex[j]);
			free(regex);
			subprocess_free(sp);
			return NULL;
		}
	}
	subprocess_free(sp);
	return regex;
} // }}}

int module_load(Module *module, char *moddir) {
	if(module->binary)
		free(module->binary);
	size_t blen = strlen(module->name);
	if(moddir != NULL)
		blen += strlen(moddir) + strlen("/");
	module->binary = calloc(blen + 1, 1);
	if(module->binary == NULL) {
		fprintf(stderr, "module_load: calloc failure\n");
		return 0;
	}

	if(moddir != NULL) {
		strcpy(module->binary, moddir);
		strcat(module->binary, "/");
	}
	strcat(module->binary, module->name);

	if(!util_exists(module->binary)) {
		fprintf(stderr, "module_load: %s: does not exist\n", module->binary);
		return 0;
	}

	fprintf(stderr, "module_load: bin: %s\n", module->binary);

	module->m_names = module_fetchNames(module);
	if(module->m_names == NULL) {
		fprintf(stderr, "module_load: could not fetch names\n");
		return 0;
	}
	for(int i = 0; module->m_names[i] != NULL; ++i)
		fprintf(stderr, "\t%d: %s\n", i, module->m_names[i]);

	module->m_regex = module_fetchRegex(module);
	if(module->m_regex == NULL) {
		fprintf(stderr, "module_load: could not fetch regex\n");
		return 0;
	}
	for(int i = 0; module->m_regex[i] != NULL; ++i)
		fprintf(stderr, "\t%d: %s\n", i, module->m_regex[i]);

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


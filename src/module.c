#include "module.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "util.h"
#include "subprocess.h"

#define BUF_SIZE 4096

void module_freeNames(Module *module);
void module_freeRegex(Module *module);
void module_freePCRE(Module *module);

char **module_fetchNames(Module *module);
char **module_fetchRegex(Module *module);
pcre **module_constructRegex(Module *module);

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
	module_freeNames(module);
	module_freeRegex(module);
	module_freePCRE(module);
	free(module);
} // }}}

void module_freeNames(Module *module) { // {{{
	if(!module)
		return;
	if(!module->m_names)
		return;
	for(int i = 0; module->m_names[i] != NULL; ++i)
		free(module->m_names[i]);
	free(module->m_names);
} // }}}
void module_freeRegex(Module *module) { // {{{
	if(!module)
		return;
	if(!module->m_regex)
		return;
	for(int i = 0; module->m_regex[i] != NULL; ++i)
		free(module->m_regex[i]);
	free(module->m_regex);
} // }}}
void module_freePCRE(Module *module) { // {{{
	if(!module)
		return;
	if(!module->regex)
		return;
	for(int i = 0; module->regex[i] != NULL; ++i)
		pcre_free(module->regex[i]);
	free(module->regex);
} // }}}

// TODO: reduce these, they share almost entirely the same code
char **module_fetchNames(Module *module) { // {{{
	char *namesArgV[1] = { "names" };
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
	char *regexArgV[1] = { "regex" };
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

pcre **module_constructRegex(Module *module) { // {{{
	pcre **regex = calloc(sizeof(pcre *), module->loaded);
	const char *errorMessage = NULL;
	int errorOffset = 0;
	for(int i = 0; i < module->loaded; ++i) {
		regex[i] = pcre_compile(module->m_regex[i], 0,
				&errorMessage, &errorOffset, NULL);
		if(regex[i] == NULL) {
			fprintf(stderr, "module_constructRegex: pcre_compile failure @%d: %s\n",
					errorOffset, errorMessage);
			for(int j = 0; j < i; ++j)
				pcre_free(regex[i]);
			free(regex);
			return NULL;
		}
	}
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

	module->m_names = module_fetchNames(module);
	if(module->m_names == NULL) {
		fprintf(stderr, "module_load: could not fetch names\n");
		return 0;
	}
	int nameCount = 0;
	for(nameCount = 0; module->m_names[nameCount] != NULL; ++nameCount)
		; // count up names
	module->m_regex = module_fetchRegex(module);
	if(module->m_regex == NULL) {
		fprintf(stderr, "module_load: could not fetch regex\n");
		return 0;
	}
	int regexCount = 0;
	for(regexCount = 0; module->m_regex[regexCount] != NULL; ++regexCount)
		; // count up regex

	if(nameCount != regexCount) {
		fprintf(stderr, "module_load: regex count != name count\n");
		module_freeNames(module);
		module_freeRegex(module);
		return 0;
	}
	module->loaded = nameCount;

	module->regex = module_constructRegex(module);
	if(module->regex == NULL) {
		module->loaded = 0;
		return 0;
	}

	return module->loaded;
}
char *module_exec(Module *module, char **args, int idx) {
	if(!module || !args)
		return NULL;
	char **argv = calloc(sizeof(char *), 6);
	if(!argv) {
		fprintf(stderr, "module_exec: argv alloc failure\n");
		return NULL;
	}
	argv[0] = "dispatch";
	for(int i = 0; i < 4; ++i)
		argv[i + 1] = args[i];
	char idxBuf[BUF_SIZE] = { 0 };
	snprintf(idxBuf, BUF_SIZE, "%d", idx);
	argv[5] = idxBuf;
	// TODO: this is a lot like fetchNames and fetchRegex
	Subprocess *sp = subprocess_create(module->binary, argv, 6);
	if(sp == NULL) {
		fprintf(stderr, "module_exec: couldn't create subprocess\n");
		return NULL;
	}
	if(subprocess_run(sp) != 0) {
		fprintf(stderr, "module_exec: could not run subproc\n");
		subprocess_free(sp);
		return NULL;
	}
	bufreader_setBlocking(sp->br);
	char *line = subprocess_read(sp);
	subprocess_free(sp);
	return line;
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


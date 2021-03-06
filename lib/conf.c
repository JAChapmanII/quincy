#include "conf.h"
#include "defines.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 4096

static VMap *m_confMap = NULL;

static char *m_server = NULL;
static char *m_nick   = NULL;
static char *m_chan   = NULL;
static char *m_binary = NULL;
static int m_port = -1;

VMap *conf_map(void) {
	return m_confMap;
}

void conf_parseArguments(char **argv, int argc) {
	if(argc > 1)
		m_server = argv[1];
	if(argc > 2)
		m_nick = argv[2];
	if(argc > 3)
		m_chan = argv[3];
	if(argc > 4)
		m_binary = argv[4];
	if(argc > 5) {
		m_port = atoi(argv[5]);
		if(m_port == 0)
			m_port = DEFAULT_PORT;
	}
}
void conf_read(const char *file) {
	if(file == NULL)
		file = DEFAULT_CONF;

	FILE *in = fopen(file, "r");
	if(in == NULL)
		return;

	if(m_confMap != NULL) {
		vmap_free(m_confMap);
		m_confMap = NULL;
	}
	m_confMap = vmap_create();
	if(m_confMap == NULL)
		return;

	char scope[BUF_SIZE] = { 0 };
	size_t scope_len = 0;
	// default scope is core
	strcpy(scope, "core");
	scope_len = 4;

	char buf[BUF_SIZE] = { 0 };
	while(!feof(in)) {
		if(fgets(buf, BUF_SIZE, in) != buf)
			break;
		size_t len = strlen(buf);
		buf[--len] = '\0';
		if(buf[0] == '#') {
			// a comment line
			continue;
		} else if(len == 0) {
			// a blank line
			continue;
		} else if((buf[0] == '[') && (buf[len - 1] == ']')) {
			strncpy(scope, buf + 1, len - 2);
			scope_len = len - 2;
			scope[scope_len] = '\0';
		} else if(strstr(buf, "=")) {
			char *variable = NULL, *value = NULL, *saveptr = NULL;
			char *tok = strtok_r(buf, " \t", &saveptr);
			if(tok == NULL) {
				fprintf(stderr, "conf_read: no first argument?\n");
				continue;
			}

			variable = tok;
			char vnbuf[BUF_SIZE] = { 0 };
			if(scope_len + strlen(tok) + 2 >= BUF_SIZE) {
				fprintf(stderr, "conf_read: scoped variable name too long: %s.%s\n",
						scope, variable);
				continue;
			}
			strcpy(vnbuf, scope);
			strcat(vnbuf, ".");
			strcat(vnbuf, variable);

			tok = strtok_r(NULL, " \t", &saveptr);
			if((tok == NULL) || (strcmp(tok, "=") != 0)) {
				fprintf(stderr, "conf_read: second word is not \"=\"\n");
				continue;
			}

			tok = strtok_r(NULL, " \t", &saveptr);
			if(tok == NULL) {
				fprintf(stderr, "conf_read: no second argument?\n");
				continue;
			}
			value = tok;

			vmap_add(m_confMap, vnbuf, value);
		} else {
			fprintf(stderr, "conf_read: unable to parse: %s\n", buf);
		}
	}
}

char *conf_server(void) {
	if(m_server)
		return m_server;
	if(m_confMap && vmap_find(m_confMap, "core.server"))
		return vmap_find(m_confMap, "core.server")->val;
	return DEFAULT_SERVER;
}
int conf_port(void) {
	if(m_port > 0)
		return m_port;
	if(m_confMap && vmap_find(m_confMap, "core.port")) {
		int tmp = atoi(vmap_find(m_confMap, "core.port")->val);
		if(tmp > 0)
			return tmp;
	}
	return DEFAULT_PORT;
}

char *conf_nick(void) {
	if(m_nick)
		return m_nick;
	if(m_confMap && vmap_find(m_confMap, "core.nick"))
		return vmap_find(m_confMap, "core.nick")->val;
	return DEFAULT_NICK;
}
char *conf_chan(void) {
	if(m_chan)
		return m_chan;
	if(m_confMap && vmap_find(m_confMap, "core.chan"))
		return vmap_find(m_confMap, "core.chan")->val;
	return DEFAULT_CHAN;
}

char *conf_binary(void) {
	if(m_binary)
		return m_binary;
	if(m_confMap && vmap_find(m_confMap, "conman.binary"))
		return vmap_find(m_confMap, "conman.binary")->val;
	return DEFAULT_CHAN;
}


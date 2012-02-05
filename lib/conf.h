#ifndef CONF_H
#define CONF_H

#include "vmap.h"

VMap *conf_map(void);

void conf_parseArguments(char **argv, int argc);
void conf_read(const char *file);

char *conf_server(void);
char *conf_nick(void);
char *conf_chan(void);
char *conf_binary(void);
int conf_port(void);

#endif // CONF_H

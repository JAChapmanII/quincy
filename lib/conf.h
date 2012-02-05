#ifndef CONF_H
#define CONF_H

#include "vmap.h"

// Return a pointer to the VMap used internally for variable storage
VMap *conf_map(void);

// Parse the standard main arguments for configuration options
void conf_parseArguments(char **argv, int argc);
/* Read in a configuration file
 * 	if file is NULL, the filename is taken from either the compile time
 * 	constants or previously parsed command line arguments
 */
void conf_read(const char *file);

// Return the server this is configured to connect to
char *conf_server(void);
// Return the port this is configured to connect to
int conf_port(void);

// Return the nick this is configured to connect with
char *conf_nick(void);
// Return the channel this is configured to connect to
char *conf_chan(void);

// Return the binary this is configured to run as a subprocess
char *conf_binary(void);

#endif // CONF_H

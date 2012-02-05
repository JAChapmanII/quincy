#ifndef BASE_H
#define BASE_H

char *userNick  = NULL;
char *userHMask = NULL;
char *target    = NULL;
char *message   = NULL;

int moduleInit(void);
int moduleCleanup(void);

char **moduleNames(void);
char **moduleRegex(void);
char *moduleHelp(const char *module);
char *dispatch(int module);

#endif // BASE_H

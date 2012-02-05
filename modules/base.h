#ifndef BASE_H
#define BASE_H

char *userNick;
char *userHMask;
char *target;
char *message;

int moduleInit(void);
int moduleCleanup(void);

char **moduleNames(void);
char **moduleRegex(void);
char *moduleHelp(int module);

char *dispatch(int module);

#endif // BASE_H

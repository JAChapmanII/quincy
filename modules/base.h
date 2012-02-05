#ifndef BASE_H
#define BASE_H

// These variables are filled out whenever dispatch is called, so that the
// module can know who invoked the function, where it was invoked and what the
// message matching the regex is.
char *userNick;
char *userHMask;
char *target;
char *message;

// This function is called whenever the module starts, to setup any necessary
// data, load files, etc.
int moduleInit(void);
// Called on exit, this function should be used save persistent data to disk
int moduleCleanup(void);

// Return an array of the names of functions available in this module
char **moduleNames(void);
// Return the corresponding regex for each of the above functions
char **moduleRegex(void);
// Return a help string for a particular module function
char *moduleHelp(int module);

// Return the output from a particular module function
char *dispatch(int module);

#endif // BASE_H

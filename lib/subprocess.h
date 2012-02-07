#ifndef SUBPROCESS_H
#define SUBPROCESS_H

#include <sys/types.h>
#include <stdio.h>

typedef enum { SP_BEXEC, SP_EXEC, SP_AEXEC, SP_INVALID } SubprocessStatus;

typedef struct {
	char *binary;
	char **argv;
	int argc;
	int pipe[2];
	SubprocessStatus status;
	pid_t pid;
	int value;
} Subprocess;

// Create a Subprocess object for a binary
Subprocess *subprocess_create(char *binary, char **argv, int argc);
// Free memory associated with a subproc
void subprocess_free(Subprocess *subproc);

// Actually execute the configured binary
int subprocess_run(Subprocess *subproc);
// Attempts to update the status and returns the new one
int subprocess_status(Subprocess *subproc);
// Send the SIGKILL signal to the running subprocess, returs results of kill
int subprocess_kill(Subprocess *subproc);

// Return a write-mode FILE * hooked to the stdin of a subprocess
FILE *subprocess_wfile(Subprocess *subproc);

#endif // SUBPROCESS_H

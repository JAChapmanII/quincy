#include "subprocess.h"
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include "util.h"

Subprocess *subprocess_create(char *binary, char **argv, int argc) { // {{{
	if(!binary)
		return NULL;
	Subprocess *subproc = malloc(sizeof(Subprocess));
	if(!subproc)
		return NULL;
	subproc->binary = strdup(binary);
	if(!subproc->binary) {
		subprocess_free(subproc);
		return NULL;
	}

	subproc->argc = ((argc < 0) ? 0 : argc) + 2;
	subproc->argv = calloc(sizeof(char *), subproc->argc);
	if(!subproc->argv) {
		subprocess_free(subproc);
		return NULL;
	}
	subproc->argv[0] = subproc->binary;
	subproc->argv[argc + 1] = NULL;
	for(int i = 0; i < argc; ++i) {
		subproc->argv[i + 1] = strdup(argv[i]);
		if(subproc->argv[i + 1] == NULL) {
			subprocess_free(subproc);
			return NULL;
		}
	}

	subproc->pipe[0] = subproc->pipe[1] = 0;
	subproc->status = SP_BEXEC;
	subproc->pid = 0;
	subproc->value = 0;
	return subproc;
} // }}}
void subprocess_free(Subprocess *subproc) { // {{{
	if(!subproc)
		return;
	if(subproc->binary)
		free(subproc->binary);
	for(int i = 1; i < subproc->argc; ++i)
		if(subproc->argv[i] != NULL)
			free(subproc->argv[i]);
	if(subproc->status == SP_EXEC)
		subprocess_kill(subproc);
	free(subproc);
} // }}}

int subprocess_run(Subprocess *subproc) {
	// if we're not in the before exec phase, abort
	if(subproc->status != SP_BEXEC)
		return -1;

	// if the binary doesn't exist, abort
	if(!util_exists(subproc->binary)) {
		subproc->status = SP_INVALID;
		return -1;
	}

	int left[2] = { 0 }, right[2] = { 0 };
	// if we can't create the left pipe, abort
	int fail = pipe(left);
	if(fail)
		return fail;
	// if we can't create the right pipe, abort
	fail = pipe(right);
	if(fail) {
		util_closePipe(left);
		return fail;
	}

	// if we can't fork, abort
	subproc->pid = fork();
	if(subproc->pid == -1) {
		util_closePipe(left);
		util_closePipe(right);
		return -2;
	}

	// if we're the child, execv the binary
	if(subproc->pid == 0) {
		dup2(left[0], 0);
		util_closePipe(left);

		dup2(right[1], 1);
		util_closePipe(right);

		execv(subproc->binary, subproc->argv);
		return -99;
	}

	// if we're main, close uneeded ends and copy fds
	close(left[0]);
	close(right[1]);
	subproc->pipe[0] = right[0];
	subproc->pipe[1] = left[1];

	subproc->status = SP_EXEC;
	return 0;
}

int subprocess_status(Subprocess *subproc) {
	if(subproc->status != SP_EXEC)
		return subproc->status;
	pid_t pid = waitpid(subproc->pid, &subproc->value, WNOHANG);
	if(pid == subproc->pid)
		subproc->status = SP_AEXEC;
	return subproc->status;
}
int subprocess_kill(Subprocess *subproc) {
	if(subproc->status != SP_EXEC)
		return -1;
	return kill(subproc->pid, SIGKILL);
}



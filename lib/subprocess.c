#include "subprocess.h"
#include <unistd.h>
#include <fcntl.h>
#include "util.h"

int util_subprocessPipe(const char *binary, char **argv, int *fds) { // {{{
	// if the binary doesn't exist, abort
	if(!util_exists(binary))
		return -1;
	argv[0] = (char *)binary;

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
	pid_t pid = fork();
	if(pid == -1) {
		util_closePipe(left);
		util_closePipe(right);
		return -2;
	}

	// if we're the child, execv the binary
	if(pid == 0) {
		dup2(left[0], 0);
		util_closePipe(left);

		dup2(right[1], 1);
		util_closePipe(right);

		execv(binary, argv);
		return -99;
	}

	// if we're main, close uneeded ends and copy fds
	close(left[0]);
	close(right[1]);
	fds[0] = right[0];
	fds[1] = left[1];
	return 0;
} // }}}


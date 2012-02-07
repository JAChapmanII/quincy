#ifndef SUBPROCESS_H
#define SUBPROCESS_H

/* Create a pipe to a subprocess
 * 	binary: the name of the binary to invoke
 * 	argv: this array is passed to the subprocess as its main's argv
 * 	fds: an int[2] to store the read/write ends of the pipe
 */
int util_subprocessPipe(const char *binary, char **argv, int *fds);

#endif // SUBPROCESS_H

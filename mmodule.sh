#!/bin/bash

idir="${2-.}"
BASE_H="$idir/base.h"
MODULE_C="$idir/module.c"
STATUS_H="$idir/status.h"
STATUS_C="$idir/status.c"
odir="${3-$idir}"

if [[ -z $1 ]]; then
	echo "Usage: $0 <module name>"
	echo "Builds the binary for <module>"
	exit 1
fi

mname="$(echo $1 | sed -r 's/\.c$//')"
if [[ ! -f $mname.c ]]; then
	echo "mmodule: $mname: file does not exist"
	exit 2
fi

exit 99


#!/bin/sh
GNAT=/usr/gnat/bin
export  PATH="$GNAT:$(pwd)/utils:$PATH"
export  PERL5LIB=$(pwd)
echo $PATH | cut -d ':' -f -5 | tr ':' '\n'

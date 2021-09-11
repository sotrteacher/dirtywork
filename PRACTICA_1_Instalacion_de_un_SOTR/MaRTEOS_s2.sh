#!/bin/sh
export  PATH="$1:$(pwd)/utils:$PATH"
export  PERL5LIB=$(pwd)
echo $PATH | cut -d ':' -f -5 | tr ':' '\n'
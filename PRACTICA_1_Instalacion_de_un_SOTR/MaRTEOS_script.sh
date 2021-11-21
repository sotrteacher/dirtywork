#!/bin/sh
# Way of use of this script:
# source MaRTE_script.sh 
GNAT=/home/usuario/myapps/gnat/bin 
export  PATH="$GNAT:/home/usuario/marte_2.0_22Feb2017/utils:$PATH"
export  PERL5LIB=/home/usuario/marte_2.0_22Feb2017
echo $PATH | cut -d ':' -f -5 | tr ':' '\n'

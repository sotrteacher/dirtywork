# Linea de comando original (obtenida tecleando SOTR_mgcc -g -Wall ../examples/hello_world_c.c -o hello_world_c.exe)
# gcc -nostdinc -I/home/usuario/marte_2.0_22Feb2017/arch/include -g -Wall ../examples/hello_world_c.c    -m32 -march=i686  -o hello_world_c.exe   /home/usuario/marte_2.0_22Feb2017/arch/call_main/wrapper_main_c.o   -Wl,-T,/home/usuario/marte_2.0_22Feb2017/utils/linker.lds -static -nostartfiles -L/home/usuario/marte_2.0_22Feb2017/lib -L/home/usuario/marte_2.0_22Feb2017/gnat_rts/rts/adalib -L/home/usuario/myapps/gnat/lib/gcc/x86_64-pc-linux-gnu/4.9.4 -lmarte -lgnarl -lgnat -lmarte -lgcc_sjlj
# Comentario de documentaci\'on                                                       Martes 22 de febrero de 2022.
# El gui\'on de linker /home/usuario/marte_2.0_22Feb2017/utils/linker.lds, contiene cerca de su inicio, la l\'inea:
# INPUT(multiboot.o)
# Al buscar el archivo multiboot.o con el comando 
# find /home/usuario/marte_2.0_22Feb2017/ -name multiboot.o
# obtuve las l\'ineas:
# /home/usuario/marte_2.0_22Feb2017/objs/x86_objs/multiboot.o
# /home/usuario/marte_2.0_22Feb2017/gnat_rts/rts-marteuc_x86/adalib/multiboot.o
# Para ver cu\'al era el archivo y cu\'al un link al archivo multiboot.o, utilic\'e el comando:
# ls -lh `find /home/usuario/marte_2.0_22Feb2017/ -name multiboot.o`
# y obtuve las l\'ineas:
# lrwxrwxrwx 1 usuario usuario   24 nov 21 03:10 /home/usuario/marte_2.0_22Feb2017/gnat_rts/rts-marteuc_x86/adalib/multiboot.o -> ../../../lib/multiboot.o
# -rw-r--r-- 1 usuario usuario 4.8K nov 21 02:59 /home/usuario/marte_2.0_22Feb2017/objs/x86_objs/multiboot.o
# Despu\'es, ejecut\'e el comando
# pushd /home/usuario/marte_2.0_22Feb2017/gnat_rts/rts-marteuc_x86/adalib/
# ls -lh ../../../lib
# lrwxrwxrwx 1 usuario usuario 47 nov 20 17:09 ../../../lib -> /home/usuario/marte_2.0_22Feb2017/objs/x86_objs
# Por lo tanto, llego a la conclusi\'on de que el enlace simb\'olico
# /home/usuario/marte_2.0_22Feb2017/gnat_rts/rts-marteuc_x86/adalib/multiboot.o
# apunta al archivo
# /home/usuario/marte_2.0_22Feb2017/lib/multiboot.o
# Pero, como
# ls -lh /home/usuario/marte_2.0_22Feb2017/lib
# lrwxrwxrwx 1 usuario usuario 47 nov 20 17:09 /home/usuario/marte_2.0_22Feb2017/lib -> /home/usuario/marte_2.0_22Feb2017/objs/x86_objs
# Es decir, /home/usuario/marte_2.0_22Feb2017/lib es un enlace simb\'olico al directorio /home/usuario/marte_2.0_22Feb2017/objs/x86_objs,
# y en este directorio es donde se encontr\'o el archivo multiboot.o. Por cierto, la l\'inea:
# /home/usuario/marte_2.0_22Feb2017/objs/x86_objs/multiboot.o
# fue una de las l\'ineas de salida (vease las l\'ineas de salida arriba) del comando
# ls -lh `find /home/usuario/marte_2.0_22Feb2017/ -name multiboot.o`
#
# Para compilaci\'on
# /home/usuario/marte_2.0_22Feb2017/arch -> /home/usuario/marte_2.0_22Feb2017/x86_arch
# /home/usuario/marte_2.0_22Feb2017/x86_arch/include
# Para enlazado
#/home/usuario/marte_2.0_22Feb2017/arch/call_main/wrapper_main_c.o 
# es /home/usuario/marte_2.0_22Feb2017/x86_arch/call_main/wrapper_main_c.o
#/home/usuario/marte_2.0_22Feb2017/objs/x86_objs
#/home/usuario/marte_2.0_22Feb2017/gnat_rts/rts/adalib 
#/home/usuario/marte_2.0_22Feb2017/gnat_rts/rts -> /home/usuario/marte_2.0_22Feb2017/gnat_rts/rts-marteuc_x86 
# As\'i que
#/home/usuario/marte_2.0_22Feb2017/gnat_rts/rts/adalib es en realidad
# /home/usuario/marte_2.0_22Feb2017/gnat_rts/rts-marteuc_x86/adalib/
#/home/usuario/myapps/gnat/lib/gcc/x86_64-pc-linux-gnu/4.9.4
# En este directorio, para saber a qu\'e apuntan los softlinks, utilice el comando: ls -l `find . -type l `
# y encontr\'e que:
# lrwxrwxrwx 1 usuario usuario 21 nov 21 03:10 ./adainclude -> rts-native/adainclude
# lrwxrwxrwx 1 usuario usuario 17 nov 21 03:10 ./adalib -> rts-native/adalib
# lrwxrwxrwx 1 usuario usuario 21 nov 20 16:54 ./include-fixed/bits -> x86_64-linux-gnu/bits
# lrwxrwxrwx 1 usuario usuario 58 nov 21 03:06 ./rts-marteuc_x86 -> /home/usuario/marte_2.0_22Feb2017/gnat_rts/rts-marteuc_x86
# lrwxrwxrwx 1 usuario usuario 16 may 16  2016 ./rts-native/adalib/libgnarl.so -> libgnarl-2016.so
# lrwxrwxrwx 1 usuario usuario 15 may 16  2016 ./rts-native/adalib/libgnat.so -> libgnat-2016.so
# As\'i que modificar\'e el soft link rts-marteuc_x86 para que quede como sigue:
# ./rts-marteuc_x86 -> ./../../../../../gnat_rts/rts-marteuc_x86
# du -sh --total /home/usuario/marte_2.0_22Feb2017/objs/x86_objs /home/usuario/marte_2.0_22Feb2017/gnat_rts/rts/adalib /home/usuario/myapps/gnat/lib/gcc/x86_64-pc-linux-gnu/4.9.4

all: hello_world_c.xtn

# Suffix Rule
.c.o:
	gcc -nostdinc -I/home/usuario/marte_2.0_22Feb2017/arch/include -g -Wall -m32 -march=i686 -c $<
hello_world_c.xtn: hello_world_c.o
	gcc -o $@ $< /home/usuario/marte_2.0_22Feb2017/arch/call_main/wrapper_main_c.o   -Wl,-T,/home/usuario/marte_2.0_22Feb2017/utils/linker.lds -static -nostartfiles -L/home/usuario/marte_2.0_22Feb2017/lib -L/home/usuario/marte_2.0_22Feb2017/gnat_rts/rts/adalib -L/home/usuario/myapps/gnat/lib/gcc/x86_64-pc-linux-gnu/4.9.4 -lmarte -lgnarl -lgnat -lmarte -lgcc_sjlj
clean:
	rm -v hello_world_c.xtn hello_world_c.o


GNAT := /home/usuario/myapps/gnat/bin
PATH  := $(GNAT):$(PATH):/home/usuario/marte_2.0_22Feb2017/utils
PERL5LIB := /home/usuario/marte_2.0_22Feb2017
MPATH := /home/usuario/marte_2.0_22Feb2017
SHELL := env PATH=$(PATH) PERL5LIB=$(PERL5LIB) /bin/bash


all: hello_world_c.xtn

# Suffix Rule
.c.o:
	gcc -nostdinc -I/home/usuario/marte_2.0_22Feb2017/arch/include -g -Wall -m32 -march=i686 -c $<
hello_world_c.xtn: hello_world_c.o
	gcc -o $@ $< /home/usuario/marte_2.0_22Feb2017/arch/call_main/wrapper_main_c.o   -Wl,-T,/home/usuario/marte_2.0_22Feb2017/utils/linker.lds -s -static -nostartfiles -L/home/usuario/marte_2.0_22Feb2017/lib -L/home/usuario/marte_2.0_22Feb2017/gnat_rts/rts/adalib -L/home/usuario/myapps/gnat/lib/gcc/x86_64-pc-linux-gnu/4.9.4 -lmarte -lgnarl -lgnat -lmarte -lgcc_sjlj
# La siguiente l\'inea produce errores de enlazado, (se agreg\'o la opci\'on -nostdlib) lo que significa que
# en la construcci\'on del kernel hello_world_c.xtn se est\'an enlazando algunas funciones de la biblioteca
# glibc del SO Host (Debian GNU/Linux 11, de 64 bits).
# Falta revisar si en la biblioteca uClibC 1.0.39 se puede compilar las funciones de bilbioteca glibc que se
# est\'an usando. En caso de que no, se podr\'ia intentar subir ademas de los 130MB de archivos objeto y
# bibliotecas de MaRTE, la bilbioteca est\'andar glibc a MAteriales de clase en ms teams.
#	gcc -o $@ $< /home/usuario/marte_2.0_22Feb2017/arch/call_main/wrapper_main_c.o   -Wl,-T,/home/usuario/marte_2.0_22Feb2017/utils/linker.lds -s -static -nostartfiles -nostdlib -L/home/usuario/marte_2.0_22Feb2017/lib -L/home/usuario/marte_2.0_22Feb2017/gnat_rts/rts/adalib -L/home/usuario/myapps/gnat/lib/gcc/x86_64-pc-linux-gnu/4.9.4 -L/usr/x86_64-linux-uclibc/usr/lib -lc -lmarte -lgnarl -lgnat -lmarte -lgcc_sjlj
run:
	qemu-system-i386 -kernel hello_world_c.xtn
run1:
	qemu-system-i386 -kernel bin/Release/HolaMaRTE2OS
clean:
	rm -v hello_world_c.xtn *.o
identifytoolchain:
	gcc --version

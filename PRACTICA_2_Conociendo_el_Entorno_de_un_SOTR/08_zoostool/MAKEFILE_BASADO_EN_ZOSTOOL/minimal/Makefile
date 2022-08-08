#EXE = minimal.xtn minimal_xtn
EXE = minimal_xtn hello.xtn
CC = gcc
LD = ld
# Para utilizar i686-unknown-linux-gnu-gcc se debe modificar previamente la variable de ambiente 
# PATH con el comando 
# source /ruta/a/Crosstools_GCC-2.95.3_script.sh
# En mi PC (LMC), el comando es:
# source /home/usuario/myapps/Crosstools_GCC-2.95.3_script.sh
CC_I686 = i686-unknown-linux-gnu-gcc
LD_I686 = i686-unknown-linux-gnu-ld
all:minimal_xtn hello.xtn
#minimal_xtn:minimal.c
#	$(CC_I686) -nostdlib -fno-builtin -c $< 
#	$(LD_I686) --verbose minimal.o ./crt_dir/crt1.o ./crt_dir/crti.o -static -nostdlib -o $@
minimal_xtn:minimal.c
	$(CC_I686) -o $@ -fno-builtin -static -s $< 
#	$(LD_I686) --verbose minimal.o ./crt_dir/crt1.o ./crt_dir/crti.o -static -nostdlib -o $@
hello.xtn:hello.c
	$(CC_I686) -o $@ -fno-builtin -static -s $< 
CRT_DIR = ./crt_dir
TARGET_PREFIX = ./target_prefix
STARTUP_FILES = ${CRT_DIR}/crt1.o ${CRT_DIR}/crti.o
END_FILES = ${CRT_DIR}/crtn.o

LINKED_FILES = ${STARTUP_FILES} ${OBJS} ${END_FILES}
LIBS = -lc
USER_CFLAGS += -static -nostdlib ${LINKED_FILES} -L${TARGET_PREFIX} ${LIBS}
CFLAGS = -m32 -fno-builtin -c
USER_CFLAGS += -static -nostdlib ${LINKED_FILES} -L${TARGET_PREFIX} ${LIBS}
minimal.xtn:minimal.c
	$(CC) $(CFLAGS) $< 
	$(LD) minimal.o $(USER_CFLAGS) -o $@
clean:
	rm -v $(EXE)



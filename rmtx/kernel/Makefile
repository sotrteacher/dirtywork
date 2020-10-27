# To recompile MTX: 
# Either run make followed by make install on this Makefile
# OR     run the sh script mk 3
# Both install mtx to partition 3 of a VMware VM

# Switch is controlled by the bcc preprocessor -DMK 
# if -DMK, t.c includes all the .c files during compilation
# else     each .c file includes type.h and is compiled separately

H     =	 ../type.h
AS    =  as86
CC    =  bcc -ansi
FLAGS =  -ansi -c
LD    =  ld86
LIB   =  /usr/lib/bcc/libc.a

OBJ =	t.o io.o queue.o wait.o mem.o loader.o fork.o syscall.o signal.o \
	exec.o threads.o mes.o pipe.o int.o

mtx:    $(OBJ)
	$(AS) -o ts.o ts.s	

install:
	vminstall mtx 3

#dependency list	
t.o:		$(H)
io.o:		$(H)		
queue.o:	$(H)		
wait.o:  	$(H)	 
mem.o:          $(H)	
loader.o:       $(H)	
fork.o:         $(H)	
exec.o:    	$(H)	
threads.o: 	$(H)	
mes.o:          $(H)
syscall.o:      $(H)
signal.o:	$(H)
int.o:    	$(H)	

clean:
	rm *.o

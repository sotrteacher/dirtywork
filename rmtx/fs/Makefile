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

OBJ =	alloc_dealloc.o buffer.o cd_pwd.o chmod_chown.o dev.o fs.o global.o \
	link_unlink.o misc1.o mkdir_creat.o mount.o mount_root.o open_close.o \
	read.o rmdir.o stat.o touch.o util.o write.o

mtx:    $(OBJ)
#	$(LD) -i -o mtx ts.o $(OBJ) fs.o $(LIB)

install:
	vminstall mtx 3

#dependency list	
alloc_dealloc.o:	$(H)
buffer.o:		$(H)		
cd_pwd.o:		$(H)		
chmod_chown.o:		$(H)		
dev.o:		  	$(H)	 
fs.o:		    	$(H)	
global.o:		$(H)	
link_unlink.o:		$(H)	
misc1.o:	        $(H)	
mkdir_creat.o:		$(H)	
mount.o:	    	$(H)	
mount_root.o:    	$(H)	
open_close.o:	 	$(H)	
read.o:			$(H)
rmdir.o:	  	$(H)	
stat.o:		     	$(H)	
touch.o:		$(H)	
util.o:		     	$(H)	
write.o:	      	$(H)	

clean:
	rm *.o mtx hdmtx




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
#CC    =  bcc -ansi -DMK
FLAGS =  -ansi -c
LD    =  ld86
LIB   =  /usr/lib/bcc/libc.a

OBJ =	vid.o timer.o pv.o kbd.o pr.o serial.o fd.o hd.o atapi.o

mtx:    $(OBJ)
#	$(CC) $(FLAGS) -o fs.o FS/fs.c
#	$(AS) -o ts.o ts.x	
#	$(LD) -i -o mtx ts.o $(OBJ) fs.o $(LIB)

install:
	vminstall mtx 3

#dependency list	
vid.o:     	$(H)	
timer.o:   	$(H)	
pv.o:    	$(H)	
kbd.o:     	$(H)	
pr.o:		$(H)
serial.o:  	$(H)	
fd.o:      	$(H)	
hd.o:      	$(H)	
atapi.o:   	$(H)	

clean:
	rm *.o mtx hdmtx




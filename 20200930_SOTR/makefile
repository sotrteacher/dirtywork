EXE:=test
OBJS:=hola.o archivo.o getcs.o 
CC=gcc
AS=as
.s.o:  # for each fname.o, assemble fname.s into fname.o
	$(AS) $< -o $@  # -o $@ REQUIRED  for .s files
#	$(AS) --32 $< -o $@  # -o $@ REQUIRED  for .s files
.c.o:  # for each fname.o, compile fname.c into fname.o
	$(CC) -c $< -o $@  # -o $@ optional for .c files
#	$(CC) -m32 -c $< -o $@  # -o $@ optional for .c files
$(EXE):$(OBJS)
	$(CC) $^ -o $@

clean:
	rm -v *.o $(EXE)

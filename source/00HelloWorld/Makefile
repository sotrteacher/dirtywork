EXE:=test
EXE1:=test_g
EXE2:=test_uclib.xtn	#Generar con enlazado static con la biblioteca uClibc
EXE3:=test_g_uclibc.xtn #Generar con enlazado static con la biblioteca uClibc
CC = gcc
#CC = tcc
MYCFLAGS = -DTEST1 -I./include

all:$(EXE1) $(EXE) $(EXE2) $(EXE3)
default:$(EXE)

# A suffix rule
.c.o:
	$(CC) -I./include -Wall -c $<
$(EXE):hello.c
	$(CC) -I./include $< -o $@
$(EXE1):g.c hello.c
#	$(CC) -DTEST1 -I./include -o $@ $^
	$(CC) $(MYCFLAGS) -o $@ $^
$(EXE2):hello.o
	# AGREGUE SU CODIGO MAKE AQUI (Generar con enlazado static con la biblioteca uClibc)
$(EXE3):g.o hello.o 
	# AGREGUE SU CODIGO MAKE AQUI (Generar con enlazado static con la biblioteca uClibc)
clean:
	rm -v $(EXE) $(EXE1)

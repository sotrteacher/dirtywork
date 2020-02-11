EXE:=TestArr 
PARACLEAN:=$(EXE)
CC=gcc
default:$(EXE)
$(EXE):ArreglosYEstructuras.c main.c
	$(CC) -I./ $^ -o $@
clean:
	rm -v $(PARACLEAN)

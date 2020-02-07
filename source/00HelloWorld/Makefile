EXE:=test
EXE1:=test_g
#CC = gcc
CC = tcc
MYCFLAGS = -DTEST1 -I./include

all:$(EXE1) $(EXE)
default:$(EXE)

$(EXE):hello.c
	$(CC) -I./include $< -o $@
$(EXE1):g.c hello.c
#	$(CC) -DTEST1 -I./include -o $@ $^
	$(CC) $(MYCFLAGS) -o $@ $^
clean:
	rm -v $(EXE) $(EXE1)

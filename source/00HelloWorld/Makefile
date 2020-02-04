EXE:=test
EXE1:=test_g
CC = gcc

all:$(EXE1)
default:$(EXE)

$(EXE):hello.c
	$(CC) -I./include $< -o $@
$(EXE1):g.c hello.c
	$(CC) -DTEST1 -I./include -o $@ $^
clean:
	rm -v $(EXE) $(EXE1)

EXE:=test
EXE1:=test_g
CC = gcc

$(EXE1):g.c hello.c
	$(CC) -o $@ $^
$(EXE):hello.c
	$(CC) $< -o $@
clean:
	rm -v $(EXE)

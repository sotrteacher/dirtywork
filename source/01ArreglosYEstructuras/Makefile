EXE:=TestArr 
EXE1:=TestStruct
PARACLEAN:=*.o $(EXE) $(EXE1) 
CC=gcc
MYCFLAGS = -I./ -I./include/
default:$(EXE) $(EXE1)


$(EXE):ArreglosYEstructuras.c main.c
	$(CC) $(MYCFLAGS) $^ -o $@
$(EXE1):ArreglosYEstructuras.o main1.o
	$(CC) $^ -o $@
main1.o:main1.c
	$(CC) -c $(MYCFLAGS) $< -o $@
ArreglosYEstructuras.o:ArreglosYEstructuras.c
	$(CC) -c $(MYCFLAGS) $< -o $@
clean:
	rm -v $(PARACLEAN)

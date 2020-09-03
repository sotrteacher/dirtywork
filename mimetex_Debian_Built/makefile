## Build on MinGW
# Build on Debian
EXE:=mimetex.exe
CC=gcc
$(EXE):gifsave.c mimetex.c strcasestr.c
#	$(CC) -DAA -DWINDOWS $^ -lm -o $@ 
	$(CC) -DAA $^ -lm -o $@ 

clean:
	rm -v $(EXE)


CC=gcc
CFLAGS=-fno-builtin -m32 -c

notmain:notmain.o Xishere.o
	$(CC) $(CFLAGS) $^ -o $@

notmain.o:notmain.c
	$(CC) $(CFLAGS) $<

xishere.o:Xishere.c
	$(CC) $(CFLAGS) $<
	

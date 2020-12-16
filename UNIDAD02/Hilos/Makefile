.PHONY: clean default

CC=mgcc
CXX=mg++
GNAT=mgnatmake
ADA_FLAGS = -gnata -gnato -g
CFLAGS = -g -Wall
SRC := $(wildcard *.c -or *.adb)

default: hello_world.exe hello_world_c.exe

%.exe: $(SRC)
	@exec echo -en "\n>> Compiling $@: ";
	@if [ -f $*.c ]; then $(CC) $(CFLAGS) $*.c -o $@ 1> /dev/null; fi
	@if [ -f $*.adb ]; then $(GNAT) $(ADA_FLAGS) $*.adb -o $@ 1> /dev/null; fi
	@if [ -f $*.cc ]; then $(CXX) $(CFLAGS) $*.cc -o $@ 1> /dev/null; fi
	@exec echo "  [OK]";

clean:
	@exec echo -e "\n>> Cleaning... ";
	@find \( -name '*.[oa]' -or -name '*~*' -or -name '*.ali' -or -name '*.log' \
		-or -name '*.exe' -or -name '*.img' -or -name '*.list' -or -name 'mprogram' -or -name 'a.out' \) -print -delete
	@rm -f hello_world
	@exec echo ">> End Cleaning"



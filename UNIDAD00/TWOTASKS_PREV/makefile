AS = as
LD = ld
all: a.out a.bin a_clon.bin a_modif.out
all1: a_modif_lma.out
a1.o:a1.s
	$(AS) --32 $< -o $@

a2.o:a2.s
	$(AS) --32 $< -o $@

a.out: a1.o a2.o
	$(LD) -m elf_i386 a1.o a2.o -o a.out   -T   test.lds

a.bin: a.out
	objcopy -j abc -j def -O binary a.out a.bin
	od -t x1 a.bin

a_clon.bin: a.out
	objcopy -j def -j abc -O binary a.out a_clon.bin
	od -t x1 a_clon.bin

a_modif.out: a1.o a2.o
#	$(LD) -m elf_i386 a1.o a2.o -o a_modif.out   -T   test1.lds
	$(LD)  a1.o a2.o -o a_modif.out   -T   test1.lds
	objcopy -j def -j abc -O binary a_modif.out a_modif.bin
	@echo 'Please check out of: make objd3 <--|'
	@echo 'and of: make objd3h and make bin3 <--|'
	@echo 'After that, check out of make a_modif_lma.out <--|'
	@echo 'Or shorter: make all1 <--|'

objd3:
	objdump -D a_modif.out
objd3h:
	objdump -h a_modif.out
bin3:
	od -t x1 a_modif.bin

a_modif_lma.out: a1.o a2.o
	$(LD) -m elf_i386 a1.o a2.o -o a_modif_lma.out   -T   test2.lds
	objcopy -j def -j abc -O binary a_modif_lma.out a_modif_lma.bin
	@echo 'Please check out that out of: make objd4 is the same <--|'
	@echo 'that the out of: make objd3 <--|'
	@echo 'And check out that out of : make bin4  is the same <--|'
	@echo 'that the out of: make bin3 <--|'
	@echo 'But also check out that out of : make objd4h shows '
	@echo 'the new value of LMA of a_modif_lma.out'

objd4:
	objdump -D a_modif_lma.out
objd4h:
	objdump -h a_modif_lma.out
bin4:
	od -t x1 a_modif_lma.bin
clean:
	rm -v *.o *.out *.bin



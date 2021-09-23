#El contenido de un archivo objeto lo vemos con:
#objdump -D file.o
#Las secciones de un archivo objeto las vemos con:
#objdump -h file.o

all:a.out a.bin b.bin vma_lma.out vma_lma.bin
a.out:a1.o a2.o
	ld $^ -o $@ -T test.lds

vma_lma.out:a1.o a2.o
	ld $^ -o $@ -T test_vma_lma.lds

a1.o:a1.s
	as a1.s -o a1.o

a2.o:a2.s
	as a2.s -o a2.o

a.bin:a.out
	objcopy -j abc -j def -O binary $< $@

b.bin:a.out
	objcopy -j def -j abc -O binary $< $@

vma_lma.bin:vma_lma.out
	objcopy -j abc -j def -O binary $< $@

content_of_adotbin:a.bin
	od -t x1 $<

content_of_bdotbin:b.bin
	od -t x1 $<

content_of_vma_lmadotbin:vma_lma.bin
	od -t x1 $<

clean:
	rm -v *.o a.out a.bin b.bin vma_lma.out vma_lma.bin

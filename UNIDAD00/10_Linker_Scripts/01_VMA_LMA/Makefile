#El contenido de un archivo objeto lo vemos con:
#objdump -D file.o
#Las secciones de un archivo objeto las vemos con:
#objdump -h file.o

all:a.out a.bin content_of_adotbin

a.out:a1.o a2.o
	ld $^ -o $@ -T link_vma_lma.lds

a.bin:a.out
	objcopy -j abc -j def -O binary $< $@

content_of_adotbin:a.bin
	od -t x1 $<

clean:
	rm -v *.o a.out a.bin

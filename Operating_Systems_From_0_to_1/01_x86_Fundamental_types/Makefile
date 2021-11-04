NAME = hello

#show: $(NAME).c
#	gcc -c $< -o $(NAME).o
#	objdump -z -M intel -S -D -j .data -j .bss $(NAME).o | less
#
# Note: zero bytes are hidden with three dot symbols: ... To show all the zero bytes, 
# we add -z option. 
#
show: $(NAME).c
	gcc $< -o $(NAME)
	objdump -z -M intel -S -D -j .data -j .bss $(NAME) | less
show1:	
	readelf -x .data $(NAME)
clean:
	rm -v $(NAME)

# Intel is a little-endian machine, which means smaller addresses 
# hold bytes with smaller values, larger addresses hold byte with 
# larger values.

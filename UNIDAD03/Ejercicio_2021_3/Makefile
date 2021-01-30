RTOS := ejercicio3
ISOIMAGE := rtos.iso
.PHONY: clean default $(RTOS).exe

CC = mgcc
AR = ar
STRIP = strip
MARTE = /home/usuario1/04_Sand_box_for_MaRTE_OS/marte_2.0_22Feb2017
#CFLAGS = -Wall -Wextra -I$(MARTE)/arch/drivers/svga -I ../port -I ../include
CFLAGS = -Wall -Wextra  -I ../port -I ../include
OFLAGS = -s -Os -DNDEBUG
#LDFLAGS = -lsvga

default: $(RTOS).exe

#flynshoot.exe:
$(RTOS).exe:
	$(CC) $(CFLAGS) $(OFLAGS)  *.c $(LDFLAGS) -o $@;
	$(STRIP) --strip-all $@
#Use typing:make rtos.iso
$(ISOIMAGE):$(RTOS).exe
	@cp -v $< ../../booteable-sotr/isofiles/boot/$(RTOS)
	cd ../../booteable-sotr/ && ./make_iso.sh
	@./mover_a_dir_2019.sh
bsp_testE:bsp.c
	$(CC) -E $(CFLAGS) $(OFLAGS) $< -o $@
clean: 
	@exec echo -e "\n>> Cleaning... ";
	@find \( -name '*.[oa]' -or -name '*~*' -or -name '*.ali' -or -name '*.log' \
		-or -name '*.exe' -or -name 'mprogram' -or -name 'a.out' \) -print -delete
	@exec echo ">> End Cleaning"

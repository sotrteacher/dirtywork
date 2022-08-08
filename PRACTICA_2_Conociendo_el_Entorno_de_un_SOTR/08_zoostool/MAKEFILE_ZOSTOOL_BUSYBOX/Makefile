all: slinux.iso

slinux.iso:kernel initrd.img.gz
	echo "default slinux" >> isolinux.cfg
	echo "label slinux" >> isolinux.cfg
	echo "kernel kernel" >> isolinux.cfg
	echo "append initrd=initrd.gz" >> isolinux.cfg
	echo "timeout 300" >> isolinux.cfg
	echo "prompt 1" >> isolinux.cfg
	echo "display slinux.msg" >> isolinux.cfg
	mv initrd.img.gz isolinux/isolinux/initrd.gz
	mv isolinux.cfg isolinux/isolinux/
	echo "Presione Enter para continuar..." >> slinux.msg
	mv slinux.msg isolinux/isolinux/
	cp -v isolinux.bin isolinux/isolinux/
	cp -v kernel isolinux/isolinux/
#	make -C isolinux/
	genisoimage -r -V "slinux" -cache-inodes -J -l -b isolinux/isolinux.bin -c isolinux/boot.cat -no-emul-boot -boot-load-size 4 -boot-info-table -o isolinux/slinux.iso isolinux/

initrd.img.gz:
	chmod 775 ash
	chmod 775 bash
	dd if=/dev/zero of=initrd.img bs=512 count=8000
	mke2fs -F -m0 initrd.img
	mkdir mount
	mount -v -o loop initrd.img mount/
#	make -C mount/
	mkdir -p mount/bin mount/dev/pts mount/etc mount/lib mount/proc mount/var/log
	echo "#!/bin/bash" >> mount/linuxrc
	echo 'echo "#############    BIENVENIDO A MI SISTEMA!!    ############"' >> mount/linuxrc
	echo "PATH=/bin" >> mount/linuxrc
	echo "export PATH" >> mount/linuxrc
#	echo "mount -t proc none /proc" >> mount/linuxrc
	echo "mount -t proc /proc /proc" >> mount/linuxrc
	echo "mount -t devpts none /dev/pts" >> mount/linuxrc
	echo 'PS1="# "' >> mount/linuxrc
	echo "export PS1" >> mount/linuxrc
	echo "rm linuxrc" >> mount/linuxrc
	echo "hash -r" >> mount/linuxrc
	echo 'echo "0x100" > /proc/sys/kernel/real-root-dev' >> mount/linuxrc
	echo 'echo "TERMINANDO EJECUCION DEL SCRIPT /linuxrc"' >> mount/linuxrc
	echo "exit 0" >> mount/linuxrc
	chmod +x mount/linuxrc
	cat mount/linuxrc
#	make -C bin/
	echo "#!/bin/bash" >> mount/bin/init
	echo 'echo "Iniciando init ..."' >> mount/bin/init
	echo "rm /bin/init" >> mount/bin/init
# Crecion del dispositivo especial para el USB (ttyACM0)
	echo "mknod /dev/ttyACM0 c 166 0" >> mount/bin/init
	echo "exec /bin/bash" >> mount/bin/init
	chmod +x mount/bin/init
#	make -C ../
#	make -C dev/
	cp -a -v /dev/console mount/dev/
	cp -a -v /dev/full mount/dev/
	cp -a -v /dev/loop0 mount/dev/
	cp -a -v /dev/loop1 mount/dev/
	cp -a -v /dev/mem mount/dev/
	cp -a -v /dev/null mount/dev/
	cp -a -v /dev/port mount/dev/
# LMC 2022.01.06
#	cp -a -v /dev/ram0 mount/dev/
#	cp -a -v /dev/ram1 mount/dev/
##
	cp -a -v /dev/tty mount/dev/
	cp -a -v /dev/tty1 mount/dev/
	cp -a -v /dev/tty2 mount/dev/
	cp -a -v /dev/ttyS0 mount/dev/
	cp -a -v /dev/urandom mount/dev/
	cp -a -v /dev/zero mount/dev/
#	ln -s mount/dev/ram1 mount/dev/ram
#	ln -s /proc/kcore mount/dev/core
	cp -v Makefile_de_dev mount/dev/Makefile
	make -C mount/dev/
	rm -v mount/dev/Makefile
	cp -v Makefile_de_mount mount/Makefile
	make -C mount/
	rm -v mount/Makefile
#	cp -v ash mount/bin/
	cp -v busybox mount/bin/
	cp -v bash mount/bin/
#	chmod +x mount/bin/ash
	chmod +x mount/bin/busybox
	chmod +x mount/bin/bash
# Definicion de comandos
# Comandos que deben ser incluidos forzosamente: mount ls rm echo cd mknod
# Comandos opcionales: adduser, awk, basename, bunzip2, busubox,
# ...
# time, touch, tr, ndhcpc, umount, unzip, xargs, zcat.
	echo "#!/bin/bash" >> mount/bin/comm.sh
	echo "ln -s busybox mount" >> mount/bin/comm.sh
	echo "ln -s busybox ls" >> mount/bin/comm.sh
	echo "ln -s busybox rm" >> mount/bin/comm.sh
	echo "ln -s busybox echo" >> mount/bin/comm.sh
	echo "ln -s busybox cd" >> mount/bin/comm.sh
	echo "ln -s busybox mknod" >> mount/bin/comm.sh
	echo "ln -s busybox zcat" >> mount/bin/comm.sh
	echo "ln -s busybox adduser" >> mount/bin/comm.sh
	echo "ln -s busybox awk" >> mount/bin/comm.sh
	echo "ln -s busybox basename" >> mount/bin/comm.sh
	echo "ln -s busybox bunzip2" >> mount/bin/comm.sh
	echo "ln -s busybox busybox" >> mount/bin/comm.sh
	echo "ln -s busybox bzcat" >> mount/bin/comm.sh
	echo "ln -s busybox cat" >> mount/bin/comm.sh
	echo "ln -s busybox chgrp" >> mount/bin/comm.sh
	echo "ln -s busybox chmod" >> mount/bin/comm.sh
	echo "ln -s busybox chown" >> mount/bin/comm.sh
	echo "ln -s busybox chroot" >> mount/bin/comm.sh
	echo "ln -s busybox clear" >> mount/bin/comm.sh
	echo "ln -s busybox cp" >> mount/bin/comm.sh
	echo "ln -s busybox cut" >> mount/bin/comm.sh
	echo "ln -s busybox deallocvt" >> mount/bin/comm.sh
	echo "ln -s busybox deluser" >> mount/bin/comm.sh
	echo "ln -s busybox df" >> mount/bin/comm.sh
	echo "ln -s busybox dmesg" >> mount/bin/comm.sh
	echo "ln -s busybox du" >> mount/bin/comm.sh
	echo "ln -s busybox egrep" >> mount/bin/comm.sh
	echo "ln -s busybox expr" >> mount/bin/comm.sh
	echo "ln -s busybox fdisk" >> mount/bin/comm.sh
	echo "ln -s busybox getty" >> mount/bin/comm.sh
	echo "ln -s busybox grep" >> mount/bin/comm.sh
	echo "ln -s busybox gunzip" >> mount/bin/comm.sh
	echo "ln -s busybox gzip" >> mount/bin/comm.sh
	echo "ln -s busybox halt" >> mount/bin/comm.sh
	echo "ln -s busybox hdparm" >> mount/bin/comm.sh
	echo "ln -s busybox head" >> mount/bin/comm.sh
	echo "ln -s busybox hwclock" >> mount/bin/comm.sh
	echo "ln -s busybox ifconfig" >> mount/bin/comm.sh
	echo "ln -s busybox ifdown" >> mount/bin/comm.sh
	echo "ln -s busybox ifup" >> mount/bin/comm.sh
	echo "ln -s busybox init" >> mount/bin/comm.sh
	echo "ln -s busybox insmod" >> mount/bin/comm.sh
	echo "ln -s busybox ipcalc" >> mount/bin/comm.sh
	echo "ln -s busybox kill" >> mount/bin/comm.sh
	echo "ln -s busybox killall" >> mount/bin/comm.sh
	echo "ln -s busybox klogd" >> mount/bin/comm.sh
	echo "ln -s busybox linuxrc" >> mount/bin/comm.sh
	echo "ln -s busybox ln" >> mount/bin/comm.sh
	echo "ln -s busybox logger" >> mount/bin/comm.sh
	echo "ln -s busybox losetup" >> mount/bin/comm.sh
	echo "ln -s busybox lsmod" >> mount/bin/comm.sh
	echo "ln -s busybox mkdir" >> mount/bin/comm.sh
	echo "ln -s busybox mkswap" >> mount/bin/comm.sh
	echo "ln -s busybox modprobe" >> mount/bin/comm.sh
	echo "ln -s busybox more" >> mount/bin/comm.sh
	echo "ln -s busybox mv" >> mount/bin/comm.sh
	echo "ln -s busybox pidof" >> mount/bin/comm.sh
	echo "ln -s busybox ping" >> mount/bin/comm.sh
	echo "ln -s busybox pipe" >> mount/bin/comm.sh
	echo "ln -s busybox progess" >> mount/bin/comm.sh
	echo "ln -s busybox pivot_root" >> mount/bin/comm.sh
	echo "ln -s busybox poweroff" >> mount/bin/comm.sh
	echo "ln -s busybox ps" >> mount/bin/comm.sh
	echo "ln -s busybox reboot" >> mount/bin/comm.sh
	echo "ln -s busybox reset" >> mount/bin/comm.sh
	echo "ln -s busybox rmmod" >> mount/bin/comm.sh
	echo "ln -s busybox route" >> mount/bin/comm.sh
	echo "ln -s busybox sed" >> mount/bin/comm.sh
	echo "ln -s busybox sleep" >> mount/bin/comm.sh
	echo "ln -s busybox strings" >> mount/bin/comm.sh
	echo "ln -s busybox su" >> mount/bin/comm.sh
	echo "ln -s busybox swapoff" >> mount/bin/comm.sh
	echo "ln -s busybox sync" >> mount/bin/comm.sh
	echo "ln -s busybox sysctl" >> mount/bin/comm.sh
	echo "ln -s busybox syslogd" >> mount/bin/comm.sh
	echo "ln -s busybox tar" >> mount/bin/comm.sh
	echo "ln -s busybox time" >> mount/bin/comm.sh
	echo "ln -s busybox touch" >> mount/bin/comm.sh
	echo "ln -s busybox tr" >> mount/bin/comm.sh
	echo "ln -s busybox udhcpc" >> mount/bin/comm.sh
	echo "ln -s busybox umount" >> mount/bin/comm.sh
	echo "ln -s busybox unzip" >> mount/bin/comm.sh
	echo "ln -s busybox xargs" >> mount/bin/comm.sh
#	echo "ln -s ash zcat" >> mount/bin/comm.sh
#	echo "ln -s ash " >> mount/bin/comm.sh
	chmod +x mount/bin/comm.sh
	cp -v Makefile_mount_bin mount/bin/Makefile
	make -C mount/bin/
	rm -v mount/bin/Makefile
#	./comm.sh
#	rm -vf comm.sh
#	make -C ../../
	mkdir -p isolinux/isolinux
#	umount -v initrd.img
	umount -v mount/
# Aqui se crea el archivo initrd.img.gz
	gzip -9 initrd.img
# Inclucion de imagen de inicio
# Debe ser una imagen de 640 x 480 pixeles a 14 colores
#	cp -v img_file isolinux/isolinux/slinux.logo
#	echo "slinux.logo" >> slinux.msg


clean:
	rm -rvf initrd.img isolinux/ mount/


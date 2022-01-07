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
	dd if=/dev/zero of=initrd.img bs=512 count=4000
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
	cp -v ash mount/bin/
	cp -v bash mount/bin/
	chmod +x mount/bin/ash
	chmod +x mount/bin/bash
# Definicion de comandos
# Comandos que deben ser incluidos forzosamente: mount ls rm echo cd mknod
# Comandos opcionales: adduser, awk, basename, bunzip2, busubox,
# ...
# time, touch, tr, ndhcpc, umount, unzip, xargs, zcat.
	echo "#!/bin/bash" >> mount/bin/comm.sh
	echo "ln -s ash mount" >> mount/bin/comm.sh
	echo "ln -s ash ls" >> mount/bin/comm.sh
	echo "ln -s ash rm" >> mount/bin/comm.sh
	echo "ln -s ash echo" >> mount/bin/comm.sh
	echo "ln -s ash cd" >> mount/bin/comm.sh
	echo "ln -s ash mknod" >> mount/bin/comm.sh
	echo "ln -s ash zcat" >> mount/bin/comm.sh
	echo "ln -s ash adduser" >> mount/bin/comm.sh
	echo "ln -s ash awk" >> mount/bin/comm.sh
	echo "ln -s ash basename" >> mount/bin/comm.sh
	echo "ln -s ash bunzip2" >> mount/bin/comm.sh
	echo "ln -s ash busybox" >> mount/bin/comm.sh
	echo "ln -s ash bzcat" >> mount/bin/comm.sh
	echo "ln -s ash cat" >> mount/bin/comm.sh
	echo "ln -s ash chgrp" >> mount/bin/comm.sh
	echo "ln -s ash chmod" >> mount/bin/comm.sh
	echo "ln -s ash chown" >> mount/bin/comm.sh
	echo "ln -s ash chroot" >> mount/bin/comm.sh
	echo "ln -s ash clear" >> mount/bin/comm.sh
	echo "ln -s ash cp" >> mount/bin/comm.sh
	echo "ln -s ash cut" >> mount/bin/comm.sh
	echo "ln -s ash deallocvt" >> mount/bin/comm.sh
	echo "ln -s ash deluser" >> mount/bin/comm.sh
	echo "ln -s ash df" >> mount/bin/comm.sh
	echo "ln -s ash dmesg" >> mount/bin/comm.sh
	echo "ln -s ash du" >> mount/bin/comm.sh
	echo "ln -s ash egrep" >> mount/bin/comm.sh
	echo "ln -s ash expr" >> mount/bin/comm.sh
	echo "ln -s ash fdisk" >> mount/bin/comm.sh
	echo "ln -s ash getty" >> mount/bin/comm.sh
	echo "ln -s ash grep" >> mount/bin/comm.sh
	echo "ln -s ash gunzip" >> mount/bin/comm.sh
	echo "ln -s ash gzip" >> mount/bin/comm.sh
	echo "ln -s ash halt" >> mount/bin/comm.sh
	echo "ln -s ash hdparm" >> mount/bin/comm.sh
	echo "ln -s ash head" >> mount/bin/comm.sh
	echo "ln -s ash hwclock" >> mount/bin/comm.sh
	echo "ln -s ash ifconfig" >> mount/bin/comm.sh
	echo "ln -s ash ifdown" >> mount/bin/comm.sh
	echo "ln -s ash ifup" >> mount/bin/comm.sh
	echo "ln -s ash init" >> mount/bin/comm.sh
	echo "ln -s ash insmod" >> mount/bin/comm.sh
	echo "ln -s ash ipcalc" >> mount/bin/comm.sh
	echo "ln -s ash kill" >> mount/bin/comm.sh
	echo "ln -s ash killall" >> mount/bin/comm.sh
	echo "ln -s ash klogd" >> mount/bin/comm.sh
	echo "ln -s ash linuxrc" >> mount/bin/comm.sh
	echo "ln -s ash ln" >> mount/bin/comm.sh
	echo "ln -s ash logger" >> mount/bin/comm.sh
	echo "ln -s ash losetup" >> mount/bin/comm.sh
	echo "ln -s ash lsmod" >> mount/bin/comm.sh
	echo "ln -s ash mkdir" >> mount/bin/comm.sh
	echo "ln -s ash mkswap" >> mount/bin/comm.sh
	echo "ln -s ash modprobe" >> mount/bin/comm.sh
	echo "ln -s ash more" >> mount/bin/comm.sh
	echo "ln -s ash mv" >> mount/bin/comm.sh
	echo "ln -s ash pidof" >> mount/bin/comm.sh
	echo "ln -s ash ping" >> mount/bin/comm.sh
	echo "ln -s ash pipe" >> mount/bin/comm.sh
	echo "ln -s ash progess" >> mount/bin/comm.sh
	echo "ln -s ash pivot_root" >> mount/bin/comm.sh
	echo "ln -s ash poweroff" >> mount/bin/comm.sh
	echo "ln -s ash ps" >> mount/bin/comm.sh
	echo "ln -s ash reboot" >> mount/bin/comm.sh
	echo "ln -s ash reset" >> mount/bin/comm.sh
	echo "ln -s ash rmmod" >> mount/bin/comm.sh
	echo "ln -s ash route" >> mount/bin/comm.sh
	echo "ln -s ash sed" >> mount/bin/comm.sh
	echo "ln -s ash sleep" >> mount/bin/comm.sh
	echo "ln -s ash strings" >> mount/bin/comm.sh
	echo "ln -s ash su" >> mount/bin/comm.sh
	echo "ln -s ash swapoff" >> mount/bin/comm.sh
	echo "ln -s ash sync" >> mount/bin/comm.sh
	echo "ln -s ash sysctl" >> mount/bin/comm.sh
	echo "ln -s ash syslogd" >> mount/bin/comm.sh
	echo "ln -s ash tar" >> mount/bin/comm.sh
	echo "ln -s ash time" >> mount/bin/comm.sh
	echo "ln -s ash touch" >> mount/bin/comm.sh
	echo "ln -s ash tr" >> mount/bin/comm.sh
	echo "ln -s ash udhcpc" >> mount/bin/comm.sh
	echo "ln -s ash umount" >> mount/bin/comm.sh
	echo "ln -s ash unzip" >> mount/bin/comm.sh
	echo "ln -s ash xargs" >> mount/bin/comm.sh
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


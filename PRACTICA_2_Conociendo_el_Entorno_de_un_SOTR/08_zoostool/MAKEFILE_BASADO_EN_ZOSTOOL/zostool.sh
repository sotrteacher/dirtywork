#!/bin/bash

#############################################################################
#  zostool.sh    -  Zavael Operating System Tool 0.1 beta
#
#  Copyright (C) 2009 Edgar A. García-Martínez  <zavael@gmail.com>
#
#############################################################################

#Seleccionar con que tipo de dilog quieres trabajar
#     dialog  -> para linea de comandos
#     gdialog -> para dialog con gnome

_COM_DIALOG_=gdialog

$_COM_DIALOG_ --title "Zavael Operating System Tool 0.1 beta" --msgbox "Bienvenido a Zavael Operating System Tool 0.1 beta\n\nInformacion: Antes de comenzar recuerda ejecutar este asistente de creacion del SO en modo superusuario(con privilegios de usuario root) para que pueda funcionar todo correctamente." 0 0
check_reg=`ls reg | wc -l`
if [ "$check_reg" -eq 1 ]; then
  rm reg
  echo "part1.empty">>reg
  echo "part2.empty">>reg
  echo "part3.empty">>reg
  echo "part4.empty">>reg
  echo "part5.empty">>reg
else
  echo "part1.empty">>reg
  echo "part2.empty">>reg
  echo "part3.empty">>reg
  echo "part4.empty">>reg
  echo "part5.empty">>reg
fi
root=`pwd`
chmod 775 ash
chmod 775 "bash"
main=1
while [ "$main" -eq 1 ]; do
  $_COM_DIALOG_ --stdout --title "Zavael Operating System Tool 0.1 beta" --menu "Pasos a seguir:" 15 70 7 '1' 'Ubicacion del Kernel' '2' 'Creacion imagen initrd' '3' 'Definir comandos' '4' 'Incrustar programas' '5' 'Crear .iso' '6' 'Quemar CD-ROM' 'S' 'Salir' 2>> dialog.$$
  opc=$(head -1 dialog.$$)
  case $opc in #Menu
    1) #Copia Kernel
      $_COM_DIALOG_ --title "ZOSTOOL - Informacion sobre el kernel" --msgbox "Informacion: Antes de llenar el apartado que sigue, recuerda que debes tener un kernel." 0 80
      $_COM_DIALOG_ --stdout --title "ZOSTOOL - Ubicacion del Kernel" --inputbox "Escriba la ruta completa de la ubicacion del kernel que se usara:" 0 80 2>> rutak.$$
      kernel_ruth=$(head -1 rutak.$$)
      rm -r rutak.$$
      echo "Ruta: [$kernel_ruth]"
      check_kernel=`ls $kernel_ruth | wc -l`
      if [ "$check_kernel" -eq 1 ]; then #Checa la ubicacion del Kernel
        check_exist_kernel=`ls kernel | wc -l`
        if [ "$check_exist_kernel" -eq 1 ]; then #Checa si ya existe un Kernel
          $_COM_DIALOG_ --stdout --title "Ya existe el kernel" --menu "¿Desea sustituirlo?" 0 70 2 '1' 'Aceptar' '2' 'Cancelar' 2>> kernels.$$
          change_kernel=$(head -1 kernels.$$)
	  rm -rf kernels.$$
          if [ "$change_kernel" -eq 1 ]; then #Sustituye al kernel
            cp $kernel_ruth $(pwd)/kernel
	    echo "El kernel [$kernel_ruth] fue copiado satisfactoriamente..."
            check_state_p1=`grep "part1.empty" reg | wc -l`
            if [ "$check_state_p1" -eq 1 ]; then #Checa registro de part1
              cd $root
              grep -v --invert-match 'part1' reg>>reg.temp
              rm reg
              mv reg.temp reg
              echo "part1.done">>reg
            fi
            $($_COM_DIALOG_ --title "Zavael Operating System Tool 0.1 beta" --msgbox "El kernel $kernel_ruth fue sustituido en $root.\n\n  Esta parte del asistente se concluyo satisfactoriamente. Ya puede continuar con el siguiente paso del asistente." 0 70)
          fi
        else #Si no existe un kernel
          cp $kernel_ruth $(pwd)/kernel
          $_COM_DIALOG_ --title "Zavael Operating System Tool 0.1 beta" --msgbox "El kernel $kernel_ruth fue copiado en $root.\n\n  Esta parte del asistente se concluyo satisfactoriamente. Ya puede continuar con el siguiente paso del asistente." 0 70
          check_state_p1=`grep "part1.empty" reg | wc -l`
          if [ "$check_state_p1" -eq 1 ]; then #Checa registro de part1
            cd $root
            grep -v --invert-match "part1" reg>>reg.temp
            rm reg
            mv reg.temp reg
            echo "part1.done">>reg
          fi
        fi
      else #Ruta incorrecta del Kernel
        $_COM_DIALOG_ --title "Zavael Operating System Tool 0.1 beta" --msgbox "Advertencia: La ruta que especifico es incorrecta. Intentelo de nuevo. Es necesario concluir este paso del asistente para poder continuar con los pasos siguientes." 0 70
      fi
    ;;
    2)#Crea la imagen initrd
      bs=512
      count=4000
      $_COM_DIALOG_ --stdout --title "Definir Tamaño de los bloques" --menu "Tamaño del bloque:" 10 60 3 '1' 'Definir tamaño' '2' 'Valor por default (512)' '3' 'Cancelar' 2>> blocks.$$
      block_size=$(head -1 blocks.$$);
      rm -rf blocks.$$
      if [ "$block_size" -eq 1 ]; then #Tamaño de bloques definido p el usuario
        $_COM_DIALOG_ --stdout --title "Tamaño de los bloques:" --inputbox "Defina el tamaño de los bloques(Ej. 512):" 0 70 2>> bs.$$
	bs=$(head -1 bs.$$)
	rm -rf bs.$$
      elif [ "$block_size" -eq 3 ]; then
        return 1
      fi
      $_COM_DIALOG_ --stdout --title "Definir el Tamaño de la initrd" --menu "Definir el tamaño de la imagen initrd" 10 60 3 '1' 'Definir tamaño' '2' 'Valor por default (4000)' '3' 'Cancelar' 2>> imgsize.$$
      img_size=$(head -1 imgsize.$$)
      rm -rf imgsize.$$
      if [ "$img_size" -eq 1 ]; then #Tamaño de la imagen definido p el usuario
        $_COM_DIALOG_ --stdout --title "Tamaño de la initrd" --inputbox "Defina el tamaño de la imagen initrd(Ej. 4000):" 0 0 2>> count.$$
	count=$(head -1 count.$$)
	rm -rf count.$$
      elif [ "$img_size" -eq 3 ]; then
        return 1
      fi
      dd if=/dev/zero of=initrd.img bs=$bs count=$count
      mke2fs -F -m0 initrd.img
      mkdir "mount"
      mount initrd.img "mount" -o loop
      cd "mount"
      mkdir -p bin dev/pts etc lib proc var/log
      echo "#!/bin/bash">>linuxrc
      echo 'echo "Bienvenido a mi sistema"'>>linuxrc
      echo "PATH=/bin">>linuxrc
      echo "export PATH">>linuxrc
      echo "mount -t proc none /proc">>linuxrc
      echo "mount -t devpts none /dev/pts">>linuxrc
      echo 'PS1="# "'>>linuxrc
      echo "export PS1">>linuxrc
      echo "rm -f linuxrc">>linuxrc
      echo "hash -r">>linuxrc
      echo 'echo "0x100" > /proc/sys/kernel/real-root-dev'>>linuxrc
      echo "exit 0 ">>linuxrc
      chmod +x linuxrc
      cd bin
      echo "#!/bin/bash">>init
      echo 'echo "Iniciando init..."'>>init
      echo "rm -rf /bin/init">>init
      #crea dispositivo especial para el USB(ttyACM0)
      echo "mknod /dev/ttyACM0 c 166 0">>init
      echo "exec /bin/bash">>init
      chmod +x init
      cd ..
      cd dev
      cp -a /dev/console .
      cp -a /dev/full .
      cp -a /dev/loop0 .
      cp -a /dev/loop1 .
      cp -a /dev/mem .
      cp -a /dev/null .
      cp -a /dev/port .
      cp -a /dev/ram0 .
      cp -a /dev/ram1 .
      cp -a /dev/tty .
      cp -a /dev/tty1 .
      cp -a /dev/tty2 .
      cp -a /dev/ttyS0 .
      cp -a /dev/urandom .
      cp -a /dev/zero .
      ln -s ram1 ram
      ln -s /proc/kcore core
      cd $root
      check_state_p2=`grep "part2.empty" reg | wc -l`
      $_COM_DIALOG_ --title "ZOSTOOL - Creacion de la imagen initrd" --msgbox "Informacion: La imagen initrd fue creada satisfactoriamente.\n\n  Este paso del asistente fue concluido correctamente. Ya puede continuar al siguiente paso." 0 70
      if [ "$check_state_p2" -eq 1 ]; then #Checa registro de part2
        cd $root
        grep -v --invert-match "part2" reg>>reg.temp
        rm reg
        mv reg.temp reg
        echo "part2.done">>reg
      fi
    ;;
    3) #Define comandos
      cd $root/"mount"/bin
      cp $root/ash .
      cp $root/"bash" .
      main3=1
      $_COM_DIALOG_ --title "ZOSTOOL - Definicion de comandos del SO" --msgbox "Bienvenido a la parte de creacion de comandos del SO. \n\nAdvertencia: Rescuerda que los comandos basicos que debes crear para que el SO pueda correr son los siguientes: \n  mount\n  rm\n  ls\n  echo\n  cd\n\nAsí que debes recordar crear estos comandos forzosamente.\n\nInformacion: Recuerda que si quieres darle soporte al USB debes crear el comando mknod para que pueda ser creado el dispositivo especial. Tambien puedes crear los comandos adicionales que quieras. A continuacion se muestra una lista de los comandos soportados en esta version." 0 80
      $_COM_DIALOG_ --title "ZOSTOOL - Comandos soportados" --msgbox "Los comandos soportados que puedes crear en esta version son los siguientes: \n\nadduser, awk, basename, bunzip2, busybox, bzcat, cat, chgrp, chmod, chown, chroot, clear, cp, cut, deallocvt, deluser, df, dmesg, du, echo, egrep, expr, fdisk, getty, grep, gunzip, gzip, halt, hdparm, head, hwclock, ifconfig, ifdown, ifup, init, insmod, ipcalc, kill, killall, klogd, linuxrc, ln, logger, losetup, ls, lsmod, mkdir, mknod, mkswap, modprobe, more, mount, mv, pidof, ping, pipe_progress, pivot_root, poweroff, ps, reboot, reset, rm, rmmod, route, sed, sleep, strings, su, swapoff, swapon, sync, sysctl, syslogd, tar, time, touch, tr, udhcpc, umount, unzip, xargs, zcat" 0 80
      while [ "$main3" -eq 1 ]; do #Registra los comandos
        $_COM_DIALOG_ --stdout --title "ZOSTOOL - Definicion de comandos del SO" --inputbox "Escriba los comandos uno por uno, que quiera utilizar en su SO (escriba "-end" para terminar)" 10 40 2>>commands.$$
	comm=$(head -1 commands.$$)
	rm -rf commands.$$
        if [ "$comm" == "-end" ]; then #Terminar registro de comandos
          show_comm=`cat comm | cut -f 4 -d ' '`
          echo $show_comm>>comm.temp
          $_COM_DIALOG_ --title "Comandos seleccionados:" --textbox "comm.temp" 10 80
          echo "#!/bin/bash">>comm.sh
          cat comm>>comm.sh
          rm comm
          chmod +x comm.sh
          ./comm.sh
          $_COM_DIALOG_ --title "ZOSTOOL - Enlaces creados" --msgbox "Informacion: Los enlaces fueron creados satisfactoriamente.\n\n  Este paso del asistente fue concluido correctamente. Ya puede continuar al siguiente paso." 0 70
          rm comm.sh
          rm comm.temp
          cd $root
          check_state_p3=`grep "part3.empty" reg | wc -l`
          if [ "$check_state_p3" -eq 1 ]; then #Checa registro de part3
            cd $root
            grep -v --invert-match "part3" reg>>reg.temp
            rm reg
            mv reg.temp reg
            echo "part3.done">>reg
            main3=0
          fi
        else #Captura de comandos en archivo .sh
          echo "ln -s ash" $comm>>comm
        fi
      done
    ;;
    4)#Incrustar programas
      $_COM_DIALOG_ --title "ZOSTOOL - Incrustar programa al SO" --msgbox "Informacion: Recuerda haber creado antes tu programa y haberlo compilado de forma estatica y para 32 bits para que pueda funcionar. \n\nPara compilar un programa de forma estatica y para 32 bits se hace de la siguiente forma:\n\n  Para arquitecturas de 32 bits se usa el siguiente comando:\n    gcc --static -o nombre_programa archivo_fuente.c\n\n  Para arquitecturas de 64 bits se usa el siguiente comando:\n    gcc -m32 --static -o nombre_programa archivo_fuente.c\n\n  Advertencia: para compilar a 32 bits desde arquitecturas de 64 bits debes instalar el paquete libc6-dev-i386 para que no aparezcan errores al momento de compilar.\n\n    Para instalar este paquete, escribe el siguiente comando:\n      apt-get install libc6-dev-i386" 
      cd $root/mount/bin
      main4=1
      while [ "$main4" -eq 1 ]; do #Pide ruta de la aplicacion
        $_COM_DIALOG_ --stdout --title "Ubicacion del programa"  --inputbox "Escriba la ruta de la aplicacion que desee incluir (escriba "-end" para terminar)" 0 0 2>> program.$$
	prog=$(head -1 program.$$)
	rm -rf program.$$
        if [ "$prog" == "-end" ]; then #Termina de capturar los programas
          show_prog=`cat name_prog | cut -f 4 -d ' '`
          echo $show_prog>>prog.temp
          $_COM_DIALOG_ --title "ZOSTOOL - Programas seleccionados:" --textbox "prog.temp" 10 40
          echo "#!/bin/bash">>prog.sh
          echo "#!/bin/bash">>name_prog.sh
          cat prog>>prog.sh
          cat name_prog>>prog.sh
          rm prog
          rm name_prog
          chmod +x prog.sh
          chmod +x name_prog.sh
          ./prog.sh
          ./name_prog.sh
          $_COM_DIALOG_ --title "ZOSTOOL - Programas incrustados" --msgbox "Informacion: Los programas fueron incrustados satisfactoriamente.\n\n  Este paso del asistente fue concluido correctamente. Ya puede continuar al siguiente paso." 0 0
          rm prog.sh
          rm name_prog.sh
          rm prog.temp
          cd $root
          check_state_p4=`grep "part4.empty" reg | wc -l`
          if [ "$check_state_p4" -eq 1 ]; then #Checa registro de part4
            cd $root
            grep -v --invert-match "part4" reg>>reg.temp
            rm reg
            mv reg.temp reg
            echo "part4.done">>reg
            main4=0
          fi          
        fi
        check_prog=`ls $prog | wc -l`
        if [ "$check_prog" -eq 1 ]; then #Checa si la ruta es correcta
          main4_name_prog=1
          while [ "$main4_name_prog" -eq 1 ]; do #Pide nombre de la aplicacion
            $_COM_DIALOG_ --stdout --title "ZOSTOOL - Nombre de la aplicacion" --inputbox "Asigne un nombre a la aplicacion ($prog):" 0 0 2>> name_prog.$$
            name_prog=$(head -1 name_prog.$$)
            rm -rf name_prog.$$
            check_name_prog=`ls $name_prog | wc -l`
            if [ "$check_name_prog" -eq 1 ]; then #Checa si existe el nombre
              $_COM_DIALOG_ --title "ZOSTOOL - Nombre de la aplicacion" --msgbox "Advertencia: El nombre que elijio ya existe" 0 0
            else #Crea enlaces de la aplicacion
              echo "cp" $prog $name_prog>>prog
	      echo $name_prog>>name_prog
              #echo "ln -s" $prog $name_prog>>name_prog
              main4_name_prog=0
            fi
          done
        else #Ruta de la aplicacion incorrecta
          if [ "$main4" -eq 1 ]; then
            $_COM_DIALOG_ --title "ZOSTOOL - Ruta del programa" --msgbox "Advertencia: La ruta del programa que especifico no es correcta" 0 0
          fi
        fi
      done
    ;;
    5) #Crea la imagen .iso
      $_COM_DIALOG_ --title "ZOSTOOL - Creacion de la imagen iso" -- msgbox "Informacion: Para crear la imagen iso recuerda que debes tener la imagen en formato lss16(conviertala primero) y saber la ruta donde esta se encuentra, ademas del archivo de ayuda que deseas incluir. En versiones posteriores se le dara soporte a la creacion automatica de las imagenes." 0 70
      cd $root
      check_p1=`grep "part1.done" reg | wc -l`
      check_p2=`grep "part2.done" reg | wc -l`
      check_p3=`grep "part3.done" reg | wc -l`
      check_p4=`grep "part4.done" reg | wc -l`
      if [ "$check_p1" -eq 1 ]; then #Checa part1 done
        if [ "$check_p2" -eq 1 ]; then #Checa part2 done
          if [ "$check_p3" -eq 1 ]; then #Checa part3 done
            cd $root
            mkdir -p isolinux/isolinux
            umount initrd.img
            gzip -9 initrd.img
            $_COM_DIALOG_ --stdout --title "Creacion del iso - Incluir imagen de inicio" --menu "¿Desea incluir una imagen de inicio?" 10 60 2 '1' 'Aceptar' '2' 'Cancelar' 2>> opc_img_iso.$$
            opc_img_iso=$(head -1 opc_img_iso.$$)
            rm -rf opc_img_iso.$$
            main5_img_iso=1
            while [ "$main5_img_iso" -eq 1 ]; do
              if [ "$opc_img_iso" -eq 1 ]; then
                $_COM_DIALOG_ --title "ZOSTOOL - Informacion sobre Imagen de inicio" --msgbox "La imagen de inicio debe medir 640x480 a 14 colores, de lo contrario no se mostrara o marcara algun error al cargar su Sistema" 0 0
                $_COM_DIALOG_ --stdout --title "ZOSTOOL - Ubicacion de la imagen de inicio" --inputbox "Escriba la ruta completa de la ubicacion de la imagen:" 0 0 2>> img_iso.$$
                img_iso=$(head -1 img_iso.$$)
		echo "Imagen iso: [$img_iso]"
                rm -rf img_iso.$$
                check_img_iso=`ls $img_iso | wc -l`
                if [ "$check_img_iso" -eq 1 ]; then
                  cp $img_iso isolinux/isolinux/slinux.logo
                  echo "slinux.logo">>slinux.msg
                  main5_img_iso=0
                else
                  $_COM_DIALOG_ --title "ZOSTOOL - Imagen de inicio" --msgbox "Advertencia: La ruta de la imagen de inicio es incorrecta, intentelo de nuevo." 0 0
                fi
              else
                $_COM_DIALOG_ --title "ZOSTOOL - Imagen de inicio" --msgbox "Informacion: No se inserto ninguna imagen de inicio." 0 0
                main5_img_iso=0
              fi
            done
            echo "default slinux">>isolinux.cfg
            echo "label slinux">>isolinux.cfg
            echo "kernel kernel">>isolinux.cfg
            echo "append initrd=initrd.gz">>isolinux.cfg
            echo "timeout 300">>isolinux.cfg
            echo "prompt 1">>isolinux.cfg
            echo "display slinux.msg">>isolinux.cfg
            $_COM_DIALOG_ --stdout --title "ZOSTOOL - Agregar archivo de ayuda al isolinux" --menu "¿Desea agregar un archivo de ayuda?" 8 60 2 '1' 'Aceptar' '2' 'Cancelar' 2>> opc_help_iso.$$
            opc_help_iso=$(head -1 opc_help_iso.$$)
            rm -rf opc_help_iso.$$
            main5_help_iso=1
            while [ "$main5_help_iso" -eq 1 ]; do
              if [ "$opc_help_iso" -eq 1 ]; then
                $_COM_DIALOG_ --stdout --title "ZOSTOOL - Ubicacion del archivo de ayuda" --inputbox "Escriba la ruta completa de la ubicacion del archivo de texto de ayuda:" 0 0 2>> help_iso.$$
                help_iso=$(head -1 help_iso.$$)
                rm -rf help_iso.$$
                check_help_iso=`ls $help_iso | wc -l`
                if [ "$check_help_iso" -eq 1 ]; then
                  cp $help_iso isolinux/isolinux/f1.msg
                  echo "f1 f1.msg">>isolinux.cfg
                  echo "Presione F1 para ayuda o Enter para continuar...">>slinux.msg
                  main5_help_iso=0
                else
                  $_COM_DIALOG_ --title "ZOSTOOL - Ubicacion del archivo de ayuda" --msgbox "Advertencia: La ruta del archivo de ayuda es incorrecta" 0 0
                fi
              else
                echo "Presione Enter para continuar...">>slinux.msg
                main5_help_iso=0
              fi
            done
            mv initrd.img.gz isolinux/isolinux/initrd.gz
            mv isolinux.cfg isolinux/isolinux
            mv slinux.msg isolinux/isolinux
            #cp "boot.cat" isolinux/isolinux
            cp isolinux.bin isolinux/isolinux
            cp kernel isolinux/isolinux
            cd isolinux
#            name_iso=`dialog --inputbox --stdout "Escribe el nombre de tu Sistema Operativo:" 0 0`
            mkisofs -r -V "slinux" -cache-inodes -J -l -b isolinux/isolinux.bin -c isolinux/boot.cat -no-emul-boot -boot-load-size 4 -boot-info-table -o slinux.iso .
            path_iso=$(pwd)
            $_COM_DIALOG_ --title "ZOSTOOL - Creacion de la imagen iso" --msgbox "Informacion: La imagen iso fue creada satisfactoriamente.\n\nEl archivo de la imagen iso que fue creado se encuentra en la siguiente ruta:\n[$path_iso/slinux.iso].\n\n  Si desea probar el SO digite el siguiente comando:\n\n    qemu -cdrom $path_iso/slinux.iso\n\n    Advertencia: Debes tener instalado qemu para poder emular el SO. Para instalarlo ejecuta el siguiente comando:\n\n      apt-get install qemu"
#            mkisofs -l -r -J -V "slinux" -cache-inodes -J -l -b isolinux.bin -c boot.cat -no-emul-boot -boot-load-size 4 -boot-info-table -o slinux.iso .

            cd $root
            check_state_p5=`grep "part5.empty" reg | wc -l`
            if [ "$check_state_p5" -eq 1 ]; then #Checa registro de part5
              grep -v --invert-match "part5" reg>>reg.temp
              rm reg
              mv reg.temp reg
              echo "part5.done">>reg
            fi
          else
            $_COM_DIALOG_ --title "ZOSTOOL - Imagen iso" --msgbox "Advertencia: No se puede crear la imagen .iso debido a que aun faltan elementos para continuar" 0 0
          fi
        else
          $_COM_DIALOG_ --title "ZOSTOOL - Imagen iso" --msgbox "Advertencia: No se puede crear la imagen .iso debido a que aun faltan elementos para continuar" 0 0
        fi
      else #Faltan elementos para crear la imagen .iso
        $_COM_DIALOG_ --title "ZOSTOOL - Imagen iso" --msgbox "Advertencia: No se puede crear la imagen .iso debido a que aun faltan elementos para continuar" 0 0
      fi
    ;;
    6)
      cd $root
      check_p5=`grep "part5.done" reg | wc -l`
      if [ "$check_p5" -eq 1 ]; then
        /dev/hdb eject
        #cdrecord
      else
        $_COM_DIALOG_ --title "ZOSTOOL - Quemar la imagen iso" --msgbox "Aun no tienes una imagen .iso o esta dentro de otro directorio" 0 0
      fi
    ;;
    S)
      main=0
      rm -rf dialog.$$
    ;;
  esac
  rm -rf dialog.$$
done
rm reg
umount "mount"
rm -rf "mount"

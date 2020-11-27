# Fly 'n' Shoot
This repository contains ports for the **Fly 'n' Shoot** game originally developed by *Miro Samek* for the following platforms:
- **MaRTE OS** real-time operating system *(master branch)*
- ***NIX systems** using an ncurses-compatible terminal *(curses branch)*

The port is based on the *QEP/C 80x86 real mode vanilla DOS* version 5.3.1 of the example code provided by the *QP platform by Quantum Leaps*.

## Port authors
Port development by:
- José Fausto Romero Lujambio
- Rodríguez Echeverría Pastor Alan

of the  [Interdisciplinary Professional Unit in Engineering and Advanced Technologies of the National Polytechnic Institute of Mexico *(UPIITA-IPN)*](https://www.upiita.ipn.mx) for the *Real-Time Operating Systems* learning unit in charge of B.Eng. Lamberto Maza Casas.

## Common changes
All the code is built using the *GNU C Compiler* and a custom *Makefile* is provided for each platform port.

DOS interrupt vectors for keyboard and clock events have been replaced by two supporting procedures dispatched inside a thread (using *pthreads*) that also handles screen redraws. Remaining interrupt-related code has been commented or removed.

The body of a single iteration of the *QF_run()* procedure of the vanilla QP kernel has been separated into its own procedure *QF_run_event_loop()* and placed in a loop dispatched inside a thread in order to allow handling of thread events and capping the execution rate of the kernel.

The code for all video support features has been rewritten accordingly for each platform port by first wrapping the video displaying calls in a virtual terminal for DOS-emulation that supports background and foreground coloring as well as text blinking.

## MaRTE OS version
The port for the *MaRTE OS* platform developed by the *University of Cantabria, Spain* adapts code for video and font support from the *MaRTEnoid* game example by *Álvaro García Cuesta*.

## ncurses version
The port for **NIX* platforms is based on the *ncurses* programming library developed by the *GNU Project*.

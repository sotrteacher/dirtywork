// File: full_adder.cpp
/** REF. Page 22, Chapter 2 Getting Started,
 * 2.4 Verifying the Functionality
 * J. Bhasker - A SystemC Primer-Star Galaxy Pub (2002).djvu
 */
#include "full_adder.h"
void full_adder::prc_or () {
  carry_out = c1 | c2;
}

/**
 * Cuando el tama\~no de este archivo era menor a 512 (es
 * decir, menos de 512 bytes), en las propiedades del archivo
 * en Windows 10, el tama\~no del archivo en disco aparec\'ia
 * como de 0 bytes. Y al parecer esto es la causa de que en
 * en el CodeBlocks lanzado desde la App Debian el archivo
 * se abre "en blanco". Si el tam
 */



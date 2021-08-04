// File: driver.cpp
/** REF. Page 24, Chapter 2 Getting Started,
 * 2.4 Verifying the Functionality
 * J. Bhasker - A SystemC Primer-Star Galaxy Pub (2002).djvu
 */
#include "driver.h"
void driver::prc_driver (){
  sc_uint<3> pattern;
  pattern = 0;

  while (1) {
    d_a = pattern[0];
    d_b = pattern[1];
    d_cin = pattern[2];
    wait (5, SC_NS);
    pattern++;
  }
}

/**
 * Cuando el tama\~no de esta archivo era menor a 512 (es
 * decir, menos de 512 bytes), en las propiedades del archivo
 * en Windows 10, el tama\~no del archivo en disco aparec\'ia
 * como de 0 bytes. Y al parecer esto es la causa de que en
 * en el CodeBlocks lanzado desde la App Debian el archivo
 * se abre "en blanco".
 */

// File: driver.h
/** REF. Page 24, Chapter 2 Getting Started,
 * 2.4 Verifying the Functionality
 * J. Bhasker - A SystemC Primer-Star Galaxy Pub (2002).djvu
 */
#ifndef DRIVER_H_INCLUDED
#define DRIVER_H_INCLUDED
#include <systemc.h> /* add -I<path to your systemc> in your
                        make file, or
                        Project->Build Options->Search Directories */
SC_MODULE (driver) {
  sc_out<bool> d_a, d_b, d_cin;
  void prc_driver ();
  SC_CTOR (driver) {
    SC_THREAD (prc_driver);
  }
};
#endif // DRIVER_H_INCLUDED

/**
 * Cuando el tama\~no de esta archivo era menor a 512 (es
 * decir, menos de 512 bytes), en las propiedades del archivo
 * en Windows 10, el tama\~no del archivo en disco aparec\'ia
 * como de 0 bytes. Y al parecer esto es la causa de que en
 * en el CodeBlocks lanzado desde la App Debian el archivo
 * se abre "en blanco".
 */

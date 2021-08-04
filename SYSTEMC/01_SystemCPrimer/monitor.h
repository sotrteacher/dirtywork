// File: monitor.h
/** REF. Page 25, Chapter 2 Getting Started,
 * 2.4 Verifying the Functionality
 * J. Bhasker - A SystemC Primer-Star Galaxy Pub (2002).djvu
 */
#ifndef MONITOR_H_INCLUDED
#define MONITOR_H_INCLUDED
#include <systemc.h>
SC_MODULE (monitor) {
  sc_in<bool> m_a, m_b, m_cin, m_sum, m_cout;
  void prc_monitor ();
  SC_CTOR (monitor) {
    SC_METHOD (prc_monitor);
    sensitive << m_a << m_b << m_cin << m_sum<< m_cout;
  }
};
#endif // MONITOR_H_INCLUDED

/**
 * Cuando el tama\~no de esta archivo era menor a 512 (es
 * decir, menos de 512 bytes), en las propiedades del archivo
 * en Windows 10, el tama\~no del archivo en disco aparec\'ia
 * como de 0 bytes. Y al parecer esto es la causa de que en
 * en el CodeBlocks lanzado desde la App Debian el archivo
 * se abre "en blanco".
 */

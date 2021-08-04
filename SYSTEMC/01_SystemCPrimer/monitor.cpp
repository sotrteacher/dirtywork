// File: monitor.cpp
/** REF. Page 25, Chapter 2 Getting Started,
 * 2.4 Verifying the Functionality
 * J. Bhasker - A SystemC Primer-Star Galaxy Pub (2002).djvu
 */
#include "monitor.h"
void monitor::prc_monitor () {
  cout << "At time" << sc_time_stamp() << "::";
  cout << "(a, b, carry_in): ";
  cout << m_a << m_b << m_cin;
  cout << " (sum, carry_out): "<< m_sum
       << m_cout << endl;
}

/**
 * Cuando el tama\~no de esta archivo era menor a 512 (es
 * decir, menos de 512 bytes), en las propiedades del archivo
 * en Windows 10, el tama\~no del archivo en disco aparec\'ia
 * como de 0 bytes. Y al parecer esto es la causa de que en
 * en el CodeBlocks lanzado desde la App Debian el archivo
 * se abre "en blanco".
 */

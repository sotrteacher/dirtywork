// File: half_adder.cpp                 // Line 1
#include "half_adder.h"                 // Line 2
void half_adder::prc_half_adder () {    // Line 3
  sum = a ^ b;                          // Line 4
  carry = a & b;                        // Line 5
}

/**
 * REF. Page 17, Chapter 2 Getting Started, 2.1 Basics.
 * half adder example
 * J. Bhasker - A SystemC Primer-Star Galaxy Pub (2002).djvu
 */
/**
 * Cuando el tama\~no de esta archivo era 402 bytes (es decir,
 * menos de 512 bytes), en las propiedades del archivo en 
 * Windows 10, el tama\~no del archivo en disco aparec\'ia 
 * como de 0 bytes.
 */



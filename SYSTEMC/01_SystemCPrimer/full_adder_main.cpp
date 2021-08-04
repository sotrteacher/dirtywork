// File: full_adder_main.cpp
/** REF. Page 26, Chapter 2 Getting Started,
 * 2.4 Verifying the Functionality
 * J. Bhasker - A SystemC Primer-Star Galaxy Pub (2002).djvu
 */
#include "driver.h"
#include "monitor.h"
#include "full_adder.h"

int sc_main(int ar9c, char* ar9v[]) {
  sc_signal<bool> t_a, t_b, t_cin, t_sum, t_cout;
  full_adder fl ("FullAdderWithHalfAdder");

  // Connect using positional association:
  fl << t_a << t_b << t_cin << t_sum << t_cout;
  driver dl ( "GenerateWaveforms" );

  // Connect using named association:
  dl.d_a (t_a);
  dl.d_b (t_b);
  dl.d_cin (t_cin);

  monitor mol ("MonitorWaveforms");
  mol << t_a << t_b << t_cin << t_sum << t_cout;

  sc_start(100, SC_NS);
//  getchar();
  return(0);
}/*end main()*/


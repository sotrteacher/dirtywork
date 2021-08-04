// File: pulse_main.cpp
// cd bin/03_TestBench
// sudo service dbus start
// gtkwave pulse.vcd
#include "pulse.h"

int sc_main (int argc, char *argv[]) {
  sc_signal<bool> pout;
  sc_trace_file *tf;
  sc_clock clock ("master_clk", 5, SC_NS);

  // Instantiate the pulse module:
  pulse pl ("pulse_pl");
  pl.clk (clock);
  pl.pulse_out (pout);

  // Specify trace file pulse.vcd and trace signals:
  tf = sc_create_vcd_trace_file ("pulse");
  sc_trace (tf, clock, "clock");
  sc_trace (tf, pout, "pulse_out");

  sc_start (100, SC_NS);
  sc_close_vcd_trace_file (tf);
  cout << "Finished at time "<< sc_time_stamp() << endl;

  return 0;
}/*end sc_main()*/


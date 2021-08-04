#ifndef PULSE_H_INCLUDED
#define PULSE_H_INCLUDED
// File: pulse.h
#include <systemc.h>
#define DELAY 2, SC_NS
#define ON_DURATION 1, SC_NS

SC_MODULE (pulse) {
  sc_in<bool> clk;
  sc_out<bool> pulse_out;

  void prc_pulse ();

  SC_CTOR (pulse) {
  SC_THREAD (prc_pulse);
  sensitive_pos << clk;
  }
};
#endif // PULSE_H_INCLUDED

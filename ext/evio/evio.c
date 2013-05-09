#include "evio.h"



void Init_evio()
{
  mEvIO = rb_define_module("EvIO");
  init_loop();
  init_signal();
  init_stream();
  init_file();
}

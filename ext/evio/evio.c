#include "evio.h"



void Init_evio()
{
  mEvIO = rb_define_module("EvIO");
  init_loop();
  init_emitter();
  init_timer();
  init_signal();

  id_call = rb_intern("call");
  /* init_stream(); */
  /* init_file(); */
}

#include <ruby.h>
#include <ev.h>

#include "evio.h"


void Init_evio()
{
  loop = EV_DEFAULT;
  VALUE module = rb_define_module("Evio");
  init_ev(module);
  init_signal(module);
}

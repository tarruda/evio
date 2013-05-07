#include <ruby.h>
#include <ev.h>

#include "evio.h"


void Init_evio()
{
  VALUE module = rb_define_module("Evio");
  init_ev(module);
}

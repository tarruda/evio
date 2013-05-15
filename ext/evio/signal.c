#include "evio.h"

UV_WRAPPER(signal)

static inline void
uv_wrapper_signal_start(uv_signal_t *handle, VALUE argv)
{
  int signum;

  signum = NUM2INT(rb_ary_entry(argv, 0));
  uv_signal_start(handle, signal_handle_cb, signum);
}

void
init_signal()
{
  VALUE cSignal;

  cSignal = rb_define_class_under(mEvIO, "Signal", rb_cObject);
  rb_define_private_method(cSignal, "signal_handle_new", signal_handle_new,
      -2);

  rb_define_private_method(cHandleWrap, "disable_signal"
      , signal_handle_disable, 1);
}

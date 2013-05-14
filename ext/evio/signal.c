#include "evio.h"

static void
free_signal_handle(uv_signal_t *handle)
{
  event_data *data = handle->data;

  UNINSTALL_UV_HANDLE(signal);
}

static VALUE
subscribe_signal(VALUE self, VALUE signum, VALUE event, VALUE block)
{
  VALUE handlers, handler_array, argv;
  uv_signal_t *handle;
  event_data *data;
  int sign;

  CHECK_HANDLERS_OR_RETURN;

  argv = event;
  sign = FIX2INT(signum);
  INSTALL_UV_HANDLE(signal, signal_cb, sign);

  return Data_Wrap_Struct(rb_cObject, 0, free_signal_handle, handle);
}

void
init_signal()
{
  VALUE cSignal;

  cSignal = rb_define_class_under(mEvIO, "Signal", rb_cObject);
  rb_define_private_method(cSignal, "subscribe_signal", subscribe_signal, 3);
}

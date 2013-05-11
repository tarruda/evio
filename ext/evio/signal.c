#include "evio.h"

static void
signal_cb(uv_signal_t *handle, int revents)
{
  block_wrapper *data = handle->data;
  VALUE rv;

  rv = rb_funcall(data->block, rb_intern("call"), 1, INT2FIX(handle->signum));

  if (rv == Qfalse) {
    uv_signal_stop(handle);
    rb_gc_unregister_address(&data->block);
    free(data);
    free(handle);
  }
}

static VALUE
on_signal(VALUE self, VALUE signum)
{
  uv_signal_t *handle;
  block_wrapper *data;
  int sign;

  SECURE_CHECK;

  if (TYPE(signum) != T_FIXNUM)
    rb_raise(rb_eArgError, "signal must be an integer");

  sign = FIX2INT(signum);
  INSTALL_HANDLE(signal, block_wrapper, sign);

  return Qnil;
}

void
init_signal()
{
  rb_define_singleton_method(mEvIO, "on_signal", on_signal, 1);
}

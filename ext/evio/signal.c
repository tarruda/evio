#include "evio.h"

static void
signal_cb(struct ev_loop *loop, ev_signal *watcher, int revents)
{
  block_wrapper *data = watcher->data;
  VALUE rv;

  rv = rb_funcall(data->block, rb_intern("call"), 1, INT2FIX(watcher->signum));

  if (rv == Qfalse) {
    ev_signal_stop(loop, watcher);
    rb_gc_unregister_address(&data->block);
    free(data);
    free(watcher);
  }
}

static VALUE
on_signal(VALUE self, VALUE signum)
{
  ev_signal *watcher;
  block_wrapper *data;
  int sign;

  SECURE_CHECK;

  if (TYPE(signum) != T_FIXNUM)
    rb_raise(rb_eArgError, "signal must be an integer");

  sign = FIX2INT(signum);
  watcher = ALLOC(ev_signal);
  data = ALLOC(block_wrapper);
  data->block = rb_block_proc();
  watcher->data = data;
  rb_gc_register_address(&data->block);
  ev_signal_init(watcher, signal_cb, sign);
  ev_signal_start(loop, watcher);

  return Qnil;
}

void
init_signal()
{
  rb_define_singleton_method(mEvIO, "on_signal", on_signal, 1);
}

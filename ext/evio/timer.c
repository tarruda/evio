#include "evio.h"

static void
idle_cb(uv_idle_t *handle, int revents)
{
  block_wrapper *data = handle->data;

  rb_funcall(data->block, rb_intern("call"), 0);
  UNINSTALL_HANDLE(idle);
}

static void
timer_cb(uv_timer_t *handle, int revents)
{
  block_wrapper *data = handle->data;
  VALUE rv;

  rv = rb_funcall(data->block, rb_intern("call"), 0);

  if (handle->repeat == 0. || rv == Qfalse) {
    UNINSTALL_HANDLE(timer);
  }
}

static VALUE
on_tick(VALUE self)
{
  uv_idle_t *handle;
  block_wrapper *data;

  if (!rb_block_given_p())
    rb_raise(rb_eArgError, "a block is required");

  INSTALL_HANDLE(idle, block_wrapper);

  return Qnil;
}

static VALUE
on_timeout(VALUE self, VALUE delay)
{
  uv_timer_t *handle;
  block_wrapper *data;
  int type;
  double dl;

  SECURE_CHECK;

  if ((type = TYPE(delay)) == T_FLOAT || type == T_FIXNUM)
    dl = NUM2DBL(delay);
  else
    rb_raise(rb_eArgError, "delay argument must be a number");

  if (!rb_block_given_p())
    rb_raise(rb_eArgError, "a block is required");

  INSTALL_TIMER(dl, 0.);

  return Qnil;
}

static VALUE
on_interval(int argc, VALUE *argv)
{
  uv_timer_t *handle;
  block_wrapper *data;
  int type;
  double repeat, dl;

  SECURE_CHECK;

  if (argc < 1 || argc > 2)
    rb_raise(rb_eArgError, "wrong number of arguments (%d for 1..2)", argc);

  if ((type = TYPE(argv[0])) == T_FLOAT || type == T_FIXNUM)
    repeat = NUM2DBL(argv[0]);
  else
    rb_raise(rb_eArgError, "interval must be a number");

  if (repeat <= 0)
    rb_raise(rb_eArgError, "interval must be positive");

  if (argc == 2) {
    if ((type = TYPE(argv[1])) == T_FLOAT || type == T_FIXNUM)
      dl = NUM2DBL(argv[1]);
    else
      rb_raise(rb_eArgError, "delay must be a number");
  } else {
    dl = repeat;
  }

  if (!rb_block_given_p())
    rb_raise(rb_eArgError, "a block is required");

  INSTALL_TIMER(dl, repeat);

  return Qnil;
}

void
init_timer()
{
  rb_define_singleton_method(mEvIO, "on_tick", on_tick, 0);
  rb_define_singleton_method(mEvIO, "on_timeout", on_timeout, 1);
  rb_define_singleton_method(mEvIO, "on_interval", on_interval, -1);
}

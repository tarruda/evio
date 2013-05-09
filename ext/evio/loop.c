#include "evio.h"


static void
timer_cb(struct ev_loop *loop, ev_timer *watcher, int revents)
{
  block_wrapper *data = watcher->data;

  VALUE rv;

  rv = rb_funcall(data->block, rb_intern("call"), 0);

  if (watcher->repeat == 0. || rv == Qfalse) {
    // remove watcher from loop
    ev_timer_stop(loop, watcher);
    // make the block available for gc collect
    rb_gc_unregister_address(&data->block);
    // free resources unmanaged by the gc
    free(data);
    free(watcher);
  }
}

static VALUE
on_tick(VALUE self)
{
  ev_timer *watcher;
  block_wrapper *data;

  if (!rb_block_given_p())
    rb_raise(rb_eArgError, "a block is required");

  INSTALL_TIMER(0., 0.);

  return Qnil;
}

static VALUE
on_timeout(VALUE self, VALUE delay)
{
  ev_timer *watcher;
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
  ev_timer *watcher;
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

static void
prepare_cb(struct ev_loop *loop, ev_prepare *watcher, int revents)
{
  block_wrapper *data = watcher->data;

  rb_funcall(data->block, rb_intern("call"), 0);
  ev_prepare_stop(loop, watcher);
  rb_gc_unregister_address(&data->block);
  free(data);
  free(watcher);
}

static VALUE
start()
{
  ev_prepare *watcher;
  block_wrapper *data;

  if (rb_block_given_p()) {
    INSTALL_WATCHER(prepare, block_wrapper);
  }

  ev_run(loop, 0);

  return Qnil;
}

void init_loop()
{
  loop = EV_DEFAULT;
  rb_define_singleton_method(mEvIO, "on_tick", on_tick, 0);
  rb_define_singleton_method(mEvIO, "on_timeout", on_timeout, 1);
  rb_define_singleton_method(mEvIO, "on_interval", on_interval, -1);
  rb_define_singleton_method(mEvIO, "start", start, 0);
}

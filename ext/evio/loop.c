#include "evio.h"


static void
prepare_cb(struct ev_loop *loop, ev_prepare *watcher, int revents)
{
  prepare_data *data = watcher->data;

  rb_funcall(data->block, rb_intern("call"), 0);
  ev_prepare_stop(loop, watcher);
  rb_gc_unregister_address(&data->block);
  free(data);
  free(watcher);
}

static void
timeout_cb(struct ev_loop *loop, ev_timer *watcher, int revents)
{
  timeout_data *data = watcher->data;
  VALUE rv;

  rv = rb_funcall(data->block, rb_intern("call"), 0);

  if (data->repeat == 0 || rv == Qfalse) {
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
on_timeout(VALUE self, VALUE delay, VALUE repeat_delay)
{
  ev_timer *watcher;
  timeout_data *data;
  int type;
  double dl, repeat_dl;

  rb_secure(2);

  if ((type = TYPE(delay)) == T_FLOAT || type == T_FIXNUM)
    dl = NUM2DBL(delay);
  else
    rb_raise(rb_eArgError, "first argument must be a number");

  if ((type = TYPE(repeat_delay)) == T_FLOAT || type == T_FIXNUM)
    repeat_dl = NUM2DBL(repeat_delay);
  else
    rb_raise(rb_eArgError, "second argument must be a number");

  if (!rb_block_given_p())
    rb_raise(rb_eArgError, "a block is required");

  watcher = ALLOC(ev_timer);
  data = ALLOC(timeout_data);
  // bind the block to its outer scope and store for later use
  data->block = rb_block_proc();
  // don't let the gc collect it
  rb_gc_register_address(&data->block);
  watcher->data = data;
  if (repeat_dl > 0) data->repeat = 1;
  else data->repeat = 0;
  ev_timer_init(watcher, timeout_cb, dl, repeat_dl);
  ev_timer_start(loop, watcher);

  return Qnil;
}

static VALUE
start_looping()
{
  ev_prepare *watcher;
  prepare_data *data;

  if (rb_block_given_p()) {
    watcher = ALLOC(ev_prepare);
    data = ALLOC(prepare_data);
    data->block = rb_block_proc();
    rb_gc_register_address(&data->block);
    watcher->data = data;
    ev_prepare_init(watcher, prepare_cb);
    ev_prepare_start(loop, watcher);
  }

  ev_run(loop, 0);

  return Qnil;
}

void init_loop()
{
  loop = EV_DEFAULT;
  rb_define_singleton_method(mEvIO, "on_timeout", on_timeout, 2);
  rb_define_singleton_method(mEvIO, "start_looping", start_looping, 0);
}

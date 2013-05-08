#include <ruby.h>
#include <ev.h>

#include "evio.h"


struct ev_loop *loop;

static void
timer_cb (EV_P_ ev_timer *watcher, int revents)
{
  timer_data *data = watcher->data;
  VALUE rv;

  rv = rb_funcall(data->bound_block, rb_intern("call"), 0);

  if (data->repeat == 0 || rv == Qfalse) {
    // remove watcher from loop and free resources
    ev_timer_stop(loop, watcher);
    free(data);
    free(watcher);
  }
}

static VALUE
set_timer(VALUE self, VALUE delay, VALUE repeat_delay)
{
  ev_timer *rv;
  timer_data *data;
  int type;
  double dl, repeat_dl;

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

  rv = ALLOC(ev_timer);
  data = ALLOC(timer_data);
  // bind the block to its outer scope and store for later use
  data->bound_block = rb_block_proc();
  rv->data = data;
  if (repeat_dl > 0) data->repeat = 1;
  else data->repeat = 0;
  ev_timer_init(rv, timer_cb, dl, repeat_dl);
  ev_timer_start(loop, rv);
  return Qnil;
}

static VALUE
start_loop(VALUE self)
{
  ev_run(loop, 0);
  return Qnil;
}

static void
stop_loop(VALUE self)
{
  ev_break(loop, EVBREAK_ALL);
}

void init_ev(VALUE module)
{
  loop = EV_DEFAULT;
  rb_define_module_function(module, "set_timer", set_timer, 2);
  rb_define_module_function(module, "start_loop", start_loop, 0);
  rb_define_module_function(module, "stop_loop", stop_loop, 0);
}

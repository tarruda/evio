#include <ruby.h>
#include <ev.h>

#include "evio.h"


struct ev_loop *loop;

static void
timeout_cb (EV_P_ ev_timer *watcher, int revents)
{
  event_data *data = watcher->data;

  rb_funcall(data->bound_block, rb_intern("call"), 0);
}

static void
set_timeout(VALUE self, VALUE delay, VALUE repeat_delay)
{
  ev_timer *rv;
  event_data *data;
  int type, dl;
  double repeat_dl;

  if (TYPE(delay) == T_FIXNUM)
    dl = FIX2INT(delay);
  else
    rb_raise(rb_eArgError, "first argument must be an integer");

  if ((type = TYPE(repeat_delay)) == T_FLOAT || type == T_FIXNUM)
    repeat_dl = NUM2DBL(repeat_delay);
  else
    rb_raise(rb_eArgError, "second argument must be a float or integer");

  if (!rb_block_given_p()) {
    rb_raise(rb_eArgError, "a block is required");
  }

  rv = ALLOC(ev_timer);
  data = ALLOC(event_data);
  // bind the block to its outer scope and store for later use
  data->bound_block = rb_block_proc();
  rv->data = data;
  ev_timer_init(rv, timeout_cb, dl, repeat_dl);
  ev_timer_start(loop, rv);
}

static void
start_loop(VALUE self)
{
  ev_run(loop, 0);
}

static void
stop_loop(VALUE self)
{
  ev_break(loop, EVBREAK_ALL);
}

void init_ev(VALUE module)
{
  loop = EV_DEFAULT;
  rb_define_module_function(module, "set_timeout", set_timeout, 2);
  rb_define_module_function(module, "start_loop", start_loop, 0);
  rb_define_module_function(module, "stop_loop", stop_loop, 0);
}

#include "evio.h"

VALUE tick_sym, timeout_sym, interval_sym;

static VALUE
emit_tick(VALUE self, VALUE handler_array)
{
  VALUE event, argv;
  uv_idle_t *handle;
  event_data *data;

  event = tick_sym;
  argv = tick_sym;
  INSTALL_UV_HANDLE(idle, idle_cb);

  return Qnil;
}

static VALUE
emit_timeout(VALUE self, VALUE handler_array, VALUE delay)
{
  VALUE event, argv;
  uv_timer_t *handle;
  event_data *data;
  double dl;
 
  dl = NUM2DBL(delay);
  event = timeout_sym;
  argv = timeout_sym;
  INSTALL_UV_HANDLE(timer, timer_cb, dl * 1000, 0.);

  return Qnil;
}

static VALUE
emit_interval(VALUE self, VALUE handler_array, VALUE interval, VALUE delay)
{
  VALUE event, argv;
  uv_timer_t *handle;
  event_data *data;
  double dl, iv;

  iv = NUM2DBL(interval);
  dl = NUM2DBL(delay);
  event = interval_sym;
  argv = interval_sym;
  INSTALL_UV_HANDLE(timer, timer_cb, dl * 1000, iv * 1000);

  return Qnil;
}


void
init_timer()
{
  VALUE cTimer;

  cTimer = rb_define_class_under(mEvIO, "Timer", rb_cObject);
  rb_define_private_method(cTimer, "emit_tick", emit_tick, 1);
  rb_define_private_method(cTimer, "emit_timeout", emit_timeout, 2);
  rb_define_private_method(cTimer, "emit_interval", emit_interval, 3);

  tick_sym = ID2SYM(rb_intern("tick"));
  timeout_sym = ID2SYM(rb_intern("timeout"));
  interval_sym = ID2SYM(rb_intern("interval"));
}

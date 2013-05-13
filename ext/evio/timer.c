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
subscribe_tick(VALUE block)
{
  uv_idle_t *handle;
  block_wrapper *data;

  INSTALL_HANDLE2(idle, block_wrapper, block);

  return Qnil;
}

/* static VALUE */
/* emit_timeout(VALUE self, VALUE argv) */
/* { */
/*   VALUE block, delay; */
/*   uv_timer_t *handle; */
/*   event_data *data; */
/*   int type; */
/*   double dl; */

/*   block = rb_ary_shift(argv); */
/*   delay = rb_ary_shift(argv); */

/*   if ((type = TYPE(delay)) == T_FLOAT || type == T_FIXNUM) */
/*     dl = NUM2DBL(delay); */
/*   else */
/*     rb_raise(rb_eArgError, "delay argument must be a number"); */

/*   data = ALLOC(event_data); */
/*   data->emitter = self; */
/*   data->event = block; // block will identify the correct handler */
/*   data->argv = argv; */
/*   rb_gc_register_address(&data->argv); */
/*   rb_gc_register_address(&data->emitter); */
/*   rb_gc_register_address(&data->event); */
/*   handle = ALLOC(uv_timer_t); */
/*   handle->data = data; */
/*   uv_timer_init(uv_default_loop(), handle); */
/*   uv_timer_start(handle, (void (*)(uv_idle_t *, int))default_cb, dl, 0.); */

/*   /1* INSTALL_TIMER(dl, 0.); *1/ */

/*   return Qnil; */
/* } */

static VALUE
subscribe_interval(int argc, VALUE *argv)
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
  VALUE cTimer, rb_mSingleton, timer_instance;

  /* rb_require("singleton"); */
  /* rb_mSingleton = rb_eval_string("Singleton"); */

  cTimer = rb_define_class_under(mEvIO, "Timer", rb_cObject);
  /* rb_define_private_method(cTimer, "emit_timeout", emit_timeout, -2); */

  /* rb_define_singleton_method(mEvIO, "on_tick", on_tick, 0); */
  /* rb_define_singleton_method(mEvIO, "on_timeout", on_timeout, 1); */
  /* rb_define_singleton_method(mEvIO, "on_interval", on_interval, -1); */
}

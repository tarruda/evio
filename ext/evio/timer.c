#include "evio.h"

UV_WRAPPER(timer)

static inline void
uv_wrapper_timer_start(uv_timer_t *handle, VALUE argv)
{
  double delay, interval;

  delay = NUM2DBL(rb_ary_entry(argv, 0));
  interval = NUM2DBL(rb_ary_entry(argv, 1));

  uv_timer_start(handle, timer_handle_cb, delay * 1000, interval * 1000);
}

void
init_timer()
{
  VALUE cTimer;

  cTimer = rb_define_class_under(mEvIO, "Timer", rb_cObject);
  rb_define_private_method(cTimer, "timer_handle_new", timer_handle_new, -2);

  rb_define_private_method(cHandleWrap, "disable_timer"
      , timer_handle_disable, 1);
  rb_define_private_method(cHandleWrap, "enable_timer", timer_handle_enable,
      1);

}

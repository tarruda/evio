#include "evio.h"

UV_WRAPPER(idle)

static inline void
uv_wrapper_idle_start(uv_idle_t *handle, VALUE argv)
{
  uv_idle_start(handle, idle_handle_cb);
}


void init_emitter()
{
  mEmitter = rb_define_module_under(mEvIO, "Emitter");
  rb_define_private_method(mEmitter, "idle_handle_new", idle_handle_new, -2);

  cHandleWrap = rb_define_class_under(mEvIO, "HandleWrap", rb_cObject);
  rb_define_private_method(cHandleWrap, "disable_idle"
      , idle_handle_disable, 1);
}

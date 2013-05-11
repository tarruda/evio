#include "evio.h"


static void
idle_cb(uv_idle_t *handle, int status)
{
  block_wrapper *data = handle->data;

  rb_funcall(data->block, rb_intern("call"), 0);
  uv_idle_stop(handle);
  rb_gc_unregister_address(&data->block);
  free(data);
  free(handle);
}

static VALUE
start()
{
  uv_idle_t *handle;
  block_wrapper *data;

  if (rb_block_given_p()) {
    INSTALL_HANDLE(idle, block_wrapper);
  }

  uv_run(uv_default_loop(), 0);

  return Qnil;
}

void
init_loop()
{
  rb_define_singleton_method(mEvIO, "start", start, 0);
}

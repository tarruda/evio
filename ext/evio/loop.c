#include "evio.h"

VALUE start_sym;

static void
start_cb(uv_idle_t *handle, int status)
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
    data = ALLOC(block_wrapper);
    data->block = rb_block_proc();
    rb_gc_register_address(&data->block);
    handle = ALLOC(uv_idle_t);
    handle->data = data;
    uv_idle_init(uv_default_loop(), handle);
    uv_idle_start(handle, start_cb);
  }

  uv_run(uv_default_loop(), 0);

  return Qnil;
}

void
init_loop()
{
  rb_define_singleton_method(mEvIO, "start", start, 0);

  start_sym = ID2SYM(rb_intern("start"));
}

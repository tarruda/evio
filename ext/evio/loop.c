#include "evio.h"

VALUE start_sym;

static void
start_cb(uv_idle_t *handle, int status)
{
  event_data *data = handle->data;

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
  event_data *data;

  if (rb_block_given_p()) {
    data = ALLOC(event_data);
    data->block = rb_block_proc();
    rb_gc_register_address(&data->block);
    handle = ALLOC(uv_idle_t);
    handle->data = data;
    uv_idle_init(event_loop, handle);
    uv_idle_start(handle, start_cb);
  }

  uv_run(event_loop, UV_RUN_DEFAULT);

  return Qnil;
}

void
init_loop()
{
  rb_define_singleton_method(mEvIO, "start", start, 0);

  start_sym = ID2SYM(rb_intern("start"));
  event_loop = uv_default_loop();
}

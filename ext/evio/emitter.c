#include "evio.h"


void
default_cb(uv_handle_t *handle, int status)
{
  event_data *data = handle->data;

  rb_funcall(data->emitter, rb_intern("process_event"), 2, data->event,
      data->argv);
  rb_gc_unregister_address(&data->argv);
  rb_gc_unregister_address(&data->emitter);
  rb_gc_unregister_address(&data->event);
  uv_idle_stop((uv_idle_t *)handle);
  free(handle);
  free(data);
}

static VALUE
emit(VALUE self, VALUE argv)
{
  VALUE event, can_emit, handlers, handler_array;
  uv_idle_t *handle;
  event_data *data;

  event = rb_ary_shift(argv);

  handlers = rb_iv_get(self, "@handlers");
  if (handlers == Qnil)
    return Qfalse;

  handler_array = rb_hash_aref(handlers, event);
  if (handler_array == Qnil || RARRAY_LEN(handler_array) == 0)
    return Qfalse;

  INSTALL_UV_HANDLE(idle, default_cb);

  return Qtrue;
}

void init_emitter()
{
  mEmitter = rb_define_module_under(mEvIO, "Emitter");
  rb_define_protected_method(mEmitter, "emit", emit, -2);
}

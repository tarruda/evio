#include "evio.h"


static VALUE
emit(VALUE self, VALUE argv)
{
  VALUE event, handlers, handler_array;
  uv_idle_t *handle;
  event_data *data;

  event = rb_ary_shift(argv);

  handlers = rb_iv_get(self, "@handlers");
  if (handlers == Qnil)
    return Qfalse;

  handler_array = rb_hash_aref(handlers, event);
  if (handler_array == Qnil || RARRAY_LEN(handler_array) == 0)
    return Qfalse;

  INSTALL_UV_HANDLE(idle, idle_cb);

  return Qtrue;
}

void init_emitter()
{
  mEmitter = rb_define_module_under(mEvIO, "Emitter");
  rb_define_protected_method(mEmitter, "emit", emit, -2);

  stop_sym = ID2SYM(rb_intern("stop"));
}

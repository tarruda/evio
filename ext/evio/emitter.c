#include "evio.h"


ID id_subscribe, id_handlers, id_stop, id_call;
VALUE stop_sym;
VALUE cHandler;

static void
handler_free(event_handler *handler)
{
  rb_gc_unregister_address(&handler->block);
  free(handler);
}

static VALUE
handler_stop(VALUE handler_wrapped)
{
  event_handler *handler;

  Data_Get_Struct(handler_wrapped, event_handler, handler);
  rb_ary_delete(handler->handler_list, handler_wrapped);

  return Qnil;
}

static void
idle_cb(uv_idle_t *handle, int status)
{
  VALUE *handler_list, rv;
  event_handler *handler;
  int i, len;
  event_data *data = handle->data;

  len = RARRAY_LEN(data->handler_list);
  handler_list = RARRAY_PTR(data->handler_list);

  i = 0;
  while (i < len) {
    Data_Get_Struct(handler_list[i], event_handler, handler);
    rv = rb_apply(handler->block, id_call, data->argv);
    if (rv == stop_sym) rb_ary_delete_at(data->handler_list, i);
    else i++;
  }

  rb_gc_unregister_address(&data->handler_list);
  rb_gc_unregister_address(&data->argv);
  uv_idle_stop(handle);
  free(handle);
  free(data);
}

static VALUE
subscribe(int argc, VALUE *argv, VALUE emitter)
{
  VALUE handlers, list, event;
  event_handler *handler;

  handler = ALLOC(event_handler);
  handler->block = argv[0];
  rb_gc_register_address(&handler->block);
  event = argv[1];

  // get the hash associating each event symbol with a handler array
  handlers = rb_ivar_get(emitter, id_handlers);
  if (handlers == Qnil) {
    handlers = rb_hash_new();
    rb_ivar_set(emitter, id_handlers, handlers);
  }

  // get the list of handler instances for this event
  list = rb_hash_aref(handlers, event);
  if (list == Qnil) {
    list = rb_ary_new();
    rb_hash_aset(handlers, event, list);
  }

  // store the handler
  handler->handler_list = list;
  rb_ary_push(list, Data_Wrap_Struct(cHandler, 0, handler_free, handler));

  return Qnil;
}

static VALUE
emit(VALUE emitter, VALUE argv)
{
  VALUE event, handlers, list;
  uv_idle_t *handle;
  event_data *data;

  event = rb_ary_shift(argv);

  handlers = rb_ivar_get(emitter, id_handlers);
  if (handlers == Qnil)
    return Qfalse;

  list = rb_hash_aref(handlers, event);
  if (list == Qnil || RARRAY_LEN(list) == 0)
    return Qfalse;

  data = ALLOC(event_data);
  data->handler_list = list;
  rb_gc_register_address(&data->handler_list);
  data->argv = argv;
  rb_gc_register_address(&data->argv);
  handle = ALLOC(uv_idle_t);
  handle->data = data;
  uv_idle_init(uv_default_loop(), handle);
  uv_idle_start(handle, idle_cb);

  return Qtrue;
}

static VALUE
on(VALUE emitter, VALUE argv)
{
  VALUE block;
  int argc;

  argc = RARRAY_LEN(argv);

  if (argc < 1)
    rb_raise(rb_eArgError, "an event is required");

  if (!rb_block_given_p())
    rb_raise(rb_eArgError, "a block is required");

  block = rb_block_proc();
  rb_ary_unshift(argv, block);
  rb_apply(emitter, id_subscribe, argv);
}

void init_emitter()
{
  cHandler = rb_define_class_under(mEvIO, "Handler", rb_cObject);
  rb_define_method(cHandler, "stop", handler_stop, 0);

  mEmitter = rb_define_module_under(mEvIO, "Emitter");
  rb_define_method(mEmitter, "on", on, -2);
  rb_define_protected_method(mEmitter, "emit", emit, -2);
  rb_define_protected_method(mEmitter, "subscribe", subscribe, -1);

  id_subscribe = rb_intern("subscribe");
  id_handlers = rb_intern("handlers");
  id_stop = rb_intern("stop");
  id_call = rb_intern("call");
  stop_sym = ID2SYM(id_stop);
}

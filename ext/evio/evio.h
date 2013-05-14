/* #include <errno.h> */
/* #include <string.h> */

/* #include <sys/types.h> */
/* #include <sys/stat.h> */
/* #include <fcntl.h> */
/* #include <unistd.h> */

#include <ruby.h>
#include <uv.h>

typedef struct event_data {
  VALUE argv;
  VALUE handler_array;
  VALUE event;
  VALUE emitter;
} event_data;

/* typedef struct { */
/*   int fd; */
/*   int closed; */
/*   enum { */
/*     file, */
/*     socket */
/*   } stream_type; */

/* } io_stream; */

typedef struct {
  VALUE block;
} block_wrapper;

typedef struct {
  VALUE block;
  int buffer_size;
  char *buffer;
} io_read_data;

VALUE mEvIO;
VALUE cStream;
VALUE cFile;
VALUE mEmitter;
VALUE stop_sym;

void stream_free();

void Init_evio();
void init_loop();
void init_emitter();
void init_timer();
void init_signal();
/* void init_stream(); */
/* void init_file(); */

#define SECURE_CHECK rb_secure(2)

#define INSTALL_UV_HANDLE_GEN(h, ht, d, dt, em, ha, ev, av, cb, ...) \
  d = ALLOC(dt); \
  d->handler_array = ha; \
  d->emitter = em; \
  d->event = ev; \
  d->argv = av; \
  rb_gc_register_address(&d->argv); \
  rb_gc_register_address(&d->emitter); \
  rb_gc_register_address(&d->event); \
  rb_gc_register_address(&d->handler_array); \
  h = ALLOC(uv_##ht##_t); \
  h->data = d; \
  uv_##ht##_init(uv_default_loop(), h); \
  uv_##ht##_start(h, (void (*)(uv_##ht##_t *, int))cb, ##__VA_ARGS__)

#define INSTALL_UV_HANDLE(handle_type, cb, ...) \
  INSTALL_UV_HANDLE_GEN(handle, handle_type, data, event_data, \
      self, handler_array, event, argv, cb, ##__VA_ARGS__)

#define UV_CALLBACK(ht) \
  void \
  ht##_cb(uv_##ht##_t *handle, int status) \
  { \
    event_data *data = handle->data; \
    if (rb_funcall(data->emitter, rb_intern("process_event"), 3, \
          data->handler_array, data->event, data->argv) == stop_sym) { \
      UNINSTALL_UV_HANDLE(ht); \
    } \
  }

#define UV_CALLBACK_PROTO(ht) \
  void ht##_cb(uv_##ht##_t *, int)

#define UNINSTALL_UV_HANDLE_GEN(h, ht, d) \
  rb_gc_unregister_address(&d->argv); \
  rb_gc_unregister_address(&d->emitter); \
  rb_gc_unregister_address(&d->event); \
  rb_gc_unregister_address(&d->handler_array); \
  uv_##ht##_stop((uv_##ht##_t *)h); \
  free(h); \
  free(d)

#define UNINSTALL_UV_HANDLE(ht) \
  UNINSTALL_UV_HANDLE_GEN(handle, ht, data)

#define CHECK_HANDLERS_OR_RETURN \
  handlers = rb_iv_get(self, "@handlers"); \
  if (handlers == Qnil) \
    return Qfalse; \
  handler_array = rb_hash_aref(handlers, event); \
  if (handler_array == Qnil || RARRAY_LEN(handler_array) == 0) \
    return Qfalse

UV_CALLBACK_PROTO(idle);
UV_CALLBACK_PROTO(timer);
UV_CALLBACK_PROTO(signal);

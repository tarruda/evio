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

void default_cb();
void stream_free();

void Init_evio();
void init_loop();
void init_emitter();
void init_timer();
void init_signal();
/* void init_stream(); */
/* void init_file(); */

#define SECURE_CHECK rb_secure(2)

#define INSTALL_UV_HANDLE_GEN(handle, handle_type, d, dt, \
    em, ha, ev, av, cb, ...) \
  d = ALLOC(dt); \
  d->handler_array = ha; \
  d->emitter = em; \
  d->event = ev; \
  d->argv = av; \
  rb_gc_register_address(&d->argv); \
  rb_gc_register_address(&d->emitter); \
  rb_gc_register_address(&d->event); \
  rb_gc_register_address(&d->handler_array); \
  handle = ALLOC(uv_##handle_type##_t); \
  handle->data = d; \
  uv_idle_init(uv_default_loop(), handle); \
  uv_idle_start(handle, (void (*)(uv_##handle_type##_t *, int))cb, \
    ##__VA_ARGS__)

#define INSTALL_UV_HANDLE(handle_type, cb, ...) \
  INSTALL_UV_HANDLE_GEN(handle, handle_type, data, event_data, \
      self, handler_array, event, argv, cb, ##__VA_ARGS__)

#define INSTALL_HANDLE_BLOCK(handle, data, handle_type, data_type, \
    block_expr, cb, ...) \
  handle = ALLOC(uv_##handle_type##_t); \
  data = ALLOC(data_type); \
  data->block = block_expr; \
  rb_gc_register_address(&data->block); \
  handle->data = data; \
  uv_##handle_type##_init(uv_default_loop(), handle); \
  uv_##handle_type##_start(handle, cb, ##__VA_ARGS__)

#define INSTALL_HANDLE(handle_type, data_type, ...) \
  INSTALL_HANDLE_BLOCK(handle, data, handle_type, data_type, rb_block_proc(), \
      handle_type##_cb, ##__VA_ARGS__)

#define INSTALL_HANDLE2(handle_type, data_type, block_expr, ...) \
  INSTALL_HANDLE_BLOCK(handle, data, handle_type, data_type, block_expr, \
      handle_type##_cb, ##__VA_ARGS__)

#define INSTALL_TIMER(delay, repeat) \
  INSTALL_HANDLE_BLOCK(handle, data, timer, block_wrapper,\
      rb_block_proc(), timer_cb, delay * 1000, repeat * 1000);

#define UNINSTALL_HANDLE(handle_type) \
  uv_##handle_type##_stop(handle); \
  rb_gc_unregister_address(&data->block); \
  free(data); \
  free(handle)

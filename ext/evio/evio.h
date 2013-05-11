/* #include <errno.h> */
/* #include <string.h> */

/* #include <sys/types.h> */
/* #include <sys/stat.h> */
/* #include <fcntl.h> */
/* #include <unistd.h> */

#include <ruby.h>
#include <uv.h>

typedef struct event_handler {
  VALUE block;
  VALUE handler_list;
} event_handler;

typedef struct event_data {
  VALUE argv;
  VALUE handler_list;
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

/* struct uv_loop_t *loop; */
VALUE mEvIO;
VALUE cStream;
VALUE cFile;
VALUE mEmitter;

void stream_free();

void Init_evio();
void init_loop();
void init_emitter();
void init_timer();
void init_signal();
/* void init_stream(); */
/* void init_file(); */

#define SECURE_CHECK rb_secure(2)

#define INSTALL_HANDLE_BLOCK(handle, data, handle_type, data_type, \
    cb, ...) \
  handle = ALLOC(uv_##handle_type##_t); \
  data = ALLOC(data_type); \
  data->block = rb_block_proc(); \
  rb_gc_register_address(&data->block); \
  handle->data = data; \
  uv_##handle_type##_init(uv_default_loop(), handle); \
  uv_##handle_type##_start(handle, cb, ##__VA_ARGS__)

#define INSTALL_HANDLE(handle_type, data_type, ...) \
  INSTALL_HANDLE_BLOCK(handle, data, handle_type, data_type,\
      handle_type##_cb, ##__VA_ARGS__)

#define INSTALL_TIMER(delay, repeat) \
  INSTALL_HANDLE_BLOCK(handle, data, timer, block_wrapper,\
      timer_cb, delay * 1000, repeat * 1000);

#define UNINSTALL_HANDLE(handle_type) \
  uv_##handle_type##_stop(handle); \
  rb_gc_unregister_address(&data->block); \
  free(data); \
  free(handle)

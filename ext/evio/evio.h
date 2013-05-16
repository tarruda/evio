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
  VALUE block;
  void *extra;
} event_data;

typedef struct {
  int fd;
  int closed;
  int writable;
  int readable;
} io_stream;

typedef struct {
  VALUE block;
  int buffer_size;
  char *buffer;
} io_read_data;

uv_loop_t *event_loop;

VALUE mEvIO;
VALUE cHandleWrap;
VALUE cStream;
VALUE cFile;
VALUE mEmitter;
VALUE id_call;
VALUE id_new;

void evio_close_stream(uv_pipe_t *);

void Init_evio();
void init_loop();
void init_emitter();
void init_timer();
void init_signal();
void init_stream();
void init_file();

#define SECURE_CHECK rb_secure(2)

#define UV_WRAPPER(ht)                                                \
  static inline void uv_wrapper_##ht##_start(uv_##ht##_t *, VALUE);   \
  static void                                                         \
  ht##_handle_cb(uv_##ht##_t *handle, int status)                     \
  {                                                                   \
    event_data *data = handle->data;                                  \
    rb_apply(data->block, id_call, data->argv);                       \
  }                                                                   \
  static VALUE                                                        \
  ht##_handle_disable(VALUE self, VALUE wrapped)                      \
  {                                                                   \
    uv_##ht##_t *handle;                                              \
    Data_Get_Struct(wrapped, uv_##ht##_t, handle);                    \
    uv_##ht##_stop(handle);                                           \
    return Qnil;                                                      \
  }                                                                   \
  static void                                                         \
  ht##_handle_free(uv_##ht##_t * handle)                              \
  {                                                                   \
    event_data *data = handle->data;                                  \
    rb_gc_unregister_address(&data->block);                           \
    rb_gc_unregister_address(&data->argv);                            \
    uv_##ht##_stop(handle);                                           \
    free(data);                                                       \
    free(handle);                                                     \
  }                                                                   \
  static VALUE                                                        \
  ht##_handle_enable(VALUE self, VALUE wrapped)                       \
  {                                                                   \
    uv_##ht##_t *handle;                                              \
    event_data *data;                                                 \
    Data_Get_Struct(wrapped, uv_##ht##_t, handle);                    \
    data = handle->data;                                              \
    uv_wrapper_##ht##_start(handle, data->argv);                      \
    return Qnil;                                                      \
  }                                                                   \
  static VALUE                                                        \
  ht##_handle_new(VALUE self, VALUE argv)                             \
  {                                                                   \
    uv_##ht##_t *handle;                                              \
    event_data *data;                                                 \
    data = ALLOC(event_data);                                         \
    data->block = rb_block_proc();                                    \
    data->argv = argv;                                                \
    rb_gc_register_address(&data->block);                             \
    rb_gc_register_address(&data->argv);                              \
    handle = ALLOC(uv_##ht##_t);                                      \
    handle->data = data;                                              \
    uv_##ht##_init(event_loop, handle);                        \
    return Data_Wrap_Struct(rb_cObject, 0, ht##_handle_free, handle); \
  }


#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <ruby.h>
#include <ev.h>

typedef struct event_handler {
  VALUE block;
  VALUE handler_list;
} event_handler;

typedef struct event_data {
  VALUE argv;
  VALUE handler_list;
} event_data;

typedef struct {
  int fd;
  int closed;
  enum {
    file,
    socket
  } stream_type;

} io_stream;

typedef struct {
  VALUE block;
} block_wrapper;

typedef struct {
  VALUE block;
  int buffer_size;
  char *buffer;
} io_read_data;

struct ev_loop *loop;
VALUE mEvIO;
VALUE cStream;
VALUE cFile;
VALUE mEmitter;

void stream_free();

void Init_evio();
void init_loop();
void init_emitter();
void init_timer();
/* void init_signal(); */
/* void init_stream(); */
/* void init_file(); */

#define SECURE_CHECK rb_secure(2)

#define INSTALL_WATCHER_BLOCK(watcher, data, watcher_type, data_type, \
    cb, ...) \
  watcher = ALLOC(ev_##watcher_type); \
  data = ALLOC(data_type); \
  data->block = rb_block_proc(); \
  rb_gc_register_address(&data->block); \
  watcher->data = data; \
  ev_##watcher_type##_init(watcher, cb, ##__VA_ARGS__); \
  ev_##watcher_type##_start(loop, watcher)

#define INSTALL_WATCHER(watcher_type, data_type) \
  INSTALL_WATCHER_BLOCK(watcher, data, watcher_type, data_type,\
      watcher_type##_cb)

#define INSTALL_TIMER(delay, repeat) \
  INSTALL_WATCHER_BLOCK(watcher, data, timer, block_wrapper,\
      timer_cb, delay, repeat);


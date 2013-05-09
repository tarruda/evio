#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <ruby.h>
#include <ev.h>

typedef struct {
  int fd;
  int closed;
  enum {
    file,
    socket
  } stream_type;

} io_stream;

struct block_wrapper {
  VALUE block;
};

typedef struct block_wrapper signal_data;
typedef struct block_wrapper prepare_data;
typedef struct {
  VALUE block;
  int buffer_size;
  char *buffer;
} io_read_data;
typedef struct {
  VALUE block;
  int repeat;
} timeout_data;

struct ev_loop *loop;
VALUE mEvIO;
VALUE cStream;
VALUE cFile;

void stream_free();

void Init_evio();
void init_loop();
void init_signal();
void init_stream();
void init_file();

#define SECURE_CHECK rb_secure(2)

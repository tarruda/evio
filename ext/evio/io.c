#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <ruby.h>
#include <ev.h>

#include "evio.h"


VALUE cIO;

static void
io_free(io_stream *stream)
{
  close(stream->fd);
  free(stream);
}

static VALUE
io_open(int argc, VALUE *argv)
{
  io_stream *stream;
  char *path, *mode;
  int flags, filemode;

  rb_secure(2);

  if (argc < 1 || argc > 3)
    rb_raise(rb_eArgError, "wrong number of arguments (%d for 1..3)", argc);

  if (TYPE(argv[0]) != T_STRING)
    rb_raise(rb_eArgError, "first argument must be a string");

  path = RSTRING_PTR(rb_str_encode_ospath(argv[0]));

  if (argc > 1) {
    flags = -1;
    if (TYPE(argv[1]) != T_STRING)
      rb_raise(rb_eArgError, "second argument must be a string");
    mode = RSTRING_PTR(argv[1]);
    if (mode[0] == 'r') {
      if (mode[1] == '\0')
        flags = O_RDONLY;
      else if (mode[1] == '+' && mode[2] == '\0')
        flags = O_RDWR;
    } else if (mode[0] == 'w') {
      if (mode[1] == '\0')
        flags = O_WRONLY | O_CREAT;
      else if (mode[1] == '+' && mode[2] == '\0')
        flags = O_RDWR | O_CREAT;
    } else if (mode[0] == 'a') {
      if (mode[1] == '\0')
        flags = O_WRONLY | O_CREAT | O_APPEND;
      else if (mode[1] == '+' && mode[2] == '\0')
        flags = O_RDWR | O_CREAT | O_APPEND;
    }
    if (flags == -1) rb_raise(rb_eArgError, "invalid file mode string");
  } else {
    flags = O_RDONLY;
  }

  if (argc == 3) {
    if (TYPE(argv[2]) != T_FIXNUM)
      rb_raise(rb_eArgError, "third argument must be an integer");
    filemode = FIX2INT(argv[2]);
  } else {
    filemode = 0644;
  }

  flags |= O_NONBLOCK;
  stream = ALLOC(io_stream);
  stream->fd = open(path, flags, filemode);

  if (stream->fd == -1)
    rb_raise(rb_eSystemCallError, "%s", strerror(errno));

  return Data_Wrap_Struct(cIO, 0, io_free, stream);
}

static void
io_read_cb(struct ev_loop *loop, ev_io *watcher, int revents)
{
  ssize_t read_count;
  VALUE rv = Qnil;
  VALUE ex = Qnil;
  io_read_data *data = watcher->data;

  read_count = read(watcher->fd, data->buffer, data->buffer_size);

  if (read_count == -1) {
    if (errno == EAGAIN) return;
    else ex = rb_exc_new2(rb_eSystemCallError, strerror(errno));
  }

  if (read_count != 0) // not EOF
    rv = rb_funcall(data->block, rb_intern("call"), 2, ex,
        rb_str_new(data->buffer, read_count));

  if (read_count == 0 || rv == Qfalse) {
    ev_io_stop(loop, watcher);
    rb_gc_unregister_address(&data->block);
    free(data->buffer);
    free(data);
    free(watcher);
  }
}

static VALUE
io_read(int argc, VALUE *argv, VALUE self)
{
  ev_io *watcher;
  io_read_data *data;
  io_stream *stream;
  int buffer_size;

  rb_secure(2);

  if (argc > 1)
    rb_raise(rb_eArgError, "must provide zero or one arguments");

  if (!rb_block_given_p())
    rb_raise(rb_eArgError, "a block is required");

  if (argc == 1) {
    if (TYPE(argv[0]) != T_FIXNUM)
      rb_raise(rb_eArgError, "first argument must be an integer");
    buffer_size = FIX2INT(argv[0]);
  } else {
    buffer_size = 4096;
  }

  Data_Get_Struct(self, io_stream, stream);
  watcher = ALLOC(ev_io);
  data = ALLOC(io_read_data);
  data->block = rb_block_proc();
  data->buffer_size = buffer_size;
  data->buffer = ALLOC_N(char, buffer_size);
  rb_gc_register_address(&data->block);
  watcher->data = data;
  ev_io_init(watcher, io_read_cb, stream->fd, EV_READ);
  ev_io_start(loop, watcher);

  return Qnil;
}

static VALUE
io_close(VALUE self)
{
  io_stream *stream;

  Data_Get_Struct(self, io_stream, stream);
  close(stream->fd);

  return Qnil;
}

void init_io(VALUE module)
{
  cIO = rb_define_class_under(module, "IO", rb_cObject); 

  rb_define_singleton_method(cIO, "open", io_open, -1);

  rb_define_method(cIO, "read", io_read, -1);
  rb_define_method(cIO, "close", io_close, 0);
}

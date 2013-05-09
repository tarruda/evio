#include "evio.h"


static void
stream_read_cb(struct ev_loop *loop, ev_io *watcher, int revents)
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
stream_read(int argc, VALUE *argv, VALUE self)
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
  ev_io_init(watcher, stream_read_cb, stream->fd, EV_READ);
  ev_io_start(loop, watcher);

  return Qnil;
}

static VALUE
stream_close(VALUE self)
{
  io_stream *stream;

  Data_Get_Struct(self, io_stream, stream);
  close(stream->fd);

  return Qnil;
}

void
stream_free(io_stream *stream)
{
  close(stream->fd);
  free(stream);
}

void init_stream()
{
  cStream = rb_define_class_under(mEvIO, "Stream", rb_cObject);

  rb_define_method(cStream, "read", stream_read, -1);
  rb_define_method(cStream, "close", stream_close, 0);
}

#include "evio.h"


static void
fs_open_cb(uv_fs_t *req)
{
  event_data *data;
  uv_pipe_t *handle;
  uv_err_t error;
  char *error_msg;
  io_stream *stream;
  VALUE rv, ex, wrapped_stream;

  rv = ex = wrapped_stream = Qnil;
  data = req->data;
  stream = data->extra;
  
  if (req->result == -1) {
    error = uv_last_error(event_loop);
    error_msg = uv_strerror(error);
    ex = rb_exc_new2(rb_eSystemCallError, error_msg);
    free(stream);
  } else {
    stream->fd = req->result;
    handle = ALLOC(uv_pipe_t);
    handle->data = stream;
    uv_pipe_init(event_loop, handle, 0);
    uv_pipe_open(handle, stream->fd);
    wrapped_stream = Data_Wrap_Struct(rb_cObject, 0,
        evio_close_stream, handle);
    rv = rb_funcall(cStream, id_new, 1, wrapped_stream);
  }

  uv_fs_req_cleanup(req);
  rb_funcall(data->block, id_call, 2, ex, rv);
  rb_gc_unregister_address(&data->block);
  free(data);
}

static VALUE
fs_open(int argc, VALUE *argv)
{
  uv_fs_t *req;
  event_data *data;
  io_stream *stream;
  char *path, *mode;
  int flags, filemode;

  SECURE_CHECK;

  if (!rb_block_given_p())
    rb_raise(rb_eArgError, "requires a block");

  if (argc < 1 || argc > 3)
    rb_raise(rb_eArgError, "wrong number of arguments (%d for 1..3)", argc);

  if (TYPE(argv[0]) != T_STRING)
    rb_raise(rb_eArgError, "path argument must be a string");

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
  stream->writable = (flags & O_WRONLY) || (flags & O_RDWR);
  stream->readable = (flags & O_RDONLY) || (flags & O_RDWR);
  stream->closed = 0;
  data = ALLOC(event_data);
  data->block = rb_block_proc();
  data->extra = stream;
  rb_gc_register_address(&data->block);
  req = ALLOC(uv_fs_t);
  req->data = data;
  uv_fs_open(event_loop, req, path, flags, filemode, fs_open_cb);

  return Qnil;
}


void init_file()
{
  cFile = rb_define_class_under(mEvIO, "File", cStream);

  rb_define_singleton_method(cFile, "open", fs_open, -1);
}

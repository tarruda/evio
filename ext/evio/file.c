#include "evio.h"


static void
io_free(io_stream *stream)
{
  close(stream->fd);
  free(stream);
}

static VALUE
file_open(int argc, VALUE *argv)
{
  io_stream *stream;
  char *path, *mode;
  int flags, filemode;

  SECURE_CHECK;

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

  return Data_Wrap_Struct(cFile, 0, stream_free, stream);
}


void init_file()
{
  cFile = rb_define_class_under(mEvIO, "File", cStream);

  rb_define_singleton_method(cFile, "open", file_open, -1);
}

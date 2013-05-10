#include "evio.h"


static void
prepare_cb(struct ev_loop *loop, ev_prepare *watcher, int revents)
{
  block_wrapper *data = watcher->data;

  rb_funcall(data->block, rb_intern("call"), 0);
  ev_prepare_stop(loop, watcher);
  rb_gc_unregister_address(&data->block);
  free(data);
  free(watcher);
}

static VALUE
start()
{
  ev_prepare *watcher;
  block_wrapper *data;

  if (rb_block_given_p()) {
    INSTALL_WATCHER(prepare, block_wrapper);
  }

  ev_run(loop, 0);

  return Qnil;
}

void
init_loop()
{
  loop = EV_DEFAULT;
  rb_define_singleton_method(mEvIO, "start", start, 0);
}

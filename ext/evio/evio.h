
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
} timer_data;

struct ev_loop *loop;

void Init_evio();
void init_ev(VALUE module);
void init_signal(VALUE module);
void init_io(VALUE module);

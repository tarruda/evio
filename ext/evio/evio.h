typedef struct {
  VALUE block;
  int repeat;
} timer_data;

struct block_wrapper {
  VALUE block;
};

typedef struct block_wrapper signal_data;
typedef struct block_wrapper prepare_data;

struct ev_loop *loop;

void Init_evio();
void init_ev(VALUE module);
void init_signal(VALUE module);

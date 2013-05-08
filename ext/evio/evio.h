typedef struct {
  VALUE bound_block;
  int repeat;
} timer_data;

void Init_evio();
void init_ev(VALUE module);
void init_signal(VALUE module);

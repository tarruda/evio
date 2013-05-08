#include <ruby.h>
#include <ev.h>

#include "evio.h"

// mostly copied from signal.c(ruby source)

static const struct signals {
  const char *signm;
  int  signo;
} siglist [] = {
  {"EXIT", 0},
#ifdef SIGHUP
  {"HUP", SIGHUP},
#endif
  {"INT", SIGINT},
#ifdef SIGQUIT
  {"QUIT", SIGQUIT},
#endif
#ifdef SIGILL
  {"ILL", SIGILL},
#endif
#ifdef SIGTRAP
  {"TRAP", SIGTRAP},
#endif
#ifdef SIGIOT
  {"IOT", SIGIOT},
#endif
#ifdef SIGABRT
  {"ABRT", SIGABRT},
#endif
#ifdef SIGEMT
  {"EMT", SIGEMT},
#endif
#ifdef SIGFPE
  {"FPE", SIGFPE},
#endif
#ifdef SIGKILL
  {"KILL", SIGKILL},
#endif
#ifdef SIGBUS
  {"BUS", SIGBUS},
#endif
#ifdef SIGSEGV
  {"SEGV", SIGSEGV},
#endif
#ifdef SIGSYS
  {"SYS", SIGSYS},
#endif
#ifdef SIGPIPE
  {"PIPE", SIGPIPE},
#endif
#ifdef SIGALRM
  {"ALRM", SIGALRM},
#endif
#ifdef SIGTERM
  {"TERM", SIGTERM},
#endif
#ifdef SIGURG
  {"URG", SIGURG},
#endif
#ifdef SIGSTOP
  {"STOP", SIGSTOP},
#endif
#ifdef SIGTSTP
  {"TSTP", SIGTSTP},
#endif
#ifdef SIGCONT
  {"CONT", SIGCONT},
#endif
#ifdef SIGCHLD
  {"CHLD", SIGCHLD},
#endif
#ifdef SIGCLD
  {"CLD", SIGCLD},
#else
# ifdef SIGCHLD
  {"CLD", SIGCHLD},
# endif
#endif
#ifdef SIGTTIN
  {"TTIN", SIGTTIN},
#endif
#ifdef SIGTTOU
  {"TTOU", SIGTTOU},
#endif
#ifdef SIGIO
  {"IO", SIGIO},
#endif
#ifdef SIGXCPU
  {"XCPU", SIGXCPU},
#endif
#ifdef SIGXFSZ
  {"XFSZ", SIGXFSZ},
#endif
#ifdef SIGVTALRM
  {"VTALRM", SIGVTALRM},
#endif
#ifdef SIGPROF
  {"PROF", SIGPROF},
#endif
#ifdef SIGWINCH
  {"WINCH", SIGWINCH},
#endif
#ifdef SIGUSR1
  {"USR1", SIGUSR1},
#endif
#ifdef SIGUSR2
  {"USR2", SIGUSR2},
#endif
#ifdef SIGLOST
  {"LOST", SIGLOST},
#endif
#ifdef SIGMSG
  {"MSG", SIGMSG},
#endif
#ifdef SIGPWR
  {"PWR", SIGPWR},
#endif
#ifdef SIGPOLL
  {"POLL", SIGPOLL},
#endif
#ifdef SIGDANGER
  {"DANGER", SIGDANGER},
#endif
#ifdef SIGMIGRATE
  {"MIGRATE", SIGMIGRATE},
#endif
#ifdef SIGPRE
  {"PRE", SIGPRE},
#endif
#ifdef SIGGRANT
  {"GRANT", SIGGRANT},
#endif
#ifdef SIGRETRACT
  {"RETRACT", SIGRETRACT},
#endif
#ifdef SIGSOUND
  {"SOUND", SIGSOUND},
#endif
#ifdef SIGINFO
  {"INFO", SIGINFO},
#endif
  {NULL, 0}
};

static int
signm2signo(const char *nm)
{
  const struct signals *sigs;

  for (sigs = siglist; sigs->signm; sigs++)
    if (strcmp(sigs->signm, nm) == 0)
      return sigs->signo;
  return 0;
}

static const char*
signo2signm(int no)
{
  const struct signals *sigs;

  for (sigs = siglist; sigs->signm; sigs++)
    if (sigs->signo == no)
      return sigs->signm;
  return 0;
}

static int
trap_signm(VALUE vsig)
{
  int sig = -1;
  const char *s;

  switch (TYPE(vsig)) {
    case T_FIXNUM:
      sig = FIX2INT(vsig);
      if (sig < 0 || sig >= NSIG) {
        rb_raise(rb_eArgError, "invalid signal number (%d)", sig);
      }
      break;

    case T_SYMBOL:
      s = rb_id2name(SYM2ID(vsig));
      if (!s) rb_raise(rb_eArgError, "bad signal");
      goto str_signal;

    default:
      s = StringValuePtr(vsig);

    str_signal:
      if (strncmp("SIG", s, 3) == 0)
        s += 3;
      sig = signm2signo(s);
      if (sig == 0 && strcmp(s, "EXIT") != 0)
        rb_raise(rb_eArgError, "unsupported signal SIG%s", s);
  }
  return sig;
}

// TODO do something here
static void
default_cb(struct ev_loop *loop, ev_signal *watcher, int revents)
{

}

static void
ignore_cb(struct ev_loop *loop, ev_signal *watcher, int revents)
{

}

static void
exit_cb(struct ev_loop *loop, ev_signal *watcher, int revents)
{
  exit(watcher->signum);
}

static void
block_cb(struct ev_loop *loop, ev_signal *watcher, int revents)
{
  signal_data *data = watcher->data;
  VALUE rv;

  rv = rb_funcall(data->block, rb_intern("call"), 1, INT2FIX(watcher->signum));

  if (rv == Qfalse) {
    ev_signal_stop(loop, watcher);
    rb_gc_unregister_address(&data->block);
    free(data);
    free(watcher);
  }
}

static VALUE
sig_trap(int argc, VALUE *argv)
{
  ev_signal *watcher;
  VALUE cmd;
  signal_data *data;
  int sign;

  rb_secure(2);

  if (argc < 1 || argc > 2)
    rb_raise(rb_eArgError, "wrong number of arguments (%d for 1..2)", argc);

  sign = trap_signm(argv[0]);
  watcher = ALLOC(ev_signal);

  if (argc == 1) {
    data = ALLOC(signal_data);
    data->block = rb_block_proc();
    watcher->data = data;
    rb_gc_register_address(&data->block);
    ev_signal_init(watcher, block_cb, sign);
  } else {
    cmd = argv[1];
    if (OBJ_TAINTED(cmd))
      rb_raise(rb_eSecurityError, "Insecure: tainted signal trap");
    if (!NIL_P(cmd)) {
      switch (RSTRING_LEN(cmd)) {
        case 0:
          goto sig_ign;
          break;
        case 14:
          if (strncmp(RSTRING_PTR(cmd), "SYSTEM_DEFAULT", 14) == 0) {
sig_dfl:
            ev_signal_init(watcher, default_cb, sign);
          }
          break;
        case 7:
          if (strncmp(RSTRING_PTR(cmd), "SIG_IGN", 7) == 0) {
sig_ign:
            ev_signal_init(watcher, ignore_cb, sign);
          }
          else if (strncmp(RSTRING_PTR(cmd), "SIG_DFL", 7) == 0) {
            goto sig_dfl;
          }
          else if (strncmp(RSTRING_PTR(cmd), "DEFAULT", 7) == 0) {
            goto sig_dfl;
          }
          break;
        case 6:
          if (strncmp(RSTRING_PTR(cmd), "IGNORE", 6) == 0)
            goto sig_ign;
          break;
        case 4:
          if (strncmp(RSTRING_PTR(cmd), "EXIT", 4) == 0) {
            ev_signal_init(watcher, exit_cb, sign);
          }
          break;
      }
    }
  }

  ev_signal_start(loop, watcher);
  return Qnil;
}

void
init_signal(VALUE module)
{
  VALUE signal_module = rb_define_module_under(module, "Signal");

  rb_define_module_function(signal_module, "trap", sig_trap, -1);
}

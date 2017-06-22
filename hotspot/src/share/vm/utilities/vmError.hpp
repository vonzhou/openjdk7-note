
#ifndef SHARE_VM_UTILITIES_VMERROR_HPP
#define SHARE_VM_UTILITIES_VMERROR_HPP

#include "utilities/globalDefinitions.hpp"


class VM_ReportJavaOutOfMemory;

class VMError : public StackObj {
  friend class VM_ReportJavaOutOfMemory;

  enum ErrorType {
    internal_error = 0xe0000000,
    oom_error      = 0xe0000001
  };
  int          _id;          // Solaris/Linux signals: 0 - SIGRTMAX
                             // Windows exceptions: 0xCxxxxxxx system errors
                             //                     0x8xxxxxxx system warnings

  const char * _message;
  const char * _detail_msg;

  Thread *     _thread;      // NULL if it's native thread


  // additional info for crashes
  address      _pc;          // faulting PC
  void *       _siginfo;     // ExceptionRecord on Windows,
                             // siginfo_t on Solaris/Linux
  void *       _context;     // ContextRecord on Windows,
                             // ucontext_t on Solaris/Linux

  // additional info for VM internal errors
  const char * _filename;
  int          _lineno;

  // used by fatal error handler
  int          _current_step;
  const char * _current_step_info;
  int          _verbose;
  // First error, and its thread id. We must be able to handle native thread,
  // so use thread id instead of Thread* to identify thread.
  static VMError* volatile first_error;
  static volatile jlong    first_error_tid;

  // Core dump status, false if we have been unable to write a core/minidump for some reason
  static bool coredump_status;

  // When coredump_status is set to true this will contain the name/path to the core/minidump,
  // if coredump_status if false, this will (hopefully) contain a useful error explaining why
  // no core/minidump has been written to disk
  static char coredump_message[O_BUFLEN];

  // used by reporting about OOM
  size_t       _size;

  // set signal handlers on Solaris/Linux or the default exception filter
  // on Windows, to handle recursive crashes.
  void reset_signal_handlers();

  // handle -XX:+ShowMessageBoxOnError. buf is used to format the message string
  void show_message_box(char* buf, int buflen);

  // generate an error report
  void report(outputStream* st);

  // generate a stack trace
  static void print_stack_trace(outputStream* st, JavaThread* jt,
                                char* buf, int buflen, bool verbose = false);

  // accessor
  const char* message() const    { return _message; }
  const char* detail_msg() const { return _detail_msg; }
  bool should_report_bug(unsigned int id) { return id != oom_error; }

public:
  // Constructor for crashes
  VMError(Thread* thread, unsigned int sig, address pc, void* siginfo,
          void* context);
  // Constructor for VM internal errors
  VMError(Thread* thread, const char* filename, int lineno,
          const char* message, const char * detail_msg);

  // Constructor for VM OOM errors
  VMError(Thread* thread, const char* filename, int lineno, size_t size,
          const char* message);
  // Constructor for non-fatal errors
  VMError(const char* message);

  // return a string to describe the error
  char *error_string(char* buf, int buflen);

  // Report status of core/minidump
  static void report_coredump_status(const char* message, bool status);

  // main error reporting function
  void report_and_die();

  // reporting OutOfMemoryError
  void report_java_out_of_memory();

  // returns original flags for signal, if it was resetted, or -1 if
  // signal was not changed by error reporter
  static int get_resetted_sigflags(int sig);

  // returns original handler for signal, if it was resetted, or NULL if
  // signal was not changed by error reporter
  static address get_resetted_sighandler(int sig);

  // check to see if fatal error reporting is in progress
  static bool fatal_error_in_progress() { return first_error != NULL; }
};

#endif // SHARE_VM_UTILITIES_VMERROR_HPP

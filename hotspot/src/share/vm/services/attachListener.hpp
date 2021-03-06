#ifndef SHARE_VM_SERVICES_ATTACHLISTENER_HPP
#define SHARE_VM_SERVICES_ATTACHLISTENER_HPP

#include "memory/allocation.hpp"
#include "utilities/debug.hpp"
#include "utilities/ostream.hpp"

// The AttachListener thread services a queue of operations that are enqueued
// by client tools. Each operation is identified by a name and has up to 3
// arguments. The operation name is mapped to a function which performs the
// operation. The function is called with an outputStream which is can use to
// write any result data (for examples the properties command serializes
// properties names and values to the output stream). When the function
// complets the result value and any result data is returned to the client
// tool.

#ifndef SERVICES_KERNEL

class AttachOperation;

typedef jint (*AttachOperationFunction)(AttachOperation* op, outputStream* out);

struct AttachOperationFunctionInfo {
  const char* name;
  AttachOperationFunction func;
};
#endif // SERVICES_KERNEL

class AttachListener: AllStatic {
 public:
  static void init()  KERNEL_RETURN;
  static void abort() KERNEL_RETURN;

  // invoke to perform clean-up tasks when all clients detach
  static void detachall() KERNEL_RETURN;

  // indicates if the Attach Listener needs to be created at startup
  static bool init_at_startup() KERNEL_RETURN_(false);

  // indicates if we have a trigger to start the Attach Listener
  static bool is_init_trigger() KERNEL_RETURN_(false);

#ifdef SERVICES_KERNEL
  static bool is_attach_supported()             { return false; }
#else // SERVICES_KERNEL
 private:
  static volatile bool _initialized;

 public:
  static bool is_initialized()                  { return _initialized; }
  static void set_initialized()                 { _initialized = true; }

  // indicates if this VM supports attach-on-demand
  static bool is_attach_supported()             { return !DisableAttachMechanism; }

  // platform specific initialization
  static int pd_init();

  // platform specific operation
  static AttachOperationFunctionInfo* pd_find_operation(const char* name);

  // platform specific flag change
  static jint pd_set_flag(AttachOperation* op, outputStream* out);

  // platform specific detachall
  static void pd_detachall();

  // platform specific data dump
  static void pd_data_dump();

  // dequeue the next operation
  static AttachOperation* dequeue();
#endif // SERVICES_KERNEL
};

#ifndef SERVICES_KERNEL
class AttachOperation: public CHeapObj {
 public:
  enum {
    name_length_max = 16,       // maximum length of  name
    arg_length_max = 1024,      // maximum length of argument
    arg_count_max = 3           // maximum number of arguments
  };

  // name of special operation that can be enqueued when all
  // clients detach
  static char* detachall_operation_name() { return (char*)"detachall"; }

 private:
  char _name[name_length_max+1];
  char _arg[arg_count_max][arg_length_max+1];

 public:
  const char* name() const                      { return _name; }

  // set the operation name
  void set_name(char* name) {
    assert(strlen(name) <= name_length_max, "exceeds maximum name length");
    strcpy(_name, name);
  }

  // get an argument value
  const char* arg(int i) const {
    assert(i>=0 && i<arg_count_max, "invalid argument index");
    return _arg[i];
  }

  // set an argument value
  void set_arg(int i, char* arg) {
    assert(i>=0 && i<arg_count_max, "invalid argument index");
    if (arg == NULL) {
      _arg[i][0] = '\0';
    } else {
      assert(strlen(arg) <= arg_length_max, "exceeds maximum argument length");
      strcpy(_arg[i], arg);
    }
  }

  // create an operation of a given name
  AttachOperation(char* name) {
    set_name(name);
    for (int i=0; i<arg_count_max; i++) {
      set_arg(i, NULL);
    }
  }

  // complete operation by sending result code and any result data to the client
  virtual void complete(jint result, bufferedStream* result_stream) = 0;
};
#endif // SERVICES_KERNEL

#endif // SHARE_VM_SERVICES_ATTACHLISTENER_HPP

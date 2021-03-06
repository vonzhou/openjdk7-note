
#ifndef SHARE_VM_RUNTIME_VMTHREAD_HPP
#define SHARE_VM_RUNTIME_VMTHREAD_HPP

#include "runtime/perfData.hpp"
#include "runtime/vm_operations.hpp"
#ifdef TARGET_OS_FAMILY_linux
# include "thread_linux.inline.hpp"
#endif
#ifdef TARGET_OS_FAMILY_solaris
# include "thread_solaris.inline.hpp"
#endif
#ifdef TARGET_OS_FAMILY_windows
# include "thread_windows.inline.hpp"
#endif

/**
 * 通常情况下是因为这些操作需要VM到达safepoints后执行
 * During a safepoint the Threads_lock is used to block any threads that were running, with the VMThread finally releasing the Threads_lock after the VM operation has been performed.
 */
 
// Prioritized queue of VM operations.
//
// Encapsulates both queue management and
// and priority policy
//
class VMOperationQueue : public CHeapObj {
 private:
  enum Priorities {
     SafepointPriority, // Highest priority (operation executed at a safepoint)
     MediumPriority,    // Medium priority
     nof_priorities
  };

  // We maintain a doubled linked list, with explicit count.
  int           _queue_length[nof_priorities];
  int           _queue_counter;
  VM_Operation* _queue       [nof_priorities];
  // we also allow the vmThread to register the ops it has drained so we
  // can scan them from oops_do
  VM_Operation* _drain_list;

  // Double-linked non-empty list insert.
  void insert(VM_Operation* q,VM_Operation* n);
  void unlink(VM_Operation* q);

  // Basic queue manipulation
  bool queue_empty                (int prio);
  void queue_add_front            (int prio, VM_Operation *op);
  void queue_add_back             (int prio, VM_Operation *op);
  VM_Operation* queue_remove_front(int prio);
  void queue_oops_do(int queue, OopClosure* f);
  void drain_list_oops_do(OopClosure* f);
  VM_Operation* queue_drain(int prio);
  // lock-free query: may return the wrong answer but must not break
  bool queue_peek(int prio) { return _queue_length[prio] > 0; }

 public:
  VMOperationQueue();

  // Highlevel operations. Encapsulates policy
  bool add(VM_Operation *op);
  VM_Operation* remove_next();                        // Returns next or null
  VM_Operation* remove_next_at_safepoint_priority()   { return queue_remove_front(SafepointPriority); }
  VM_Operation* drain_at_safepoint_priority() { return queue_drain(SafepointPriority); }
  void set_drain_list(VM_Operation* list) { _drain_list = list; }
  bool peek_at_safepoint_priority() { return queue_peek(SafepointPriority); }

  // GC support
  void oops_do(OopClosure* f);

  void verify_queue(int prio) PRODUCT_RETURN;
};


//
// A single VMThread (the primordial thread) spawns all other threads
// and is itself used by other threads to offload heavy vm operations
// like scavenge, garbage_collect etc.
//

class VMThread: public NamedThread {
 private:
  static ThreadPriority _current_priority;

  static bool _should_terminate;
  static bool _terminated;
  static Monitor * _terminate_lock;
  static PerfCounter* _perf_accumulated_vm_operation_time;

  void evaluate_operation(VM_Operation* op);
 public:
  // Constructor
  VMThread();

  // Tester
  bool is_VM_thread() const                      { return true; }
  bool is_GC_thread() const                      { return true; }

  // The ever running loop for the VMThread
  void loop();

  // Called to stop the VM thread
  static void wait_for_vm_thread_exit();
  static bool should_terminate()                  { return _should_terminate; }
  static bool is_terminated()                     { return _terminated == true; }

  // Execution of vm operation
  static void execute(VM_Operation* op);

  // Returns the current vm operation if any.
  static VM_Operation* vm_operation()             { return _cur_vm_operation;   }

  // Returns the single instance of VMThread.
  static VMThread* vm_thread()                    { return _vm_thread; }

  // GC support
  void oops_do(OopClosure* f, CodeBlobClosure* cf);

  // Debugging
  void print_on(outputStream* st) const;
  void print() const                              { print_on(tty); }
  void verify();

  // Performance measurement
  static PerfCounter* perf_accumulated_vm_operation_time()               { return _perf_accumulated_vm_operation_time; }

  // Entry for starting vm thread
  virtual void run();

  // Creations/Destructions
  static void create();
  static void destroy();

 private:
  // VM_Operation support
  static VM_Operation*     _cur_vm_operation;   // Current VM operation
  static VMOperationQueue* _vm_queue;           // Queue (w/ policy) of VM operations

  // Pointer to single-instance of VM thread
  static VMThread*     _vm_thread;
};

#endif // SHARE_VM_RUNTIME_VMTHREAD_HPP

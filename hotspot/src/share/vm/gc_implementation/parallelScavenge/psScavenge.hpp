/*
 * scavenge :���������
 * �����Ĭ�ϵ����������㷨 Parallel Scavenge
 * ʹ��ѡ��-verbose:gc -XX:+PrintGCDetails -XX:+PrintGCTimeStamps ����չʾʹ�õ�GC�㷨
 * ��: 
 
 Heap
  PSYoungGen	  total 173568K, used 166144K [0x00000000f5580000, 0x0000000100000000, 0x0000000100000000)
   eden space 172544K, 95% used [0x00000000f5580000,0x00000000ff7383f0,0x00000000ffe00000)
   from space 1024K, 53% used [0x00000000ffe00000,0x00000000ffe88000,0x00000000fff00000)
   to	space 1024K, 0% used [0x00000000fff00000,0x00000000fff00000,0x0000000100000000)
  ParOldGen 	  total 349696K, used 62533K [0x00000000e0000000, 0x00000000f5580000, 0x00000000f5580000)
   object space 349696K, 17% used [0x00000000e0000000,0x00000000e3d11788,0x00000000f5580000)
  Metaspace 	  used 70116K, capacity 71782K, committed 72064K, reserved 1114112K
   class space	  used 7953K, capacity 8252K, committed 8320K, reserved 1048576K
 */

#ifndef SHARE_VM_GC_IMPLEMENTATION_PARALLELSCAVENGE_PSSCAVENGE_HPP
#define SHARE_VM_GC_IMPLEMENTATION_PARALLELSCAVENGE_PSSCAVENGE_HPP

#include "gc_implementation/parallelScavenge/cardTableExtension.hpp"
#include "gc_implementation/parallelScavenge/psVirtualspace.hpp"
#include "gc_implementation/shared/collectorCounters.hpp"
#include "memory/allocation.hpp"
#include "oops/oop.hpp"
#include "utilities/stack.hpp"

class GCTaskManager;
class GCTaskQueue;
class OopStack;
class ReferenceProcessor;
class ParallelScavengeHeap;
class PSIsAliveClosure;
class PSRefProcTaskExecutor;

class PSScavenge: AllStatic {
  friend class PSIsAliveClosure;
  friend class PSKeepAliveClosure;
  friend class PSPromotionManager;

 enum ScavengeSkippedCause {
   not_skipped = 0,
   to_space_not_empty,
   promoted_too_large,
   full_follows_scavenge
 };

  // Saved value of to_space->top(), used to prevent objects in to_space from
  // being rescanned.
  static HeapWord* _to_space_top_before_gc;

  // Number of consecutive attempts to scavenge that were skipped
  static int                _consecutive_skipped_scavenges;


 protected:
  // Flags/counters
  static ReferenceProcessor* _ref_processor;        // Reference processor for scavenging.
  static PSIsAliveClosure    _is_alive_closure;     // Closure used for reference processing
  static CardTableExtension* _card_table;           // We cache the card table for fast access.
  static bool                _survivor_overflow;    // Overflow this collection
  static int                 _tenuring_threshold;   // tenuring threshold for next scavenge
  static elapsedTimer        _accumulated_time;     // total time spent on scavenge
  static HeapWord*           _young_generation_boundary; // The lowest address possible for the young_gen.
                                                         // This is used to decide if an oop should be scavenged,
                                                         // cards should be marked, etc.
  static Stack<markOop>          _preserved_mark_stack; // List of marks to be restored after failed promotion
  static Stack<oop>              _preserved_oop_stack;  // List of oops that need their mark restored.
  static CollectorCounters*      _counters;         // collector performance counters
  static bool                    _promotion_failed;

  static void clean_up_failed_promotion();

  static bool should_attempt_scavenge();

  static HeapWord* to_space_top_before_gc() { return _to_space_top_before_gc; }
  static inline void save_to_space_top_before_gc();

  // Private accessors
  static CardTableExtension* const card_table()       { assert(_card_table != NULL, "Sanity"); return _card_table; }

 public:
  // Accessors
  static int              tenuring_threshold()  { return _tenuring_threshold; }
  static elapsedTimer*    accumulated_time()    { return &_accumulated_time; }
  static bool             promotion_failed()    { return _promotion_failed; }
  static int              consecutive_skipped_scavenges()
    { return _consecutive_skipped_scavenges; }

  // Performance Counters
  static CollectorCounters* counters()           { return _counters; }

  // Used by scavenge_contents && psMarkSweep
  static ReferenceProcessor* const reference_processor() {
    assert(_ref_processor != NULL, "Sanity");
    return _ref_processor;
  }
  // Used to add tasks
  static GCTaskManager* const gc_task_manager();
  // The promotion managers tell us if they encountered overflow
  static void set_survivor_overflow(bool state) {
    _survivor_overflow = state;
  }
  // Adaptive size policy support.  When the young generation/old generation
  // boundary moves, _young_generation_boundary must be reset
  static void set_young_generation_boundary(HeapWord* v) {
    _young_generation_boundary = v;
  }

  // Called by parallelScavengeHeap to init the tenuring threshold
  static void initialize();

  // Scavenge entry point
  static void invoke();
  // Return true is a collection was done.  Return
  // false if the collection was skipped.
  static bool invoke_no_policy();

  // If an attempt to promote fails, this method is invoked
  static void oop_promotion_failed(oop obj, markOop obj_mark);

  template <class T> static inline bool should_scavenge(T* p);

  // These call should_scavenge() above and, if it returns true, also check that
  // the object was not newly copied into to_space.  The version with the bool
  // argument is a convenience wrapper that fetches the to_space pointer from
  // the heap and calls the other version (if the arg is true).
  template <class T> static inline bool should_scavenge(T* p, MutableSpace* to_space);
  template <class T> static inline bool should_scavenge(T* p, bool check_to_space);

  template <class T> inline static void copy_and_push_safe_barrier(PSPromotionManager* pm, T* p);

  // Is an object in the young generation
  // This assumes that the HeapWord argument is in the heap,
  // so it only checks one side of the complete predicate.
  inline static bool is_obj_in_young(HeapWord* o) {
    const bool result = (o >= _young_generation_boundary);
    return result;
  }
};

#endif // SHARE_VM_GC_IMPLEMENTATION_PARALLELSCAVENGE_PSSCAVENGE_HPP

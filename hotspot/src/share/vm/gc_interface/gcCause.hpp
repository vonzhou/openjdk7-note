#ifndef SHARE_VM_GC_INTERFACE_GCCAUSE_HPP
#define SHARE_VM_GC_INTERFACE_GCCAUSE_HPP

#include "memory/allocation.hpp"

//
// This class exposes implementation details of the various
// collector(s), and we need to be very careful with it. If
// use of this class grows, we should split it into public
// and implemenation-private "causes".
//

class GCCause : public AllStatic {
 public:
  enum Cause {
    /* public */
    _java_lang_system_gc,
    _full_gc_alot,
    _scavenge_alot,
    _allocation_profiler,
    _jvmti_force_gc,
    _gc_locker,
    _heap_inspection,
    _heap_dump,

    /* implementation independent, but reserved for GC use */
    _no_gc,
    _no_cause_specified,
    _allocation_failure,

    /* implementation specific */

    _tenured_generation_full,
    _permanent_generation_full,

    _cms_generation_full,
    _cms_initial_mark,
    _cms_final_remark,

    _old_generation_expanded_on_last_scavenge,
    _old_generation_too_full_to_scavenge,
    _adaptive_size_policy,

    _g1_inc_collection_pause,

    _last_ditch_collection,
    _last_gc_cause
  };

  inline static bool is_user_requested_gc(GCCause::Cause cause) {
    return (cause == GCCause::_java_lang_system_gc ||
            cause == GCCause::_jvmti_force_gc);
  }

  inline static bool is_serviceability_requested_gc(GCCause::Cause
                                                             cause) {
    return (cause == GCCause::_jvmti_force_gc ||
            cause == GCCause::_heap_inspection ||
            cause == GCCause::_heap_dump);
  }

  // Return a string describing the GCCause.
  static const char* to_string(GCCause::Cause cause);
};

#endif // SHARE_VM_GC_INTERFACE_GCCAUSE_HPP


#include "precompiled.hpp"
#include "memory/allocation.inline.hpp"
#include "memory/resourceArea.hpp"
#include "runtime/mutexLocker.hpp"
#ifdef TARGET_OS_FAMILY_linux
# include "thread_linux.inline.hpp"
#endif
#ifdef TARGET_OS_FAMILY_solaris
# include "thread_solaris.inline.hpp"
#endif
#ifdef TARGET_OS_FAMILY_windows
# include "thread_windows.inline.hpp"
#endif

//------------------------------ResourceMark-----------------------------------
debug_only(int ResourceArea::_warned;)      // to suppress multiple warnings

// The following routines are declared in allocation.hpp and used everywhere:

// Allocation in thread-local resource area
extern char* resource_allocate_bytes(size_t size) {
  return Thread::current()->resource_area()->allocate_bytes(size);
}
extern char* resource_allocate_bytes(Thread* thread, size_t size) {
  return thread->resource_area()->allocate_bytes(size);
}

extern char* resource_reallocate_bytes( char *old, size_t old_size, size_t new_size){
  return (char*)Thread::current()->resource_area()->Arealloc(old, old_size, new_size);
}

extern void resource_free_bytes( char *old, size_t size ) {
  Thread::current()->resource_area()->Afree(old, size);
}

#ifdef ASSERT
ResourceMark::ResourceMark(Thread *thread) {
  assert(thread == Thread::current(), "not the current thread");
  initialize(thread);
}

DeoptResourceMark::DeoptResourceMark(Thread *thread) {
  assert(thread == Thread::current(), "not the current thread");
  initialize(thread);
}
#endif


//-------------------------------------------------------------------------------
// Non-product code
#ifndef PRODUCT

void ResourceMark::free_malloced_objects() {
  Arena::free_malloced_objects(_chunk, _hwm, _max, _area->_hwm);
}

void DeoptResourceMark::free_malloced_objects() {
  Arena::free_malloced_objects(_chunk, _hwm, _max, _area->_hwm);
}

#endif

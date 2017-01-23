#ifndef SHARE_VM_OOPS_KLASSOOP_HPP
#define SHARE_VM_OOPS_KLASSOOP_HPP

#include "oops/oop.hpp"

// A klassOop is the C++ equivalent of a Java class.
// Part of a klassOopDesc is a Klass which handle the
// dispatching for the C++ method calls.

//  klassOop object layout:
//    [header     ]
//    [klass_field]
//    [KLASS      ]

class klassOopDesc : public oopDesc {
 public:
  // size operation
  static int header_size()                       { return sizeof(klassOopDesc)/HeapWordSize; }

  // support for code generation
  static int klass_part_offset_in_bytes()        { return sizeof(klassOopDesc); }

  // returns the Klass part containing dispatching behavior
  Klass* klass_part() const                      { return (Klass*)((address)this + klass_part_offset_in_bytes()); }

  // Convenience wrapper
  inline oop java_mirror() const;

 private:
  // These have no implementation since klassOop should never be accessed in this fashion
  oop obj_field(int offset) const;
  void obj_field_put(int offset, oop value);
  void obj_field_raw_put(int offset, oop value);

  jbyte byte_field(int offset) const;
  void byte_field_put(int offset, jbyte contents);

  jchar char_field(int offset) const;
  void char_field_put(int offset, jchar contents);

  jboolean bool_field(int offset) const;
  void bool_field_put(int offset, jboolean contents);

  jint int_field(int offset) const;
  void int_field_put(int offset, jint contents);

  jshort short_field(int offset) const;
  void short_field_put(int offset, jshort contents);

  jlong long_field(int offset) const;
  void long_field_put(int offset, jlong contents);

  jfloat float_field(int offset) const;
  void float_field_put(int offset, jfloat contents);

  jdouble double_field(int offset) const;
  void double_field_put(int offset, jdouble contents);

  address address_field(int offset) const;
  void address_field_put(int offset, address contents);

  oop obj_field_acquire(int offset) const;
  void release_obj_field_put(int offset, oop value);

  jbyte byte_field_acquire(int offset) const;
  void release_byte_field_put(int offset, jbyte contents);

  jchar char_field_acquire(int offset) const;
  void release_char_field_put(int offset, jchar contents);

  jboolean bool_field_acquire(int offset) const;
  void release_bool_field_put(int offset, jboolean contents);

  jint int_field_acquire(int offset) const;
  void release_int_field_put(int offset, jint contents);

  jshort short_field_acquire(int offset) const;
  void release_short_field_put(int offset, jshort contents);

  jlong long_field_acquire(int offset) const;
  void release_long_field_put(int offset, jlong contents);

  jfloat float_field_acquire(int offset) const;
  void release_float_field_put(int offset, jfloat contents);

  jdouble double_field_acquire(int offset) const;
  void release_double_field_put(int offset, jdouble contents);

  address address_field_acquire(int offset) const;
  void release_address_field_put(int offset, address contents);
};

#endif // SHARE_VM_OOPS_KLASSOOP_HPP

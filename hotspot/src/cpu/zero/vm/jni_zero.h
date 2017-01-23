
#if defined(__GNUC__) && (__GNUC__ >= 4)
  #define JNIEXPORT     __attribute__((visibility("default")))
  #define JNIIMPORT     __attribute__((visibility("default")))
#else
  #define JNIEXPORT
  #define JNIIMPORT
#endif
#define JNICALL

typedef int jint;
typedef signed char jbyte;

#ifdef _LP64
typedef long jlong;
#else
typedef long long jlong;
#endif

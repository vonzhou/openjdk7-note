/*
* 
 */

#include "jvm.h"
#include "java_lang_String.h"

JNIEXPORT jobject JNICALL
Java_java_lang_String_intern(JNIEnv *env, jobject this)
{
    return JVM_InternString(env, this);
}

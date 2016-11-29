#include <jni.h>
#include <stdio.h>
#include <trace/trace.h>

static void throwNullPointerException(JNIEnv* env) {
  (*env)->ThrowNew(env, (*env)->FindClass(env, "java/lang/NullPointerException"), "");
}

JNIEXPORT void JNICALL Java_com_ayxia_trace_JNITrace_initialize(
  JNIEnv* env,
  jobject thisobj,
  jstring remote_host,
  jstring process_name,
  jint flags
) {
  ayxia_trace_initialize init = {
    .remote_host = (*env)->GetStringUTFChars(env, remote_host, 0),
    .process_name = (*env)->GetStringUTFChars(env, process_name, 0),
    .max_network_memory_kb = 1024 * 1024,
  };

  if (init.remote_host == 0 || init.process_name == 0) {
    throwNullPointerException(env);
  }
  else {
    ayxia_tc_initialize(&init);
  }
}

JNIEXPORT void JNICALL Java_com_ayxia_trace_JNITrace_simpleTrace(
  JNIEnv* env,
  jobject thisobj,
  jint level,
  jstring channel,
  jstring text,
  jstring filename,
  jint lineno
) {
  const char * strChannel = (*env)->GetStringUTFChars(env, channel, 0);
  const char * strText = (*env)->GetStringUTFChars(env, text, 0);
  const char * strFileName = (*env)->GetStringUTFChars(env, filename, 0);
  if (!strChannel || !strText || !strFileName)
    throwNullPointerException(env);
  else
    ayxia_tc_simple_trace(level, strChannel, strText, strFileName, lineno);
}

JNIEXPORT void JNICALL Java_com_ayxia_trace_JNITrace_shutdown(JNIEnv* env) {
  ayxia_tc_shutdown();
}

JNIEXPORT void JNICALL Java_com_ayxia_trace_JNITrace_startFrame(JNIEnv* env) {
  ayxia_tc_start_frame();
}


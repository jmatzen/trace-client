#include <trace/trace.h>
#include <memory>
#include <jni.h>
#include <exception>
#include <string>

class JEnv;

class JClass {
public:
  JClass(JEnv& e_, jclass obj_)
    : e(e_)
    , obj(obj_) {}


  template<typename T>
  T get(const char* fieldName) {
    jfieldID id = e.env().GetFieldID(obj, fieldName, "Ljava/lang/String;");
    if (id == 0) {
      throw std::exception("field not found");
    }
    jstring val = (jstring)e.env().GetObjectField(obj, id);
    return e.env().GetStringUTFChars(val, 0);
  }

private:
  JEnv& e;
  jclass obj;
};

class JEnv {
public:
  JEnv(JNIEnv& env)
    : _env(env) {
  }

  std::unique_ptr<JClass> JEnv::getObjectClass(jobject const obj) {
    return std::make_unique<JClass>(*this, _env.GetObjectClass(obj));
  }

  JNIEnv& env() {
    return _env;
  }



private:
  JNIEnv& _env;

};



JNIEXPORT void JNICALL jni_tc_initialize(JNIEnv* e, jobject const initializer) {
  JEnv env(*e);
  auto initClass = env.getObjectClass(initializer);
  auto remote_host = initClass->get<std::string>("remoteHost");

  //jclass initClass = (*e)->GetObjectClass(initializer);
  //jfieldID fid = (*e)->GetFieldID(e, initClass, "remoteHost", "S");

}
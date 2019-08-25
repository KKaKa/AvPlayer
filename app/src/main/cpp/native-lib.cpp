#include <jni.h>
#include <string>
#include "AvFFmpeg.h"

JavaVM *javaVM = 0;
JavaCallHelper *javaCallHelper = 0;
AvFFmpeg *fFmpeg = 0;

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    javaVM = vm;
    return JNI_VERSION_1_4;
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkaka_avpalyer_AvPlayer_nativePrepare(JNIEnv *env, jobject instance, jstring dataSource_) {
    const char *dataSource = env->GetStringUTFChars(dataSource_, 0);
    javaCallHelper = new JavaCallHelper(javaVM,env,instance);
    fFmpeg = new AvFFmpeg(javaCallHelper, const_cast<char *>(dataSource));
    fFmpeg->prepare();
    env->ReleaseStringUTFChars(dataSource_, dataSource);
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkaka_avpalyer_AvPlayer_nativeStart(JNIEnv *env, jobject instance) {
    if(fFmpeg){
        fFmpeg->start();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkaka_avpalyer_AvPlayer_nativeSetSurface(JNIEnv *env, jobject instance, jobject surface) {

    // TODO

}
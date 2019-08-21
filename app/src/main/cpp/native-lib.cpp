#include <jni.h>
#include <string>
#include "AvFFmpeg.h"


extern "C"
JNIEXPORT void JNICALL
Java_cn_kkaka_avpalyer_AvPlayer_nativeStart(JNIEnv *env, jobject instance) {


}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkaka_avpalyer_AvPlayer_nativePrepare(JNIEnv *env, jobject instance, jstring dataSource_) {
    const char *dataSource = env->GetStringUTFChars(dataSource_, 0);
    JavaCallHelper *javaCallHelper = new JavaCallHelper();
    AvFFmpeg *fFmpeg = new AvFFmpeg(javaCallHelper, const_cast<char *>(dataSource));
    fFmpeg->prepare();
    env->ReleaseStringUTFChars(dataSource_, dataSource);
}
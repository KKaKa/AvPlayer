#include <jni.h>
#include <string>
#include "AvFFmpeg.h"
#include <android/native_window_jni.h>

JavaVM *javaVM = 0;
JavaCallHelper *javaCallHelper = 0;
AvFFmpeg *fFmpeg = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;//静态初始化mutex
ANativeWindow *window = 0;

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    javaVM = vm;
    return JNI_VERSION_1_4;
}

//渲染
void renderFrame(uint8_t *src_data, int src_lineSize, int width, int height){
    pthread_mutex_lock(&mutex);
    if(!window){
        pthread_mutex_unlock(&mutex);
        return;
    }
    //设置窗口属性
    ANativeWindow_setBuffersGeometry(window,
                                     width,
                                     height,
                                     WINDOW_FORMAT_RGBA_8888);
    ANativeWindow_Buffer window_buffer;
    if (ANativeWindow_lock(window, &window_buffer, 0)) {
        ANativeWindow_release(window);
        window = 0;
        pthread_mutex_unlock(&mutex);
        return;
    }
    //填充rgb数据给dst_data
    uint8_t *dst_data = static_cast<uint8_t *>(window_buffer.bits);
    int dst_lineSize = window_buffer.stride * 4;

    for(int i=0;i<window_buffer.height;i++){
        memcpy(dst_data + i * dst_lineSize,src_data + i *src_lineSize,dst_lineSize);
    }
    ANativeWindow_unlockAndPost(window);
    pthread_mutex_unlock(&mutex);
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkaka_avpalyer_AvPlayer_nativePrepare(JNIEnv *env, jobject instance, jstring dataSource_) {
    const char *dataSource = env->GetStringUTFChars(dataSource_, 0);
    javaCallHelper = new JavaCallHelper(javaVM,env,instance);
    fFmpeg = new AvFFmpeg(javaCallHelper, const_cast<char *>(dataSource));
    fFmpeg->setRenderCallback(renderFrame);
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
    pthread_mutex_lock(&mutex);
    //先释放之前的显示窗口
    if(window){
        ANativeWindow_release(window);
        window = 0;
    }
    window = ANativeWindow_fromSurface(env,surface);
    pthread_mutex_unlock(&mutex);
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkaka_avpalyer_AvPlayer_nativePause(JNIEnv *env, jobject instance) {
    if(fFmpeg){
        fFmpeg->pause();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkaka_avpalyer_AvPlayer_nativeRestart(JNIEnv *env, jobject instance) {
    if(fFmpeg){
        fFmpeg->reStart();
    }
}

extern "C"
JNIEXPORT jint JNICALL
Java_cn_kkaka_avpalyer_AvPlayer_nativeGetDuration(JNIEnv *env, jobject instance) {
    if(fFmpeg){
        return fFmpeg->getDuration();
    }
    return 0;
}
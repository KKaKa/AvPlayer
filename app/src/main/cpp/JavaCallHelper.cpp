//
// Created by Administrator on 2019/8/21 0021.
//


#include "JavaCallHelper.h"


JavaCallHelper::JavaCallHelper(JavaVM *javaVM_,JNIEnv *env_, jobject instance_) {
    this->javaVM = javaVM_;
    this->env = env_;
    //不可以直接赋值 涉及到jobject跨方法、跨线程，需要创建全局引用
    this->instance = env->NewGlobalRef(instance_);
    jclass clazz = env->GetObjectClass(instance);
    jmd_onPrepared = env->GetMethodID(clazz,"onPrepared","()V");
    jmd_onError = env->GetMethodID(clazz,"onError","(I)V");
    jmd_onProgress = env_->GetMethodID(clazz,"onProgress","(I)V");
}

JavaCallHelper::~JavaCallHelper() {
    javaVM = 0;
    env->DeleteGlobalRef(instance);
    instance = 0;
}

void JavaCallHelper::onPrepare(int threadMode) {
    //涉及到跨线程 需要javaVM
    if(threadMode == THREAD_MAIN){
        env->CallVoidMethod(instance,jmd_onPrepared);
    }else{
        //通过JavaVM的AttachCurrentThread方法来获取到当前线程中的JNIEnv指针
        JNIEnv *env_child;
        javaVM->AttachCurrentThread(&env_child,0);
        env_child->CallVoidMethod(instance,jmd_onPrepared);
        javaVM->DetachCurrentThread();
    }
}

void JavaCallHelper::onError(int threadMode, int errorCode) {
    if(threadMode == THREAD_MAIN){
        env->CallVoidMethod(instance,jmd_onError,errorCode);
    }else{
        JNIEnv *env_child;
        javaVM->AttachCurrentThread(&env_child,0);
        env_child->CallVoidMethod(instance,jmd_onError,errorCode);
        javaVM->DetachCurrentThread();
    }

}

void JavaCallHelper::onProgress(int threadMode, int progress) {
    if(threadMode == THREAD_MAIN){
        env->CallVoidMethod(instance,jmd_onProgress,progress);
    }else{
        JNIEnv *env_child;
        javaVM->AttachCurrentThread(&env_child,0);
        env_child->CallVoidMethod(instance,jmd_onProgress,progress);
        javaVM->DetachCurrentThread();
    }
}

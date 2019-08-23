//
// Created by Administrator on 2019/8/21 0021.
//

#ifndef AVPALYER_JAVACALLHELPER_H
#define AVPALYER_JAVACALLHELPER_H

#include <jni.h>
#include "macro.h"

class JavaCallHelper {
public:
    JavaCallHelper(JavaVM *javaVM_,JNIEnv *env_, jobject instance_);

    virtual ~JavaCallHelper();

    void onPrepare(int threadMode);

private:
    JavaVM *javaVM;
    JNIEnv *env;
    jobject instance;
    jmethodID jmd_onPrepared;
};


#endif //AVPALYER_JAVACALLHELPER_H

//
// Created by Administrator on 2019/8/22 0022.
//

#ifndef AVPALYER_MACRO_H
#define AVPALYER_MACRO_H

#include <android/log.h>

/**
 * 表主线程
 */
#define THREAD_MAIN 1
/**
 * 表子线程
 */
#define THREAD_CHILD 2

/**
 * 释放申请的内存
 */
#define DELETE(object) if(object){delete object; object = 0;}

/**
 * 定义日志打印宏函数
 */
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "NEFFMPEG",__VA_ARGS__)

#endif //AVPALYER_MACRO_H

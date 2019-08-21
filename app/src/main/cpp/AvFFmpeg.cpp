//
// Created by Administrator on 2019/8/21 0021.
//

#include "AvFFmpeg.h"

AvFFmpeg::AvFFmpeg(JavaCallHelper *javaCallHelper,char * dataSource) {
    this->javaCallHelper = javaCallHelper;
    this->dataSource = dataSource;//这样会导致悬空指针 要用内存拷贝
    this->dataSource = new char[strlen(dataSource + 1)];//C 字符串以\0结尾
    strcpy(this->dataSource, dataSource);
}

AvFFmpeg::~AvFFmpeg() {
    DELETE(dataSource);
    DELETE(javaCallHelper);
}

/**
 * 播放准备
 */
void AvFFmpeg::prepare() {

}

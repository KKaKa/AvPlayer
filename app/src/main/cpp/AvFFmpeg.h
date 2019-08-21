//
// Created by Administrator on 2019/8/21 0021.
//

#ifndef AVPALYER_AVFFMPEG_H
#define AVPALYER_AVFFMPEG_H

#include "JavaCallHelper.h"
#include "AudioChannel.h"
#include "VideoChannel.h"
#include <cstring>
#include "macro.h"

class AvFFmpeg {
public:
    AvFFmpeg(JavaCallHelper *javaCallHelper,char * dataSource);

    ~AvFFmpeg();

    void prepare();

private:
    JavaCallHelper *javaCallHelper = 0;
    AudioChannel *audioChannel = 0;
    VideoChannel *videoChannel = 0;
    char * dataSource;
};


#endif //AVPALYER_AVFFMPEG_H

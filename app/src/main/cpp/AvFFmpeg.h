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
#include <pthread.h>
extern "C"{
#include <libavformat/avformat.h>
};

class AvFFmpeg {
public:
    AvFFmpeg(JavaCallHelper *javaCallHelper,char * dataSource);

    ~AvFFmpeg();

    void prepare();

    void _prepare();

    void start();

    void _start();

    void setRenderCallback(RenderCallback callback);

private:
    JavaCallHelper *javaCallHelper = 0;
    AudioChannel *audioChannel = 0;
    VideoChannel *videoChannel = 0;
    char * dataSource;
    pthread_t pid_prepare;
    pthread_t pid_start;
    AVFormatContext *formatContext = 0;
    bool isPlaying = 0;
    RenderCallback renderCallback;
};


#endif //AVPALYER_AVFFMPEG_H

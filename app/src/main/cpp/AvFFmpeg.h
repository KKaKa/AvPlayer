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
    friend void *task_stop(void *args);
public:
    AvFFmpeg(JavaCallHelper *javaCallHelper,char * dataSource);

    ~AvFFmpeg();

    void prepare();

    void _prepare();

    void start();

    void _start();

    void pause();

    void reStart();

    void stop();

    void _stop();

    void setRenderCallback(RenderCallback callback);

    int getDuration() const;

    void seekTo(int progress);

    void _seekTo(int progress);

private:
    JavaCallHelper *javaCallHelper = 0;
    AudioChannel *audioChannel = 0;
    VideoChannel *videoChannel = 0;
    char * dataSource;
    pthread_t pid_prepare;
    pthread_t pid_start;
    pthread_t pid_seek;
    pthread_t pid_stop;
    AVFormatContext *formatContext = 0;
    bool isPlaying = 0;
    RenderCallback renderCallback;
    int duration;
    pthread_mutex_t seekMutex;
};


#endif //AVPALYER_AVFFMPEG_H

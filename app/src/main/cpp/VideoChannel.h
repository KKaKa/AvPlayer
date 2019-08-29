//
// Created by Administrator on 2019/8/21 0021.
//

#ifndef AVPALYER_VIDEOCHANNEL_H
#define AVPALYER_VIDEOCHANNEL_H


#include "BaseChannel.h"
extern "C"{
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>

}

typedef void (*RenderCallback)(uint8_t *, int , int ,int);

class VideoChannel : public BaseChannel{
public:
    VideoChannel(int id,AVCodecContext *codecContext,int fps);

    ~VideoChannel();

    void start();

    void pause();

    void reStart();

    void stop();

    void video_decode();

    void video_play();

    void setRenderCallback(RenderCallback callback);

private:
    pthread_t pid_video_play;
    pthread_t pid_video_decode;
    RenderCallback renderCallback;
    int fps;
};


#endif //AVPALYER_VIDEOCHANNEL_H

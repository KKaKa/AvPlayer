//
// Created by Administrator on 2019/8/21 0021.
//

#ifndef AVPALYER_VIDEOCHANNEL_H
#define AVPALYER_VIDEOCHANNEL_H


#include "BaseChannel.h"

class VideoChannel : public BaseChannel{
public:
    VideoChannel(int id);

    ~VideoChannel();

    void start();

    void video_decode();

    void stop();

private:

    pthread_t pid_video_decode;
};


#endif //AVPALYER_VIDEOCHANNEL_H

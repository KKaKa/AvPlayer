//
// Created by Administrator on 2019/8/21 0021.
//

#ifndef AVPALYER_AUDIOCHANNEL_H
#define AVPALYER_AUDIOCHANNEL_H


#include "BaseChannel.h"

class AudioChannel : public BaseChannel{
public:
    AudioChannel(int id);

    ~AudioChannel();

    void start();

    void stop();

};


#endif //AVPALYER_AUDIOCHANNEL_H

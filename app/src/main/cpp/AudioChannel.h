//
// Created by Administrator on 2019/8/21 0021.
//

#ifndef AVPALYER_AUDIOCHANNEL_H
#define AVPALYER_AUDIOCHANNEL_H


#include "BaseChannel.h"
extern "C"{
#include <libswresample/swresample.h>
};
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "macro.h"

class AudioChannel : public BaseChannel{
public:
    AudioChannel(int id,AVCodecContext *codecContext,AVRational time_base,JavaCallHelper *javaCallHelper);

    ~AudioChannel();

    void start();

    void pause();

    void reStart();

    void stop();

    void audio_decode();

    void audio_play();

    int getPCM();

    int out_channel;
    int out_sample_size;
    int out_sampleRate;
    int out_buff_size;
    uint8_t * out_buff = 0;
private:
    SwrContext *swrContext = 0;
    pthread_t pid_audio_decode;
    pthread_t pid_audio_play;

    //引擎
    SLObjectItf engineObject = 0;
    //引擎接口
    SLEngineItf engineInterface = 0;
    //混音器
    SLObjectItf outputMixObject = 0;
    //播放器
    SLObjectItf bqPlayerObject = 0;
    //播放器接口
    SLPlayItf bqPlayerPlayInterface =0;
    //播放器队列接口
    SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue = 0;

};


#endif //AVPALYER_AUDIOCHANNEL_H

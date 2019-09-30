//
// Created by Administrator on 2019/8/21 0021.
//

#ifndef AVPALYER_BASECHANNEL_H
#define AVPALYER_BASECHANNEL_H

extern "C"{
#include <libavcodec/avcodec.h>
#include <libavutil/time.h>
};

#include "safe_queue.h"
#include "JavaCallHelper.h"

/**
 * Video Audio 的父类
 */
class BaseChannel {
public:
    BaseChannel(int id, AVCodecContext *codecContext, AVRational time_base,JavaCallHelper *javaCallHelper) : id(id), codecContext(
            codecContext), time_base(time_base), javaCallHelper(javaCallHelper){

        packets.setReleaseCallback(releasePacket);
        frames.setReleaseCallback(releaseFrame);
    }

    virtual ~BaseChannel() {
        packets.clear();
        frames.clear();
        if(codecContext){
            avcodec_close(codecContext);
            avcodec_free_context(&codecContext);
            codecContext = 0;
        }
    }

    /**
     * 释放packet
     * @param packet
     */
    static void releasePacket(AVPacket **packet){
        if(packet){
            av_packet_free(packet);
            *packet = 0;
        }
    }

    static void releaseFrame(AVFrame **frame){
        if(frame){
            av_frame_free(frame);
            *frame = 0;
        }
    }

    int id;
    AVCodecContext *codecContext;

    //纯虚函数（抽象方法）
    virtual void start() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;
    virtual void reStart() = 0;

    //视频编码数据包队列
    SafeQueue<AVPacket *> packets;
    SafeQueue<AVFrame *> frames;
    AVRational time_base;
    bool isPlaying = 0;
    bool isPause = 0;
    double audio_time;
    JavaCallHelper *javaCallHelper = 0;
};


#endif //AVPALYER_BASECHANNEL_H

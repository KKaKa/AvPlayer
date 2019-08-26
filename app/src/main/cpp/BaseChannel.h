//
// Created by Administrator on 2019/8/21 0021.
//

#ifndef AVPALYER_BASECHANNEL_H
#define AVPALYER_BASECHANNEL_H

extern "C"{
#include <libavcodec/avcodec.h>
};

#include "safe_queue.h"

/**
 * Video Audio 的父类
 */
class BaseChannel {
public:
    BaseChannel(int id,AVCodecContext *codecContext) : id(id),codecContext(codecContext){
        packets.setReleaseCallback(releasePacket);
        frames.setReleaseCallback(releaseFrame);
    }

    virtual ~BaseChannel() {

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
    virtual void stop() = 0;

    //视频编码数据包队列
    SafeQueue<AVPacket *> packets;
    SafeQueue<AVFrame *> frames;

    bool isPlaying = 0;
};


#endif //AVPALYER_BASECHANNEL_H

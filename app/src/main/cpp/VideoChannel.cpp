//
// Created by Administrator on 2019/8/21 0021.
//

#include "VideoChannel.h"

VideoChannel::VideoChannel(int id) : BaseChannel(id){}

VideoChannel::~VideoChannel() {

}

void *task_video_decode(void *args){
    VideoChannel *videoChannel = static_cast<VideoChannel *>(args);
    videoChannel->video_decode();
    return 0;
};


void VideoChannel::start() {
    isPlaying = 1;
    //设置队列为可执行状态
    packets.setWork(1);
    //现在拿到的为packet 需要解码 需要开子线程zhixing
    pthread_create(&pid_video_decode,0,task_video_decode,this);

}

void VideoChannel::stop() {

}

/**
 * 真正视频解码
 */
void VideoChannel::video_decode() {
    AVPacket *packet = 0;
    //循环把数据包给解码器进行解码
    while(isPlaying){

    }
    //释放packets
    releasePaclket(&packet);
}





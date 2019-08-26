//
// Created by Administrator on 2019/8/21 0021.
//

#include "VideoChannel.h"


VideoChannel::VideoChannel(int id,AVCodecContext *codecContext) : BaseChannel(id,codecContext){}

VideoChannel::~VideoChannel() {

}

void *task_video_decode(void *args){
    VideoChannel *videoChannel = static_cast<VideoChannel *>(args);
    videoChannel->video_decode();
    return 0;
};

void *task_video_play(void *args){
    VideoChannel *videoChannel = static_cast<VideoChannel *>(args);
    videoChannel->video_play();
    return 0;
}


void VideoChannel::start() {
    isPlaying = 1;
    //设置队列为可执行状态
    packets.setWork(1);
    frames.setWork(1);
    //现在拿到的为packet 需要解码 需要开子线程执行
    pthread_create(&pid_video_decode,0,task_video_decode,this);
    //播放
    pthread_create(&pid_video_play,0,task_video_play,this);
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
        if(!isPlaying){
            break;
        }
        int ret = packets.pop(packet);
        if(!ret){
            continue;
        }
        //拿到了视频数据包（编码压缩了的），需要把数据包给解码器进行解码
        ret = avcodec_send_packet(codecContext,packet);
        if(ret == AVERROR(EAGAIN)){
            continue;
        } else if(ret){
           //往解码器发送数据包失败
            break;
        }
        //释放packet 后续不需要了
        releasePacket(&packet);
        //拿出解码后的frame
        AVFrame *frame = av_frame_alloc();
        ret = avcodec_receive_frame(codecContext,frame);
        if(ret == AVERROR(EAGAIN)){
            continue;
        } else if(ret){
            break;
        }
        //往frame队列添加
        frames.push(frame);
    }//end while
    //释放packets
    releasePacket(&packet);
}

/**
 * 真正视频播放
 */
void VideoChannel::video_play() {
    AVFrame *frame = 0;
    //对原始数据进行格式转换: yuv: 400x800 > rgba: 400x800
    SwsContext *sws_context = sws_getContext(
                codecContext->width,
                codecContext->height,
                codecContext->pix_fmt,
                codecContext->width,
                codecContext->height,
                AV_PIX_FMT_RGBA,
                SWS_BILINEAR,
                NULL,NULL,NULL);
    uint8_t *dst_data[4];
    int dst_linesize[4];
    //给 dst_data dst_linesize 申请内存
    av_image_alloc(dst_data,dst_linesize,codecContext->width,codecContext->height,AV_PIX_FMT_RGBA,1);
    while (isPlaying){
        int ret = frames.pop(frame);
        sws_scale(sws_context,frame->data,frame->linesize,0,codecContext->height,dst_data,dst_linesize);
        //需要回调回surface渲染 回调出去> native-lib里
        renderCallback(dst_data[0],dst_linesize[0],codecContext->width,codecContext->width);
        releaseFrame(&frame);
    }
    releaseFrame(&frame);
    isPlaying = 0;
    av_freep(&dst_data[0]);
    av_freep(&dst_linesize[0]);
    sws_freeContext(sws_context);
    avcodec_free_context(&codecContext);
}

void VideoChannel::setRenderCallback(RenderCallback callback) {
    this->renderCallback = callback;
}





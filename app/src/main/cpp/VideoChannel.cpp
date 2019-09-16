//
// Created by Administrator on 2019/8/21 0021.
//

#include "VideoChannel.h"

void dropAVPacket(queue<AVPacket *> &q){
    while(!q.empty()){
        // I 帧、B 帧、 P 帧
        //不能丢 I 帧,AV_PKT_FLAG_KEY: I帧（关键帧）
        AVPacket *packet = q.front();
        if(packet->flags != AV_PKT_FLAG_KEY){
            //不是关键帧
            BaseChannel::releasePacket(&packet);
            q.pop();
        }else{
            break;
        }
    }
}

void dropFrame(queue<AVFrame *> &q){
    if(!q.empty()){
        AVFrame *avFrame = q.front();
        BaseChannel::releaseFrame(&avFrame);
        q.pop();
    }
}

VideoChannel::VideoChannel(int id,AVCodecContext *codecContext,int fps,AVRational time_base,JavaCallHelper *javaCallHelper) : BaseChannel(id,codecContext,time_base,javaCallHelper){
    this->fps = fps;
    packets.setSyncHandle(dropAVPacket);
    frames.setSyncHandle(dropFrame);
}

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

void VideoChannel::pause() {
    isPause = 1;
}

void VideoChannel::reStart() {
    isPause = 0;
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
        //这里要控制下frame队列 以免内存泄漏
        while (isPlaying && frames.size() > 100){
            av_usleep(10 * 1000);
            continue;
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

//    while (isPause){
//        av_usleep(1000);
//    }

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
        if(!isPlaying){
            //如果停止播放了，跳出循环 释放packet
            return;
        }

        if(isPause){
            av_usleep(1000);
            continue;
        }

        int ret = frames.pop(frame);
        if(!ret){
            continue;
        }
        sws_scale(sws_context,frame->data,frame->linesize,0,codecContext->height,dst_data,dst_linesize);

        //每一帧还有自己的额外延时时间 根据fps（传入的流的平均帧率来控制每一帧的延时时间） sleep : fps > 时间
        double extra_delay = frame->repeat_pict / (2 * fps);
        //单位是 : 秒
        double delay_time_per_frame = 1.0 / fps;
        double real_delay = delay_time_per_frame + extra_delay;
//        av_usleep(real_delay * 1000000); 不再以视频播放规则来播放

        //音视频同步 以音频时间为基准time_base 获取视频时间
        double video_time = frame->best_effort_timestamp *av_q2d(time_base);

        if(!audioChannel){
            //没有音频的情况
            av_usleep(real_delay * 1000000);
            //没有音频的情况下 进度以视频进度为准
            if(javaCallHelper){
                javaCallHelper->onProgress(THREAD_CHILD,video_time);
            }
        }else{
            double audio_time = audioChannel->audio_time;
            //获取音视频播放的时间差
            double time_diff = video_time - audio_time;
            if(time_diff > 0){
                //视频比音频快
                //视频比音频快：等音频（sleep）
                //自然播放状态下，time_diff的值不会很大
                //但是，seek后time_diff的值可能会很大，导致视频休眠太久

                if(time_diff > 1){
                    //让音频慢慢追赶
                    av_usleep((real_delay * 2) * 1000000);
                }else{
                    av_usleep((time_diff + time_diff) * 1000000);
                }
            }else if(time_diff < 0){
                //音频比视频快
                //追音频（尝试丢视频包）
                //时间差如果大于0.05，有明显的延迟感
                if(fabs(time_diff) >= 0.05){
                    //丢包 操作队列 小心
                    frames.sync();
                }
            }else{
                //完美同步
                LOGE("完美同步");
            }
        }


        //需要回调回surface渲染 回调出去> native-lib里
        renderCallback(dst_data[0],dst_linesize[0],codecContext->width,codecContext->height);
        releaseFrame(&frame);
    }
    releaseFrame(&frame);
    isPlaying = 0;
    av_freep(&dst_data[0]);
    av_freep(&dst_linesize[0]);
}

void VideoChannel::setRenderCallback(RenderCallback callback) {
    this->renderCallback = callback;
}

void VideoChannel::setAudioChannel(AudioChannel *audioChannel) {
    this->audioChannel = audioChannel;
}







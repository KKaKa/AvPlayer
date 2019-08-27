//
// Created by Administrator on 2019/8/21 0021.
//
#include "AvFFmpeg.h"

AvFFmpeg::AvFFmpeg(JavaCallHelper *javaCallHelper,char * dataSource) {
    this->javaCallHelper = javaCallHelper;
    this->dataSource = dataSource;//这样会导致悬空指针 要用内存拷贝
    this->dataSource = new char[strlen(dataSource + 1)];//C 字符串以\0结尾
    strcpy(this->dataSource, dataSource);
}

AvFFmpeg::~AvFFmpeg() {
    DELETE(dataSource);
    DELETE(javaCallHelper);
}

/**
 * pid_prepare 真正执行的函数
 * @param args
 * @return
 */
void *task_prepare(void *args){
    //打开输出
    AvFFmpeg *fFmpeg = static_cast<AvFFmpeg *>(args);
    fFmpeg->_prepare();
    return 0;
}

/**
 * pid_start 真正执行的函数
 * @param args
 * @return
 */
void *task_start(void *args){
    AvFFmpeg *fFmpeg = static_cast<AvFFmpeg *>(args);
    fFmpeg->_start();
    return 0;
}

/**
 * 播放准备
 */
void AvFFmpeg::prepare() {
    //不能直接调用解码api 存在io 网络等的线程问题 需要创建子线程来操作
    pthread_create(&pid_prepare, 0, task_prepare, this);
}

void AvFFmpeg::start() {
    isPlaying = 1;
    if(videoChannel){
        videoChannel->start();
    }
    if(audioChannel){
        audioChannel->start();
    }
    //一样在子线程中执行
    pthread_create(&pid_start,0,task_start,this);
}

void AvFFmpeg::_prepare() {
    formatContext = avformat_alloc_context();
    AVDictionary *dictionary = 0;
    //设置超时时间 10秒 单位微秒
    av_dict_set(&dictionary,"timeout","10000000",0);

    //打开媒体 返回0为success
    int ret = avformat_open_input(&formatContext, dataSource, 0, &dictionary);
    av_dict_free(&dictionary);
    if(ret){
        LOGE("打开流媒体失败：%s",av_err2str(ret));
        ERROR_CALLBACK(javaCallHelper,THREAD_CHILD,FFMPEG_CAN_NOT_OPEN_URL);
        return;
    }

    //查找媒体中的流信息
    ret = avformat_find_stream_info(formatContext,0);
    if(ret < 0){
        LOGE("查找流媒体信息失败：%s",av_err2str(ret));
        ERROR_CALLBACK(javaCallHelper,THREAD_CHILD,FFMPEG_CAN_NOT_FIND_STREAMS);
        return;
    }

    for (int i = 0; i < formatContext->nb_streams; ++i) {
        //获取媒体流(音频或者视频)
        AVStream *stream = formatContext->streams[i];
        //获取编解码这段流的参数
        AVCodecParameters *codecParameters = stream->codecpar;
        //通过参数中的id（编解码的方式），来查找当前流的解码器
        AVCodec *codec = avcodec_find_decoder(codecParameters->codec_id);
        if(!codec){
            LOGE("查找当前流的解码器失败");
            ERROR_CALLBACK(javaCallHelper,THREAD_CHILD,FFMPEG_FIND_DECODER_FAIL);
            return;
        }
        //创建解码器上下文
        AVCodecContext *codecContext = avcodec_alloc_context3(codec);
        if(!codecContext){
            LOGE("创建当前流的解码器上下文失败");
            ERROR_CALLBACK(javaCallHelper,THREAD_CHILD,FFMPEG_ALLOC_CODEC_CONTEXT_FAIL);
            return;
        }
        //设置解码器上下文参数
        ret = avcodec_parameters_to_context(codecContext,codecParameters);
        if(ret <0){
            LOGE("设置解码器上下文参数失败：%s",av_err2str(ret));
            ERROR_CALLBACK(javaCallHelper,THREAD_CHILD,FFMPEG_CODEC_CONTEXT_PARAMETERS_FAIL);
            return;
        }
        //打开解码器
        ret = avcodec_open2(codecContext,codec,0);
        if(ret){
            LOGE("打开解码器失败：%s",av_err2str(ret));
            ERROR_CALLBACK(javaCallHelper,THREAD_CHILD,FFMPEG_OPEN_DECODER_FAIL);
            return;
        }
        //判断是视频流还是音频流
        if(codecParameters->codec_type == AVMEDIA_TYPE_VIDEO){
            //视频流
            AVRational frame_rate = stream->avg_frame_rate;
            int fps = static_cast<int>(av_q2d(frame_rate));
            videoChannel = new VideoChannel(i,codecContext,fps);
            videoChannel->setRenderCallback(renderCallback);
        }else if(codecParameters->codec_type == AVMEDIA_TYPE_AUDIO){
            //音频流
            audioChannel = new AudioChannel(i,codecContext);
        }
    }//end for
    if(!audioChannel && !videoChannel){
        LOGE("没有音视频");
        ERROR_CALLBACK(javaCallHelper,THREAD_CHILD,FFMPEG_NOMEDIA);
        return;
    }

    if(javaCallHelper){
        javaCallHelper->onPrepare(THREAD_CHILD);
    }
}

void AvFFmpeg::_start() {
    //这是一个循环读取的过程
    while (isPlaying){
        //这里要控制下packets队列 以免内存泄漏
        if(videoChannel && videoChannel->packets.size() > 100){
            av_usleep(10 * 1000);
            continue;
        }
        if(audioChannel && audioChannel->packets.size() > 100){
            av_usleep(10 * 1000);
            continue;
        }

        AVPacket *packet =av_packet_alloc();
        int ret = av_read_frame(formatContext,packet);
        // 0 为成功 false
        if(!ret){
            //判断是视频流还是音频流
            if(videoChannel && packet->stream_index == videoChannel->id){
                //往编码数据包队列添加数据
                videoChannel->packets.push(packet);
            } else if(audioChannel && packet->stream_index == audioChannel->id){
                //往音频编码数据包队列中添加数据
                audioChannel->packets.push(packet);
            }
        } else if(ret == AVERROR_EOF){
            //TODO 表示读完了
            //要考虑读完了，是否播完了的情况
        } else{
            LOGE("读取数据包失败");
            ERROR_CALLBACK(javaCallHelper,THREAD_CHILD,FFMPEG_READ_PACKETS_FAIL);
            break;
        }
    }
    isPlaying = 0;
    //停止解码播放（音频和视频）
    videoChannel->stop();
    audioChannel->stop();
}

void AvFFmpeg::setRenderCallback(RenderCallback callback) {
    this->renderCallback = callback;
}



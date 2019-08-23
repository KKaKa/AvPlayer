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
 * 播放准备
 */
void AvFFmpeg::prepare() {
    //不能直接调用解码api 存在io 网络等的线程问题 需要创建子线程来操作
    pthread_create(&pid_prepare, 0, task_prepare, this);
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
        //TODO 回调给javaCallback
        return;
    }

    //查找媒体中的流信息
    ret = avformat_find_stream_info(formatContext,0);
    if(ret < 0){
        LOGE("查找流媒体信息失败：%s",av_err2str(ret));
        //TODO 回调给javaCallback
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
            //TODO 回调给javaCallback
            return;
        }
        //创建解码器上下文
        AVCodecContext *codecContext = avcodec_alloc_context3(codec);
        if(!codecContext){
            LOGE("创建当前流的解码器上下文失败");
            //TODO 回调给javaCallback
            return;
        }
        //设置解码器上下文参数
        ret = avcodec_parameters_to_context(codecContext,codecParameters);
        if(ret <0){
            LOGE("设置解码器上下文参数失败：%s",av_err2str(ret));
            //TODO 回调给javaCallback
            return;
        }
        //打开解码器
        ret = avcodec_open2(codecContext,codec,0);
        if(ret){
            LOGE("设打开解码器失败：%s",av_err2str(ret));
            //TODO 回调给javaCallback
            return;
        }
        //判断是视频流还是音频流
        if(codecParameters->codec_type == AVMEDIA_TYPE_VIDEO){
            //视频流
            videoChannel = new VideoChannel(i);
        }else if(codecParameters->codec_type == AVMEDIA_TYPE_AUDIO){
            //音频流
            audioChannel = new AudioChannel(i);
        }
    }//end for
    if(!audioChannel && !videoChannel){
        LOGE("没有音视频");
        //TODO 回调给javaCallback
        return;
    }

    if(javaCallHelper){
        javaCallHelper->onPrepare(THREAD_CHILD);
    }
}


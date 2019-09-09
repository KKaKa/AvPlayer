//
// Created by Administrator on 2019/8/21 0021.
//

#include "AudioChannel.h"

AudioChannel::AudioChannel(int id,AVCodecContext *codecContext,AVRational time_base) : BaseChannel(id,codecContext,time_base) {
    //定义缓冲区:通道数 字节数 采样率
    out_channel = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);//双通道
    out_sample_size = av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
    out_sampleRate = 44100;
    // 2(通道数) * 2（16bit=2字节）*44100（采样率）
    out_buff_size = out_channel * out_sample_size * out_sampleRate;
    out_buff = static_cast<uint8_t *>(malloc(out_buff_size));
    //作用是将某一块内存中的内容全部设置为指定的值， 这个函数通常为新申请的内存做初始化工作
    memset(out_buff,0,out_buff_size);

    //放这里处理 不在getPCM中 防止内存过高
    swrContext = swr_alloc_set_opts(0, AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16,
                                                out_sampleRate,
                                                codecContext->channel_layout,
                                                codecContext->sample_fmt,
                                                codecContext->sample_rate,
                                                0, 0);
    //初始化重采样上下文
    swr_init(swrContext);
}

AudioChannel::~AudioChannel() {
    if(swrContext){
        swr_free(&swrContext);
        swrContext = 0;
    }
    DELETE(out_buff);
}

void *task_audio_decode(void *args){
    AudioChannel *audioChannel = static_cast<AudioChannel *>(args);
    audioChannel->audio_decode();
    return 0;
}

void *task_audio_play(void *args){
    AudioChannel *audioChannel = static_cast<AudioChannel *>(args);
    audioChannel->audio_play();
    return 0;
}

void AudioChannel::start() {
    isPlaying = 1;
    //设置队列为可执行状态
    packets.setWork(1);
    frames.setWork(1);
    //现在拿到的为packet 需要解码 需要开子线程执行
    pthread_create(&pid_audio_decode,0,task_audio_decode,this);
    //播放
    pthread_create(&pid_audio_play,0,task_audio_play,this);
}

void AudioChannel::pause() {
    //设置队列为不可执行状态
    packets.setWork(0);
    frames.setWork(0);
}

void AudioChannel::reStart() {
    packets.setWork(1);
    frames.setWork(1);
}

void AudioChannel::stop() {

}

/**
 * 解码
 */
void AudioChannel::audio_decode() {
    AVPacket *avPacket = 0;
    while(isPlaying){
        if(!isPlaying){
            break;
        }
        int ret = packets.pop(avPacket);
        if(!ret){
            continue;
        }
        //拿到了视频数据包（编码压缩了的），需要把数据包给解码器进行解码
        ret = avcodec_send_packet(codecContext,avPacket);
        if(ret == AVERROR(EAGAIN)){
            continue;
        } else if(ret){
            //往解码器发送数据包失败
            break;
        }
        //释放packet 后续不需要了
        releasePacket(&avPacket);
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
        //往frame队列添加PCM数据
        frames.push(frame);
    }//end while
    //释放packets
    releasePacket(&avPacket);
}

/**
 * 创建回调函数
 * @param bq
 * @param context
 */
void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context){
    AudioChannel *audioChannel = static_cast<AudioChannel *>(context);
    int pcm_size = audioChannel->getPCM();
    if(pcm_size > 0){
        (*bq)->Enqueue(bq,audioChannel->out_buff,pcm_size);
    }
}

/**
 * 播放
 * 1.创建引擎并获取引擎接口
 * 2.设置混音器
 * 3.创建播放器
 * 4.设置播放回调函数
 * 5、设置播放器状态为播放状态
 * 6、手动激活回调函数
 */
void AudioChannel::audio_play() {
    SLresult result;
    //1.1 创建引擎对象：SLObjectItf engineObject
    result = slCreateEngine(&engineObject,0,NULL,0,NULL,NULL);
    if(SL_RESULT_SUCCESS != result){
        LOGE("创建引擎对象失败");
        return;
    }
    //1.2 初始化引擎
    result = (*engineObject)->Realize(engineObject,SL_BOOLEAN_FALSE);
    if(SL_RESULT_SUCCESS != result){
        LOGE("初始化引擎失败");
        return;
    }
    //1.3 获取引擎接口 SLEngineItf engineInterface
    result = (*engineObject)->GetInterface(engineObject,SL_IID_ENGINE,&engineInterface);
    if(SL_RESULT_SUCCESS != result){
        LOGE("获取引擎接口失败");
        return;
    }

    //2.1 创建混音器：SLObjectItf outputMixObject
    result = (*engineInterface)->CreateOutputMix(engineInterface,&outputMixObject,0,0,0);
    if(SL_RESULT_SUCCESS != result){
        LOGE("创建混音器失败");
        return;
    }
    //2.2 初始化混音器
    result = (*outputMixObject)->Realize(outputMixObject,SL_BOOLEAN_FALSE);
    if(SL_RESULT_SUCCESS != result){
        LOGE("初始化混音器失败");
        return;
    }
    //2.3 创建播放器 配置输入声音信息 创建buffer缓冲类型的队列 2个队列
    SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,2};
    //pcm数据格式
    //SL_DATAFORMAT_PCM：数据格式为pcm格式
    //2：双声道
    //SL_SAMPLINGRATE_44_1：采样率为44100
    //SL_PCMSAMPLEFORMAT_FIXED_16：采样格式为16bit
    //SL_PCMSAMPLEFORMAT_FIXED_16：数据大小为16bit
    //SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT：左右声道（双声道）
    //SL_BYTEORDER_LITTLEENDIAN：小端模式
    SLDataFormat_PCM format_pcm = {
            SL_DATAFORMAT_PCM,
            2,
            SL_SAMPLINGRATE_44_1,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
            SL_BYTEORDER_LITTLEENDIAN
    };
    //2.4 数据源 将上述配置信息放到这个数据源中
    SLDataSource audioSrc = {&loc_bufq,&format_pcm};
    //2.5 配置音轨（输出）设置混音器(1.定位器类型 2.输出混合)
    SLDataLocator_OutputMix loc_out = {SL_DATALOCATOR_OUTPUTMIX,outputMixObject};
    //输出
    SLDataSink audioSink = {
            &loc_out,
            NULL
    };

    //3.1 创建播放器 需要的接口
    const SLInterfaceID interfaceIDs[1] = {
            SL_IID_BUFFERQUEUE
    };
    const SLboolean requiredInterfaces[1] = {
            SL_BOOLEAN_TRUE
    };

    //3.2 创建播放器
    result = (*engineInterface)->CreateAudioPlayer(
            engineInterface,
            &bqPlayerObject,
            &audioSrc,
            &audioSink,
            1,
            interfaceIDs,
            requiredInterfaces);
    if(SL_RESULT_SUCCESS != result){
        LOGE("播放器创建失败");
        return;
    }
    //3.3 初始化播放器：SLObjectItf bqPlayerObject
    result = (*bqPlayerObject)->Realize(bqPlayerObject,SL_BOOLEAN_FALSE);
    if(SL_RESULT_SUCCESS != result){
        LOGE("初始化播放器失败");
        return;
    }
    //3.4 获取播放器接口：SLPlayItf bqPlayerPlay
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject,SL_IID_PLAY,&bqPlayerPlayInterface);
    if(SL_RESULT_SUCCESS != result){
        LOGE("获取播放器接口失败");
        return;
    }

    //4.1 获取播放器队列接口：SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue
    (*bqPlayerObject)->GetInterface(bqPlayerObject,SL_IID_BUFFERQUEUE,&bqPlayerBufferQueue);
    //4.2 设置回调 void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context)
    (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue,bqPlayerCallback,this);

    //5.1 设置播放器状态为播放状态
    (*bqPlayerPlayInterface)->SetPlayState(bqPlayerPlayInterface,SL_PLAYSTATE_PLAYING);

    //6.1 手动激活回调函数
    bqPlayerCallback(bqPlayerBufferQueue,this);
}

int AudioChannel::getPCM() {
    int pcm_data_size = 0;
    AVFrame *frame = 0;

    while (isPlaying){
        if (!isPlaying) {
            //如果停止播放了，跳出循环 释放packet
            break;
        }
        int ret = frames.pop(frame);
        if (!ret) {
            LOGE("取数据包失败");
            //取数据包失败
            continue;
        }
        LOGE("音频播放中");
        //pcm数据在 frame中
        //这里获得的解码后pcm格式的音频原始数据，有可能与创建的播放器中设置的pcm格式不一样,需要做重采样
        //重采样example:resample

        //假设输入10个数据，有可能这次转换只转换了8个，还剩2个数据（delay）

        //swr_get_delay: 下一个输入数据与下下个输入数据之间的时间间隔
        int64_t delay = swr_get_delay(swrContext,frame->sample_rate);

        //AV_ROUND_UP：向上取整
        int64_t out_max_samples = av_rescale_rnd(frame->nb_samples + delay, frame->sample_rate,
                                                 out_sampleRate, AV_ROUND_UP);

        //1. 上下文  2.输出缓冲区 3. 输出缓冲区最大容纳数据量 4.输入数据 5.输入数据量
        int out_samples = swr_convert(swrContext, &out_buff, out_max_samples,
                                      (const uint8_t **) (frame->data),
                                      frame->nb_samples);

        // 获取swr_convert转换后 out_samples个 *2 （16位）*2（双声道）
        pcm_data_size = out_samples * out_sample_size * out_channel;

        //获取音频时间 audio_time 并需要传递给videoChannel
        audio_time = frame->best_effort_timestamp *av_q2d(time_base);

        break;
    }
    releaseFrame(&frame);
    return pcm_data_size;
}



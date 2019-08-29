package cn.kkaka.avpalyer;

import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class AvPlayer implements AvPlayInterface,SurfaceHolder.Callback{

    static {
        System.loadLibrary("native-lib");
    }

    private String dataSource;
    private onPreparedListener onPreparedListener;
    private SurfaceHolder surfaceHolder;
    private onErrorListener onErrorListener;

    //准备过程错误码
    public static final int ERROR_CODE_FFMPEG_PREPARE = -1000;

    //播放过程错误码
    public static final int ERROR_CODE_FFMPEG_PLAY = -2000;

    //打不开视频
    public static final int FFMPEG_CAN_NOT_OPEN_URL = (ERROR_CODE_FFMPEG_PREPARE - 1);

    //找不到媒体流信息
    public static final int FFMPEG_CAN_NOT_FIND_STREAMS = (ERROR_CODE_FFMPEG_PREPARE - 2);

    //找不到解码器
    public static final int FFMPEG_FIND_DECODER_FAIL = (ERROR_CODE_FFMPEG_PREPARE - 3);

    //无法根据解码器创建上下文
    public static final int FFMPEG_ALLOC_CODEC_CONTEXT_FAIL = (ERROR_CODE_FFMPEG_PREPARE - 4);

    //根据流信息 配置上下文参数失败
    public static final int FFMPEG_CODEC_CONTEXT_PARAMETERS_FAIL = (ERROR_CODE_FFMPEG_PREPARE - 5);

    //打开解码器失败
    public static final int FFMPEG_OPEN_DECODER_FAIL = (ERROR_CODE_FFMPEG_PREPARE - 6);

    //没有音视频
    public static final int FFMPEG_NOMEDIA = (ERROR_CODE_FFMPEG_PREPARE - 7);

    //读取媒体数据包失败
    public static final int FFMPEG_READ_PACKETS_FAIL = (ERROR_CODE_FFMPEG_PLAY - 1);

    public void setDataSource(String dataSource) {
        this.dataSource = dataSource;
    }

    @Override
    public void setSurfaceView(SurfaceView surfaceView) {
        if(surfaceHolder != null){
            surfaceHolder.removeCallback(this);
        }
        surfaceHolder = surfaceView.getHolder();
        surfaceHolder.addCallback(this);
    }

    @Override
    public void prepare() {
        nativePrepare(dataSource);
    }

    @Override
    public void onPrepared() {
        if(onPreparedListener != null){
            onPreparedListener.onPrepared();
        }
    }

    @Override
    public void setPreparedListener(onPreparedListener listener) {
        this.onPreparedListener = listener;
    }

    @Override
    public void start() {
        nativeStart();
    }

    @Override
    public void onStart() {

    }

    @Override
    public void pause() {
        nativePause();
    }

    @Override
    public void reStart() {
        nativeRestart();
    }

    @Override
    public void onError(int errorCode) {
        if(onErrorListener != null){
            onErrorListener.onError(errorCode2Str(errorCode));
        }
    }

    private String errorCode2Str(int errorCode) {
        String msg = "";
        switch (errorCode){
            case FFMPEG_CAN_NOT_OPEN_URL:
                msg = "打不开视频";
                break ;
            case FFMPEG_CAN_NOT_FIND_STREAMS:
                msg = "找不到媒体流信息";
                break ;
            case FFMPEG_FIND_DECODER_FAIL:
                msg = "找不到解码器";
                break ;
            case FFMPEG_ALLOC_CODEC_CONTEXT_FAIL:
                msg = "无法根据解码器创建上下文";
                break ;
            case FFMPEG_CODEC_CONTEXT_PARAMETERS_FAIL:
                msg = "根据流信息 配置上下文参数失败";
                break ;
            case FFMPEG_OPEN_DECODER_FAIL:
                msg = "打开解码器失败";
                break ;
            case FFMPEG_NOMEDIA:
                msg = "没有音视频";
                break ;
            case FFMPEG_READ_PACKETS_FAIL:
                msg = "读取媒体数据包失败";
                break;
            default:
                msg = "未知错误";
                break;
        }
        return msg;
    }

    @Override
    public void setErrorListener(onErrorListener listener) {
        this.onErrorListener = listener;
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {

    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        nativeSetSurface(holder.getSurface());
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }

    private native void nativePrepare(String dataSource);
    private native void nativeStart();
    private native void nativeSetSurface(Surface surface);
    private native void nativePause();
    private native void nativeRestart();
}

package cn.kkaka.avpalyer;

import android.view.SurfaceView;

public interface AvPlayInterface {

    /**
     * 设置surfaceView
     * @param surfaceView
     */
    void setSurfaceView(SurfaceView surfaceView);

    /**
     * 准备播放
     */
    void prepare();

    /**
     * 准备完毕
     */
    void onPrepared();

    /**
     * 开始播放
     */
    void start();

    /**
     * 播放中
     */
    void onStart();

    /**
     * 出现错误
     * @param errorCode
     */
    void onError(int errorCode);

}
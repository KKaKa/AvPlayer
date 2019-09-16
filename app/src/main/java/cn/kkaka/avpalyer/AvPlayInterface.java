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
     * 设置准备完毕回调
     * @param listener
     */
    void setPreparedListener(onPreparedListener listener);

    /**
     * 开始播放
     */
    void start();

    /**
     * 播放中
     */
    void onStart();

    /**
     * 暂停播放
     */
    void pause();

    /**
     * 恢复播放
     */
    void reStart();

    /**
     * 出现错误
     * @param errorCode
     */
    void onError(int errorCode);

    /**
     * 设置错误回调
     * @param listener
     */
    void setErrorListener(onErrorListener listener);

    /**
     * 获取总播放时长
     * @return
     */
    int getDuration();

    /**
     * 设置进度回调
     * @param listener
     */
    void setProgressListener(onProgressListener listener);

    /**
     * 进度回调
     * @param progress
     */
    void onProgress(int progress);

    interface onPreparedListener{
        void onPrepared();
    }

    interface onErrorListener{
        void onError(String errorMsg);
    }

    interface onProgressListener{
        void onProgress(int progress);
    }

}

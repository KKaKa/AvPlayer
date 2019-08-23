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

    public void setDataSource(String dataSource) {
        this.dataSource = dataSource;
    }

    @Override
    public void setSurfaceView(SurfaceView surfaceView) {
        if(surfaceView != null){
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
    public void onError(int errorCode) {

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
}

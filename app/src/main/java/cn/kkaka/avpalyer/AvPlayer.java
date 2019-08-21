package cn.kkaka.avpalyer;

import android.view.SurfaceView;

public class AvPlayer implements AvPlayInterface{

    static {
        System.loadLibrary("native-lib");
    }

    private String dataSource;

    public void setDataSource(String dataSource) {
        this.dataSource = dataSource;
    }

    @Override
    public void setSurfaceView(SurfaceView surfaceView) {
    }

    @Override
    public void prepare() {
        nativePrepare(dataSource);
    }

    @Override
    public void onPrepared() {

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

    private native void nativePrepare(String dataSource);
    private native void nativeStart();
}

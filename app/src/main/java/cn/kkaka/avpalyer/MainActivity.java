package cn.kkaka.avpalyer;

import android.Manifest;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Environment;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.Toast;

import java.io.File;

import permissions.dispatcher.NeedsPermission;
import permissions.dispatcher.RuntimePermissions;

@RuntimePermissions
public class MainActivity extends AppCompatActivity {
    private static final String TEST_NET_URL_1 = "http://media.w3.org/2010/05/sintel/trailer.mp4";
    private static final String TEST_NET_URL_2 = "http://tanzi27niu.cdsb.mobi/wps/wp-content/uploads/2017/04/2017-04-28_18-20-56.mp4";
    private static final String TEST_LOCAL_URL = new File(Environment.getExternalStorageDirectory() + File.separator + "input.mp4").getAbsolutePath();
    private SurfaceView surfaceView;
    private AvPlayer avPlayer;
    private String url;
    private Button mBtnPrepare;
    private Button mBtnPause;
    private Button mBtnRestart;
    private SeekBar mSeekBar;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        surfaceView = findViewById(R.id.surfaceView);
        mBtnPrepare = findViewById(R.id.btn_prepare);
        mBtnPause = findViewById(R.id.btn_pause);
        mBtnRestart = findViewById(R.id.btn_restart);
        mSeekBar = findViewById(R.id.seek_bar);
        url = TEST_NET_URL_2;
        avPlayer = new AvPlayer();
        avPlayer.setDataSource(url);
        avPlayer.setSurfaceView(surfaceView);
        avPlayer.setPreparedListener(() -> {
            int duration = avPlayer.getDuration();
            //直播类可能为0 不显示进度条
            if(duration == 0){
                runOnUiThread(()->{
                    mSeekBar.setVisibility(View.GONE);
                });
            }else{
                runOnUiThread(()->{
                    mSeekBar.setVisibility(View.VISIBLE);
                });
            }

            runOnUiThread(() -> {
                Log.i("","AvPlayer : onPrepared");
                Toast.makeText(MainActivity.this,"准备完毕",Toast.LENGTH_LONG).show();
            });
            avPlayer.start();
        });
        avPlayer.setErrorListener(errorMsg -> runOnUiThread(() -> {
            Log.i("","AvPlayer : onError : "+errorMsg);
            Toast.makeText(MainActivity.this,errorMsg,Toast.LENGTH_LONG).show();
        }));
        avPlayer.setProgressListener(progress -> {
            Log.i("","AvPlayer : onProgress : "+progress);
            runOnUiThread(()->{
                int duration = avPlayer.getDuration();
                if(duration != 0){
                    mSeekBar.setProgress(progress * 100 / duration);
                }
            });
        });
        mBtnPrepare.setOnClickListener(v -> {
            if(avPlayer != null){
                MainActivityPermissionsDispatcher.prepareWithPermissionCheck(MainActivity.this);
            }
        });
        mBtnPause.setOnClickListener(v -> {
            if(avPlayer != null){
                avPlayer.pause();
            }
        });
        mBtnRestart.setOnClickListener(v -> {
            if(avPlayer != null){
                avPlayer.reStart();
            }
        });
        mSeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {

            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    @NeedsPermission({Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE})
    void prepare() {
        avPlayer.prepare();
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        MainActivityPermissionsDispatcher.onRequestPermissionsResult(this, requestCode, grantResults);
    }
}

package cn.kkaka.avpalyer;

import android.Manifest;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Environment;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;
import android.webkit.WebView;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.Spinner;
import android.widget.Toast;

import java.io.File;

import permissions.dispatcher.NeedsPermission;
import permissions.dispatcher.RuntimePermissions;

@RuntimePermissions
public class MainActivity extends AppCompatActivity {
    private static final String TAG = "AvPlayer";
    private SurfaceView surfaceView;
    private AvPlayer avPlayer;
    private String url;
    private Button mBtnPrepare;
    private Button mBtnPause;
    private Button mBtnRestart;
    private Button mBtnStop;
    private SeekBar mSeekBar;
    private boolean isTouch ;
    private boolean isSeek ;
    private boolean isPrepared;
    private boolean isPause;
    private boolean isStop = true;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        surfaceView = findViewById(R.id.surfaceView);
        mBtnPrepare = findViewById(R.id.btn_prepare);
        mBtnPause = findViewById(R.id.btn_pause);
        mBtnRestart = findViewById(R.id.btn_restart);
        mBtnStop = findViewById(R.id.btn_stop);

        mSeekBar = findViewById(R.id.seek_bar);
        mSeekBar.setEnabled(false);
        mSeekBar.setProgress(0);

        if(getIntent() != null){
            url = getIntent().getStringExtra(SelectActivity.KEY);
        }

        avPlayer = new AvPlayer();
        avPlayer.setDataSource(url);
        avPlayer.setSurfaceView(surfaceView);
        avPlayer.setPreparedListener(() -> {
            int duration = avPlayer.getDuration();
            //直播类可能为0 不显示进度条
            if(duration == 0){
                runOnUiThread(()-> mSeekBar.setVisibility(View.GONE));
            }else{
                runOnUiThread(()-> mSeekBar.setVisibility(View.VISIBLE));
            }

            runOnUiThread(() -> {
                Log.e(TAG,"AvPlayer : onPrepared");
                Toast.makeText(MainActivity.this,"准备完毕",Toast.LENGTH_LONG).show();
                mSeekBar.setEnabled(true);
                isPrepared = true;
                isStop = false;
                avPlayer.start();
            });
        });
        avPlayer.setErrorListener(errorMsg -> runOnUiThread(() -> {
            Log.e(TAG,"AvPlayer : onError : "+errorMsg);
            Toast.makeText(MainActivity.this,errorMsg,Toast.LENGTH_LONG).show();
            isPrepared = false;
        }));
        avPlayer.setProgressListener(progress -> {
            if(!isTouch){
                runOnUiThread(()->{
                    int duration = avPlayer.getDuration();
                    if(duration != 0){
                        if(isSeek){
                            isSeek = false;
                            return;
                        }
                        mSeekBar.setProgress(progress * 100 / duration);
                    }
                });
            }
        });
        mBtnPrepare.setOnClickListener(v -> {
            if(avPlayer != null && isStop){
                MainActivityPermissionsDispatcher.prepareWithPermissionCheck(MainActivity.this);
            }
        });
        mBtnPause.setOnClickListener(v -> pause());
        mBtnRestart.setOnClickListener(v -> restart());
        mBtnStop.setOnClickListener(v -> {
            stop();
        });
        mSeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {

            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
                isTouch = true;
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                isTouch = false;
                isSeek = true;
                int seekBarProgress = seekBar.getProgress();
                int duration = avPlayer.getDuration();
                int playProgress = duration * seekBarProgress / 100;
                avPlayer.seekTo(playProgress);
            }
        });
    }

    private void restart(){
        if(avPlayer != null && isPrepared && isPause){
            Log.e(TAG,"AvPlayer : reStart");
            avPlayer.reStart();
            isPause = false;
        }
    }

    private void pause(){
        if(avPlayer != null && !isPause){
            Log.e(TAG,"AvPlayer : pause");
            avPlayer.pause();
            isPause = true;
        }
    }

    private void stop(){
        if(avPlayer != null && !isStop){
            avPlayer.stop();
            isStop = true;
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        restart();
    }

    @Override
    protected void onPause() {
        super.onPause();
        pause();
    }

    @Override
        protected void onDestroy() {
        super.onDestroy();
        if(!isStop){
            avPlayer.stop();
        }
        avPlayer.release();
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

    public static void Lauch(Context context,String url){
        Intent intent = new Intent(context,MainActivity.class);
        Bundle bundle = new Bundle();
        bundle.putString(SelectActivity.KEY,url);
        intent.putExtras(bundle);
        context.startActivity(intent);
    }
}

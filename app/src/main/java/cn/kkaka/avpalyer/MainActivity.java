package cn.kkaka.avpalyer;

import android.Manifest;
import android.os.Bundle;
import android.os.Environment;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import java.io.File;

import permissions.dispatcher.NeedsPermission;
import permissions.dispatcher.RuntimePermissions;

@RuntimePermissions
public class MainActivity extends AppCompatActivity {
    private SurfaceView surfaceView;
    private AvPlayer avPlayer;
    private String url;
    private Button mBtnPrepare;

    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        surfaceView = findViewById(R.id.surfaceView);
        mBtnPrepare = findViewById(R.id.btn_prepare);

        url = new File(Environment.getExternalStorageDirectory() + File.separator + "new.mp4").getAbsolutePath();
        avPlayer = new AvPlayer();
        avPlayer.setDataSource(url);
        avPlayer.setSurfaceView(surfaceView);
        avPlayer.setPreparedListener(new AvPlayInterface.onPreparedListener() {
            @Override
            public void onPrepared() {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Log.i("","AvPlayer : onPrepared");
                        Toast.makeText(MainActivity.this,"准备完毕",Toast.LENGTH_LONG).show();
                    }
                });
                avPlayer.start();
            }
        });
        avPlayer.setErrorListener(new AvPlayInterface.onErrorListener() {
            @Override
            public void onError(final String errorMsg) {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Log.i("","AvPlayer : onError : "+errorMsg);
                        Toast.makeText(MainActivity.this,errorMsg,Toast.LENGTH_LONG).show();
                    }
                });
            }
        });
        mBtnPrepare.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(avPlayer != null){
                    MainActivityPermissionsDispatcher.prepareWithPermissionCheck(MainActivity.this);
                }
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

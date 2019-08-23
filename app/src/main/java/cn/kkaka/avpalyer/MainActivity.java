package cn.kkaka.avpalyer;

import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import java.io.File;

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

        url = new File(Environment.getExternalStorageDirectory() + File.separator + "demo.mp4").getAbsolutePath();
        avPlayer = new AvPlayer();
        avPlayer.setDataSource("路径");
        avPlayer.setPreparedListener(new AvPlayInterface.onPreparedListener() {
            @Override
            public void onPrepared() {
                Log.i("","AvPlayer : onPrepared");
                Toast.makeText(MainActivity.this,"准备完毕",Toast.LENGTH_LONG).show();
            }
        });

        mBtnPrepare.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(avPlayer != null){
                    avPlayer.prepare();
                }
            }
        });
    }

    @Override
    protected void onResume() {
        super.onResume();
    }
}

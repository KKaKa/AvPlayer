package cn.kkaka.avpalyer;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.SurfaceView;

public class MainActivity extends AppCompatActivity {
    private SurfaceView surfaceView;
    private AvPlayer avPlayer;

    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        surfaceView = findViewById(R.id.surfaceView);
        avPlayer.setDataSource("路径");
    }

    @Override
    protected void onResume() {
        super.onResume();
        avPlayer.prepare();
    }
}

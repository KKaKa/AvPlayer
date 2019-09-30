package cn.kkaka.avpalyer;

import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.widget.Button;

import java.io.File;

public class SelectActivity extends AppCompatActivity {
    public static final String KEY = "URL";
    private static final String TEST_NET_URL_1 = "http://media.w3.org/2010/05/sintel/trailer.mp4";
    private static final String TEST_NET_URL_2 = "http://tanzi27niu.cdsb.mobi/wps/wp-content/uploads/2017/04/2017-04-28_18-20-56.mp4";
    private static final String TEST_LOCAL_URL = new File(Environment.getExternalStorageDirectory() + File.separator + "input.mp4").getAbsolutePath();
    private Button btnSelectLoc;
    private Button btnSelectNet1;
    private Button btnSelectNet2;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_select);
        initView();
        initListener();
    }

    private void initListener() {
        btnSelectLoc.setOnClickListener((view)->{
            MainActivity.Lauch(this,TEST_LOCAL_URL);
        });

        btnSelectNet1.setOnClickListener((view)->{
            MainActivity.Lauch(this,TEST_NET_URL_1);
        });

        btnSelectNet2.setOnClickListener((view)->{
            MainActivity.Lauch(this,TEST_NET_URL_2);
        });
    }

    private void initView() {
        btnSelectLoc = (Button) findViewById(R.id.btn_select_loc);
        btnSelectNet1 = (Button) findViewById(R.id.btn_select_net_1);
        btnSelectNet2 = (Button) findViewById(R.id.btn_select_net_2);
    }


}

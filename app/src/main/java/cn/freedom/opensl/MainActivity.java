package cn.freedom.opensl;

import android.graphics.Path;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity implements View.OnClickListener{
    Button btnRecStart = null;
    Button btnRecStop = null;
    Button btnPlayStart = null;
    Button btnPlayStop = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        OpenSLManager.getInstance().CreateSLEngine(this);

        btnRecStart = (Button)findViewById(R.id.btnRecStart);
        btnRecStart.setOnClickListener(this);

        btnRecStop = (Button)findViewById(R.id.btnRecStop);
        btnRecStop.setOnClickListener(this);

        btnPlayStart = (Button)findViewById(R.id.btnPlayStart);
        btnPlayStart.setOnClickListener(this);

        btnPlayStop = (Button)findViewById(R.id.btnPlayStop);
        btnPlayStop.setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.btnRecStart) {
            OpenSLManager.getInstance().createAudioRecorder();
            OpenSLManager.getInstance().startRecording();
        } else if (v.getId() == R.id.btnRecStop) {
            OpenSLManager.getInstance().stopRecording();
            OpenSLManager.getInstance().deleteAudioRecorder();
        } else if (v.getId() == R.id.btnPlayStart) {
            OpenSLManager.getInstance().createAudioPlayer();
            OpenSLManager.getInstance().startPlayout();
        } else if (v.getId() == R.id.btnPlayStop) {
            OpenSLManager.getInstance().stopPlayout();
            OpenSLManager.getInstance().deleteAudioPlayer();
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        OpenSLManager.getInstance().deleteSlEngine();
    }
}

package cn.freedom.opensl;

import android.content.Context;
import android.content.pm.PackageManager;
import android.media.AudioManager;
import android.util.Log;

/**
 * Created by vinton on 2017/11/07,0007.
 */

public class OpenSLManager {
    private static final String TAG = "OpenSLManager";

    private static final OpenSLManager ourInstance = new OpenSLManager();

    private native int createSLEngine(int sampleRate, int framesPerBuf);
    native int deleteSlEngine();

    native int createAudioPlayer();
    native int deleteAudioPlayer();
    native int startPlayout();
    native int stopPlayout();

    native int createAudioRecorder();
    native int deleteAudioRecorder();
    native int startRecording();
    native int stopRecording();

    private final static int DEFAULT_SAMPLE_RATE = 16000;

    // Randomly picked up frame size which is close to return value on N4.
    // Return this default value when
    // getProperty(PROPERTY_OUTPUT_FRAMES_PER_BUFFER) fails.
    private final static int DEFAULT_FRAMES_PER_BUFFER = 256;

    private AudioManager mAudioMgr = null;
    private int mOutputSampelRate = DEFAULT_SAMPLE_RATE;
    private int mAudioLowLatencyOutputFrameSize = DEFAULT_FRAMES_PER_BUFFER;
    private boolean isSupportLowLatency = false;

    public static OpenSLManager getInstance() {
        return ourInstance;
    }

    private OpenSLManager() {
    }

    public int CreateSLEngine(Context context) {
        mAudioMgr = (AudioManager) context.getSystemService(Context.AUDIO_SERVICE);

        String sampleRateString = mAudioMgr.getProperty(AudioManager.PROPERTY_OUTPUT_SAMPLE_RATE);
        if (sampleRateString != null) {
            mOutputSampelRate = Integer.parseInt(sampleRateString);
        }

        String framesPerBuffer = mAudioMgr.getProperty(AudioManager.PROPERTY_OUTPUT_FRAMES_PER_BUFFER);
        if (framesPerBuffer != null) {
            mAudioLowLatencyOutputFrameSize = Integer.parseInt(framesPerBuffer);
        }

        isSupportLowLatency = context.getPackageManager().hasSystemFeature(PackageManager.FEATURE_AUDIO_LOW_LATENCY);

        Log.i(TAG, "isSupportLowLatency = " + isSupportLowLatency);
        Log.i(TAG, "mOutputSampelRate = " + mOutputSampelRate);
        Log.i(TAG, "mAudioLowLatencyOutputFrameSize = " + mAudioLowLatencyOutputFrameSize);

        return createSLEngine(mOutputSampelRate, mAudioLowLatencyOutputFrameSize);
    }

    static {
        System.loadLibrary("native_audio");
    }
}

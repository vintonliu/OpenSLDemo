//
// Created by vinton on 2017/11/07,0007.
//

#include "opensles_common.h"
#include "cn_freedom_opensl_OpenSLManager.h"
#include "opensles_output.h"


typedef struct AudioEngine
{
    SLObjectItf slEngineObj_;
    SLEngineItf slEngineItf_;

    OpenSlesOutput* output_;

    SLObjectItf slRecorderObj_;
    SLRecordItf slRecorderItf_;

    int fastPathSampleRate_;
    int fastPathFramesPerBuf_;
    int numChannels_;
    int bitsPerSample;
} SLAudioEngine;

static SLAudioEngine slAudioEngine;

/*
 * Prototypes for functions exported by loadable shared libs.  These are
 * called by JNI, not provided by JNI.
 */
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{

    return JNI_VERSION_1_6;
}

/*
 * Class:     cn_freedom_opensl_OpenSLManager
 * Method:    createSLEngine
 * Signature: (II)I
 */
JNIEXPORT int JNICALL Java_cn_freedom_opensl_OpenSLManager_createSLEngine
        (JNIEnv *env, jobject thiz, jint sampleRate, jint framesPerBuf)
{
    const SLEngineOption option[] = {SL_ENGINEOPTION_THREADSAFE, SL_BOOLEAN_TRUE};

    memset(&slAudioEngine, 0x00, sizeof(SLAudioEngine));
    slAudioEngine.fastPathSampleRate_ = sampleRate;
    slAudioEngine.fastPathFramesPerBuf_ = framesPerBuf;
    slAudioEngine.numChannels_ = 1;
    slAudioEngine.bitsPerSample = 16;

    OPENSL_RETURN_ON_FAILURE(slCreateEngine(&slAudioEngine.slEngineObj_,
                                            1, option, 0, NULL, NULL),
                             -1);

    OPENSL_RETURN_ON_FAILURE((*slAudioEngine.slEngineObj_)->Realize(slAudioEngine.slEngineObj_,
                                                                    SL_BOOLEAN_FALSE),
                             -1);

    OPENSL_RETURN_ON_FAILURE((*slAudioEngine.slEngineObj_)->GetInterface(slAudioEngine.slEngineObj_,
                                                                         SL_IID_ENGINE,
                                                                         &slAudioEngine.slEngineItf_),
                             -1);
    return 0;
}

/*
 * Class:     cn_freedom_opensl_OpenSLManager
 * Method:    deleteSlEngine
 * Signature: ()I
 */
JNIEXPORT int JNICALL Java_cn_freedom_opensl_OpenSLManager_deleteSlEngine
        (JNIEnv *, jobject)
{
    if (slAudioEngine.slEngineObj_ != NULL)
    {
        (*slAudioEngine.slEngineObj_)->Destroy(slAudioEngine.slEngineObj_);
        slAudioEngine.slEngineObj_ = NULL;
        slAudioEngine.slEngineItf_ = NULL;
    }
}

/*
 * Class:     cn_freedom_opensl_OpenSLManager
 * Method:    createAudioPlayer
 * Signature: ()I
 */
JNIEXPORT int JNICALL Java_cn_freedom_opensl_OpenSLManager_createAudioPlayer
        (JNIEnv *, jobject)
{

}

/*
 * Class:     cn_freedom_opensl_OpenSLManager
 * Method:    deleteAudioPlayer
 * Signature: ()I
 */
JNIEXPORT int JNICALL Java_cn_freedom_opensl_OpenSLManager_deleteAudioPlayer
        (JNIEnv *, jobject)
{

}

/*
 * Class:     cn_freedom_opensl_OpenSLManager
 * Method:    startPlayout
 * Signature: ()I
 */
JNIEXPORT int JNICALL Java_cn_freedom_opensl_OpenSLManager_startPlayout
        (JNIEnv *, jobject)
{

}

/*
 * Class:     cn_freedom_opensl_OpenSLManager
 * Method:    stopPlayout
 * Signature: ()I
 */
JNIEXPORT int JNICALL Java_cn_freedom_opensl_OpenSLManager_stopPlayout
        (JNIEnv *, jobject)
{

}

/*
 * Class:     cn_freedom_opensl_OpenSLManager
 * Method:    createAudioRecorder
 * Signature: ()I
 */
JNIEXPORT int JNICALL Java_cn_freedom_opensl_OpenSLManager_createAudioRecorder
        (JNIEnv *, jobject)
{

}

/*
 * Class:     cn_freedom_opensl_OpenSLManager
 * Method:    deleteAudioRecorder
 * Signature: ()I
 */
JNIEXPORT int JNICALL Java_cn_freedom_opensl_OpenSLManager_deleteAudioRecorder
        (JNIEnv *, jobject)
{

}

/*
 * Class:     cn_freedom_opensl_OpenSLManager
 * Method:    startRecording
 * Signature: ()I
 */
JNIEXPORT int JNICALL Java_cn_freedom_opensl_OpenSLManager_startRecording
        (JNIEnv *, jobject)
{

}

/*
 * Class:     cn_freedom_opensl_OpenSLManager
 * Method:    stopRecording
 * Signature: ()I
 */
JNIEXPORT int JNICALL Java_cn_freedom_opensl_OpenSLManager_stopRecording
        (JNIEnv *, jobject)
{

}

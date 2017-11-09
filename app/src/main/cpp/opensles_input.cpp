//
// Created by vinton on 2017/11/07,0007.
//

#include "opensles_input.h"

OpenSlesInput::OpenSlesInput(SLEngineItf slEngineItf, int sampleRate)
        : slEngineItf_(slEngineItf),
          slRecordObjItf_(NULL),
          slRecordItf_(NULL),
          slRecordQueueBufferItf_(NULL),
          sampleRate_(sampleRate),
          samplesPer10ms_(sampleRate_ * 10 / 1000),
          bytesPer10ms_(kNumChannels * 2 * samplesPer10ms_){

    recBuf_ = new char[bytesPer10ms_];
    memset(&recBuf_, 0x00, bytesPer10ms_);
}

OpenSlesInput::~OpenSlesInput() {
    destoryAudioRecorder();
    if (recFp_ != NULL) {
        fclose(recFp_);
        recFp_ = NULL;
    }

    if (recBuf_ != NULL) {
        delete recBuf_;
        recBuf_ = NULL;
    }
}

int OpenSlesInput::initRecording() {
    createAudioRecorder();

    recFp_ = fopen("/mnt/sdcard/record_native.pcm", "wb");
    if (recFp_ == NULL) {
        LOGE("[%s %d] file open failed.", __FUNCTION__, __LINE__);
    }
    return 0;
}

int OpenSlesInput::startRecording() {

    // in case already recording, stop recording and clear buffer queue
    OPENSL_RETURN_ON_FAILURE((*slRecordItf_)->SetRecordState(slRecordItf_,
                                                             SL_RECORDSTATE_STOPPED),
                             -1);

    OPENSL_RETURN_ON_FAILURE((*slRecordQueueBufferItf_)->Clear(slRecordQueueBufferItf_),
                             -1);

    OPENSL_RETURN_ON_FAILURE((*slRecordItf_)->SetRecordState(slRecordItf_,
                                                             SL_RECORDSTATE_RECORDING),
                             -1);

    LOGI("[%s %d] ok", __FUNCTION__, __LINE__);
    return 0;
}

int OpenSlesInput::stopRecording() {
    SLuint32 curState;
    LOGI("[%s %d]", __FUNCTION__, __LINE__);

    OPENSL_RETURN_ON_FAILURE((*slRecordItf_)->GetRecordState(slRecordItf_,
                                                             &curState),
                             -1);

    if (curState == SL_RECORDSTATE_STOPPED) {
        return 0;
    }

    OPENSL_RETURN_ON_FAILURE((*slRecordItf_)->SetRecordState(slRecordItf_,
                                                             SL_RECORDSTATE_STOPPED),
                             -1);

    OPENSL_RETURN_ON_FAILURE((*slRecordQueueBufferItf_)->Clear(slRecordQueueBufferItf_),
                             -1);

    destoryAudioRecorder();

    if (recFp_ != NULL) {
        fclose(recFp_);
        recFp_ = NULL;
    }
    return 0;
}

int OpenSlesInput::createAudioRecorder() {
    // configure audio source
    SLDataLocator_IODevice loc_dev = {
            SL_DATALOCATOR_IODEVICE, // locatorType
            SL_IODEVICE_AUDIOINPUT, // deviceType
            SL_DEFAULTDEVICEID_AUDIOINPUT, // deviceID
            NULL // device
    };

    SLDataSource audioSrc = { &loc_dev, NULL};

    // configure audio sink
    SLDataLocator_AndroidSimpleBufferQueue loc_sbq = {
            SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
            kNum10MsToBuffer + kNumOpenSlBuffers
    };

    SLDataFormat_PCM formatPcm = {
            SL_DATAFORMAT_PCM, // formatType
            kNumChannels, // numChannels
            // According to the opensles documentation in the ndk:
            // samplesPerSec is actually in units of milliHz, despite the misleading name.
            // It further recommends using constants. However, this would lead to a lot
            // of boilerplate code so it is not done here.
            sampleRate_ * 1000, // samplesPerSec
            SL_PCMSAMPLEFORMAT_FIXED_16, // bitsPerSample
            SL_PCMSAMPLEFORMAT_FIXED_16, // containerSize
            SL_SPEAKER_FRONT_CENTER, // channelMask
            SL_BYTEORDER_LITTLEENDIAN // endianness
    };

    SLDataSink audioSnk = {&loc_sbq, &formatPcm};

    // Interfaces for recording android audio data and Android are needed.
    // Note the interfaces still need to be initialized. This only tells OpenSl
    // that the interfaces will be needed at some point.
    const SLInterfaceID ids[kNumInterfaces] = {
            SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
            SL_IID_ANDROIDCONFIGURATION
    };
    const SLboolean req[kNumInterfaces] = {
            SL_BOOLEAN_TRUE,
            SL_BOOLEAN_TRUE
    };

    OPENSL_RETURN_ON_FAILURE((*slEngineItf_)->CreateAudioRecorder(slEngineItf_,
                                                                  &slRecordObjItf_,
                                                                  &audioSrc,
                                                                  &audioSnk,
                                                                  kNumInterfaces,
                                                                  ids,
                                                                  req),
                             -1);

    SLAndroidConfigurationItf recCfgItf;
    OPENSL_RETURN_ON_FAILURE((*slRecordObjItf_)->GetInterface(slRecordObjItf_,
                                                              SL_IID_ANDROIDCONFIGURATION,
                                                              &recCfgItf),
                             -1);

    // Set audio recorder configuration to
    // SL_ANDROID_RECORDING_PRESET_VOICE_COMMUNICATION which ensures that we
    // use the main microphone tuned for audio communications.
    SLint32 audioSrcTyp = SL_ANDROID_RECORDING_PRESET_VOICE_COMMUNICATION;
    OPENSL_RETURN_ON_FAILURE((*recCfgItf)->SetConfiguration(recCfgItf,
                                                            SL_ANDROID_KEY_RECORDING_PRESET,
                                                            &audioSrcTyp,
                                                            sizeof(SLint32)),
                             -1);

    // Realize the recorder in synchronous mode.
    OPENSL_RETURN_ON_FAILURE((*slRecordObjItf_)->Realize(slRecordObjItf_,
                                                         SL_BOOLEAN_FALSE),
                             -1);

    // get the record interface
    OPENSL_RETURN_ON_FAILURE((*slRecordObjItf_)->GetInterface(slRecordObjItf_,
                                                              SL_IID_RECORD,
                                                              &slRecordItf_),
                             -1);

    // get the buffer queue interface
    OPENSL_RETURN_ON_FAILURE((*slRecordObjItf_)->GetInterface(slRecordObjItf_,
                                                              SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
                                                              &slRecordQueueBufferItf_),
                             -1);

    // Setup to receive buffer queue event callbacks.
    OPENSL_RETURN_ON_FAILURE((*slRecordQueueBufferItf_)->RegisterCallback(slRecordQueueBufferItf_,
                                                                          recordSimpleBufferQueueCallback,
                                                                          this),
                             -1);

    LOGI("[%s %d] ok", __FUNCTION__, __LINE__);
    return 0;
}

int OpenSlesInput::destoryAudioRecorder() {
    // destroy audio recorder object, and invalidate all associated interfaces
    if (slRecordObjItf_ != NULL) {
        (*slRecordObjItf_)->Destroy(slRecordObjItf_);
        slRecordObjItf_ = NULL;
        slRecordItf_ = NULL;
        slRecordQueueBufferItf_ = NULL;
    }

    LOGI("[%s %d] ok", __FUNCTION__, __LINE__);
    return 0;
}

void OpenSlesInput::recordSimpleBufferQueueCallback(SLAndroidSimpleBufferQueueItf slRecordSbqItf,
                                                    void *pContext) {
    OpenSlesInput* input = (OpenSlesInput*)pContext;
    input->processSbqCallback(slRecordSbqItf);
}

void OpenSlesInput::processSbqCallback(SLAndroidSimpleBufferQueueItf slRecordSbqItf) {

    memset(&recBuf_, 0x00, bytesPer10ms_);
    OPENSL_RETURN_ON_FAILURE((*slRecordQueueBufferItf_)->Enqueue(slRecordQueueBufferItf_,
                                                                 recBuf_,
                                                                 bytesPer10ms_),
    VOID_RETURN);

    if (recFp_ != NULL) {
        fwrite(recBuf_, 1, bytesPer10ms_, recFp_);
    }
}

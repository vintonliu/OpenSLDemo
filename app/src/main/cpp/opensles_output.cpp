//
// Created by vinton on 2017/11/07,0007.
//

#include "opensles_output.h"

OpenSlesOutput::OpenSlesOutput(SLEngineItf slEngineItf, int sampleRate)
        : slEngineItf_(slEngineItf),
          slOutputMixObjItf_(NULL),
          slPlayerObjItf_(NULL),
          slPlayItf_(NULL),
          slPlayBufferQueueItf_(NULL),
          sampleRate_(sampleRate),
          channels_(kNumChannels){

    // create and realize output mix
    OPENSL_RETURN_ON_FAILURE((*slEngineItf_)->CreateOutputMix(slEngineItf_,
                                                              &slOutputMixObjItf_,
                                                              0,
                                                              NULL,
                                                              NULL),
                             VOID_RETURN);

    OPENSL_RETURN_ON_FAILURE((*slOutputMixObjItf_)->Realize(slOutputMixObjItf_,
                                                            SL_BOOLEAN_FALSE),
                             VOID_RETURN);

    // Default is to use 10ms buffers
    samplesPer10ms_ = sampleRate_ * 10 / 1000;
    bytesPer10ms_ = channels_ * (16 / 8) * samplesPer10ms_;
    silenceBuf_ = new char[bytesPer10ms_];
    memset(silenceBuf_, 0x00, bytesPer10ms_);

    playBuf_ = new char[bytesPer10ms_];
    memset(playBuf_, 0x00, bytesPer10ms_);

}

OpenSlesOutput::~OpenSlesOutput() {
    destoryAudioPlayer();

    if (slOutputMixObjItf_ != NULL) {
        (*slOutputMixObjItf_)->Destroy(slOutputMixObjItf_);
        slOutputMixObjItf_ = NULL;
    }

    if (silenceBuf_ != NULL) {
        delete silenceBuf_;
        silenceBuf_ = NULL;
    }

    if (playBuf_ != NULL) {
        delete playBuf_;
        playBuf_ = NULL;
    }
}

int OpenSlesOutput::initPlayout() {

    playFp_ = fopen("/mnt/sdcard/cuiniao_stero.pcm", "rb");
    if (playFp_ == NULL) {
        LOGE("[%s %d] file open failed.", __FUNCTION__, __LINE__);
    }

    return createAudioPlayer();
}

int OpenSlesOutput::startPlayout() {
    SLuint32 state;
    OPENSL_RETURN_ON_FAILURE((*slPlayItf_)->GetPlayState(slPlayItf_, &state),
                             -1);

    if (state == SL_PLAYSTATE_PLAYING) {
        return 0;
    }

    OPENSL_RETURN_ON_FAILURE((*slPlayItf_)->SetPlayState(slPlayItf_,
                                                         SL_PLAYSTATE_STOPPED),
                             -1);

    enqueueAllBuffers();

    OPENSL_RETURN_ON_FAILURE((*slPlayItf_)->SetPlayState(slPlayItf_,
                                                         SL_PLAYSTATE_PLAYING),
                             -1);
    return 0;
}

int OpenSlesOutput::stopPlayout() {
    LOGI("[%s %d]", __FUNCTION__, __LINE__);
    SLuint32 state;
    if (slPlayItf_ != NULL) {
        OPENSL_RETURN_ON_FAILURE((*slPlayItf_)->GetPlayState(slPlayItf_,
                                                             &state),
                                 -1);

        if (state == SL_PLAYSTATE_STOPPED) {
            return 0;
        }

        OPENSL_RETURN_ON_FAILURE((*slPlayItf_)->SetPlayState(slPlayItf_,
                                                             SL_PLAYSTATE_STOPPED),
                                 -1);
        slPlayItf_ = NULL;
    }

    return destoryAudioPlayer();
}

int OpenSlesOutput::createAudioPlayer() {
    // configure audio source
    SLDataLocator_AndroidSimpleBufferQueue simple_buf_queue = {
            SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
            kNumOpenSlBuffers
    };

    SLDataFormat_PCM format_pcm;
    format_pcm.formatType = SL_DATAFORMAT_PCM;
    format_pcm.numChannels = kNumChannels;
    format_pcm.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
    format_pcm.containerSize = SL_PCMSAMPLEFORMAT_FIXED_16;
    format_pcm.channelMask = SL_SPEAKER_BACK_CENTER;
    if (channels_ == 2) {
        format_pcm.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
    }
    format_pcm.endianness = SL_BYTEORDER_LITTLEENDIAN;
    format_pcm.samplesPerSec = sampleRate_ * 1000;

    SLDataSource audioSrc = {&simple_buf_queue, &format_pcm};

    // configure audio sink
    SLDataLocator_OutputMix loc_outmix = {
            SL_DATALOCATOR_OUTPUTMIX,
            slOutputMixObjItf_};
    SLDataSink audioSnk = {&loc_outmix, NULL};

    // Interfaces for streaming audio data, setting volume and Android are needed.
    // Note the interfaces still need to be initialized. This only tells OpenSl
    // that the interfaces will be needed at some point.
    SLInterfaceID ids[kNumInterfaces] = {
            SL_IID_BUFFERQUEUE,
            SL_IID_VOLUME,
            SL_IID_ANDROIDCONFIGURATION};

    SLboolean req[kNumInterfaces] = {
            SL_BOOLEAN_TRUE,
            SL_BOOLEAN_TRUE,
            SL_BOOLEAN_TRUE};

    OPENSL_RETURN_ON_FAILURE((*slEngineItf_)->CreateAudioPlayer(slEngineItf_,
                                                                &slPlayerObjItf_,
                                                                &audioSrc,
                                                                &audioSnk,
                                                                kNumInterfaces,
                                                                ids,
                                                                req),
                             -1);

    SLAndroidConfigurationItf playCfgItf;
    OPENSL_RETURN_ON_FAILURE((*slPlayerObjItf_)->GetInterface(slPlayerObjItf_,
                                                              SL_IID_ANDROIDCONFIGURATION,
                                                              &playCfgItf),
                             -1);

    // Set audio player configuration to SL_ANDROID_STREAM_VOICE which corresponds
    // to android.media.AudioManager.STREAM_VOICE_CALL.
    SLint32 stream_type = SL_ANDROID_STREAM_VOICE;
    OPENSL_RETURN_ON_FAILURE((*playCfgItf)->SetConfiguration(playCfgItf,
                                                             SL_ANDROID_KEY_STREAM_TYPE,
                                                             &stream_type,
                                                             sizeof(SLint32)),
                             -1);

    // Realize the player in synchronous mode
    OPENSL_RETURN_ON_FAILURE((*slPlayerObjItf_)->Realize(slPlayerObjItf_,
                                                         SL_BOOLEAN_FALSE),
                             -1);

    // get the play interface
    OPENSL_RETURN_ON_FAILURE((*slPlayerObjItf_)->GetInterface(slPlayerObjItf_,
                                                              SL_IID_PLAY,
                                                              &slPlayItf_),
                             -1);

    // get the buffer queue interface
    OPENSL_RETURN_ON_FAILURE((*slPlayerObjItf_)->GetInterface(slPlayerObjItf_,
                                                              SL_IID_BUFFERQUEUE,
                                                              &slPlayBufferQueueItf_),
                             -1);

    // Register callback to receive enqueued buffers.
    OPENSL_RETURN_ON_FAILURE((*slPlayBufferQueueItf_)->RegisterCallback(slPlayBufferQueueItf_,
                                                                        playSimpleBufferQueueCallback,
                                                                        this),
                             -1);

    LOGI("[%s %d] ok", __FUNCTION__, __LINE__);

    return 0;
}

int OpenSlesOutput::destoryAudioPlayer() {
    if (slPlayBufferQueueItf_ != NULL) {
        OPENSL_RETURN_ON_FAILURE((*slPlayBufferQueueItf_)->Clear(slPlayBufferQueueItf_), -1);
        slPlayBufferQueueItf_ = NULL;
    }

    if (slPlayerObjItf_ != NULL) {
        (*slPlayerObjItf_)->Destroy(slPlayerObjItf_);
        slPlayerObjItf_ = NULL;
    }
    LOGI("[%s %d] ok", __FUNCTION__, __LINE__);

    return 0;
}

void OpenSlesOutput::enqueueAllBuffers() {
    for (int i = 0; i < kNumOpenSlBuffers; ++i) {
        OPENSL_RETURN_ON_FAILURE((*slPlayBufferQueueItf_)->Enqueue(slPlayBufferQueueItf_,
                                                                   silenceBuf_,
                                                                   bytesPer10ms_),
                                 VOID_RETURN);
    }
}

void OpenSlesOutput::playSimpleBufferQueueCallback(SLAndroidSimpleBufferQueueItf slPlayerSbqItf,
                                                   void *pContext) {
    OpenSlesOutput* audio_output = (OpenSlesOutput*)pContext;
    audio_output->processSbqCallback(slPlayerSbqItf);
}

void OpenSlesOutput::processSbqCallback(SLAndroidSimpleBufferQueueItf slPlayerSbqItf) {

    if (playFp_ != NULL) {
        int size = fread(playBuf_, 1, bytesPer10ms_, playFp_);
        if (size < bytesPer10ms_) {
            fseek(playFp_, 0, SEEK_SET);

            OPENSL_RETURN_ON_FAILURE((*slPlayItf_)->SetPlayState(slPlayItf_,
                                                                 SL_PLAYSTATE_STOPPED),
                                     VOID_RETURN);

            OPENSL_RETURN_ON_FAILURE((*slPlayItf_)->SetPlayState(slPlayItf_,
                                                                 SL_PLAYSTATE_PLAYING),
                                     VOID_RETURN);

        }
    }
    OPENSL_RETURN_ON_FAILURE((*slPlayerSbqItf)->Enqueue(slPlayerSbqItf,
                                                        playBuf_,
                                                        bytesPer10ms_),
                             VOID_RETURN);
}

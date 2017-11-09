//
// Created by vinton on 2017/11/07,0007.
//

#ifndef OPENSLDEMO_AUDIORECORDER_H
#define OPENSLDEMO_AUDIORECORDER_H

#include <stdio.h>
#include "opensles_common.h"

class OpenSlesInput {
public:
    OpenSlesInput(SLEngineItf slEngineItf, int sampleRate);
    ~OpenSlesInput();

    int initRecording();
    int startRecording();
    int stopRecording();

private:
    enum {
        kNumInterfaces = 2,
        // Keep as few OpenSL buffers as possible to avoid wasting memory. 2 is
        // minimum for playout. Keep 2 for recording as well.
        kNumOpenSlBuffers = 2,
        kNum10MsToBuffer = 3,
        kNumChannels = 1,
    };

    int createAudioRecorder();
    int destoryAudioRecorder();

    static void recordSimpleBufferQueueCallback (SLAndroidSimpleBufferQueueItf slRecordSbqItf,
                                                 void *pContext);

    void processSbqCallback(SLAndroidSimpleBufferQueueItf slRecordSbqItf);
private:
    SLEngineItf slEngineItf_;
    SLObjectItf slRecordObjItf_;
    SLRecordItf slRecordItf_;
    SLAndroidSimpleBufferQueueItf slRecordQueueBufferItf_;

    int sampleRate_;
    int samplesPer10ms_;
    int bytesPer10ms_;

    char* recBuf_;
    FILE* recFp_;
};


#endif //OPENSLDEMO_AUDIORECORDER_H

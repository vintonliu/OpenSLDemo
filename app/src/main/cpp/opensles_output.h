//
// Created by vinton on 2017/11/07,0007.
//

#ifndef OPENSLDEMO_AUDIOPLAYER_H
#define OPENSLDEMO_AUDIOPLAYER_H

#include "opensles_common.h"

class OpenSlesOutput {
public:
    OpenSlesOutput(SLEngineItf slEngineItf);
    ~OpenSlesOutput();

    int initPlayout();
    int startPlayout();
    int stopPlayout();

private:
    enum {
        kNumInterfaces = 3,
        // TODO(xians): Reduce the numbers of buffers to improve the latency.
        //              Currently 30ms worth of buffers are needed due to audio
        //              pipeline processing jitter. Note: kNumOpenSlBuffers must
        //              not be changed.
        // According to the opensles documentation in the ndk:
        // The lower output latency path is used only if the application requests a
        // buffer count of 2 or more. Use minimum number of buffers to keep delay
        // as low as possible.
        kNumOpenSlBuffers = 2,

        // NetEq delivers frames on a 10ms basis. This means that every 10ms there
        // will be a time consuming task. Keeping 10ms worth of buffers will ensure
        // that there is 10ms to perform the time consuming task without running
        // into underflow.
        // In addition to the 10ms that needs to be stored for NetEq processing
        // there will be jitter in audio pipe line due to the acquisition of locks.
        // Note: The buffers in the OpenSL queue do not count towards the 10ms of
        // frames needed since OpenSL needs to have them ready for playout.
        kNum10MsToBuffer = 6,
    };

    int Init();
private:
    SLEngineItf slEngineItf_;
    SLObjectItf slOutputMixObjItf_;
    SLObjectItf slPlayerObjItf_;
    SLPlayItf slPlayItf_;
    SLAndroidSimpleBufferQueueItf slPlayBufferQueueItf_;
};


#endif //OPENSLDEMO_AUDIOPLAYER_H

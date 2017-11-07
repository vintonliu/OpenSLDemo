//
// Created by vinton on 2017/11/07,0007.
//

#include "opensles_output.h"

OpenSlesOutput::OpenSlesOutput(SLEngineItf slEngineItf)
        : slEngineItf_(slEngineItf),
          slOutputMixObjItf_(NULL),
          slPlayerObjItf_(NULL),
          slPlayItf_(NULL),
          slPlayBufferQueueItf_(NULL) {
    Init();
}

OpenSlesOutput::~OpenSlesOutput() {
    if (slOutputMixObjItf_ != NULL) {
        (*slOutputMixObjItf_)->Destroy(slOutputMixObjItf_);
        slOutputMixObjItf_ = NULL;
    }
}

int OpenSlesOutput::Init() {
    OPENSL_RETURN_ON_FAILURE((*slEngineItf_)->CreateOutputMix(slEngineItf_,
                                                              &slOutputMixObjItf_,
                                                              0, NULL, NULL),
                             -1);

    OPENSL_RETURN_ON_FAILURE((*slOutputMixObjItf_)->Realize(slOutputMixObjItf_,
                                                            SL_BOOLEAN_FALSE),
                             -1);

    return 0;
}

int OpenSlesOutput::initPlayout() {


    return 0;
}

int OpenSlesOutput::startPlayout() {
    SLDataLocator_AndroidSimpleBufferQueue simple_buf_queue = {
            SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
            kNumOpenSlBuffers
    };


    return 0;
}

int OpenSlesOutput::stopPlayout() {
    return 0;
}

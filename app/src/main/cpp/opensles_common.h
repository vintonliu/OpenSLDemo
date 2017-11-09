//
// Created by vinton on 2017/11/07,0007.
//

#ifndef OPENSLDEMO_OPENSLES_COMMON_H
#define OPENSLDEMO_OPENSLES_COMMON_H

#include <string.h>
#include <assert.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <SLES/OpenSLES_AndroidConfiguration.h>

#include "android_debug.h"

#ifndef NULL
#define NULL    0
#endif

#define VOID_RETURN
#define OPENSL_RETURN_ON_FAILURE(op, ret_val)   \
    do {                                        \
        SLresult err = (op);                    \
        if (err != SL_RESULT_SUCCESS) {         \
            assert(false);                      \
            LOGE("[%s %d] err(%d)", __FUNCTION__, __LINE__, err); \
            return ret_val;                     \
        }                                       \
    } while(0)




#endif //OPENSLDEMO_OPENSLES_COMMON_H

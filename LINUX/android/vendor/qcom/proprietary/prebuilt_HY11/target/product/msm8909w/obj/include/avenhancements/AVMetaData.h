/*
 * Copyright (c) 2015, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#ifndef _AV_METADATA_H_
#define _AV_METADATA_H_

namespace android {

enum {
    kKeyEnableSetBuffers = 'sbuf',
};

enum {
    ENABLE_SET_BUFFERS = 0x1,
    ENABLE_AUDIO_BIG_BUFFERS = 0x2
};

} //namespace android

#endif // _AV_METADATA_H_



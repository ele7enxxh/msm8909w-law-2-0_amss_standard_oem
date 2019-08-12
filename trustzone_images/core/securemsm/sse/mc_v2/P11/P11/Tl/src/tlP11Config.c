/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "tlP11Init.h"
#include "tlP11Token.h"

#define TL_LABEL {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define TL_MANUFACTURER {'Q','U','A','L','C','O','M','M',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define TL_MODEL {'S','S','E',0,0,0,0,0,0,0,0,0,0,0,0,0}
#define TL_SERIAL {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define TL_FLAGS (0x00000001 | 0x00000004)

/*lint -esym(506,g_config) */
tlp11_config_t __attribute__((section("zconfig"))) g_config = {
    TL_MAGIC_0,
    TL_CONFIG_VERSION_MAJOR,
    TL_CONFIG_VERSION_MINOR,
    {
        TL_LABEL,
        TL_MANUFACTURER,
        TL_MODEL,
        TL_SERIAL,
        TL_FLAGS,
        TOKEN_MAX_SESSION_COUNT,
        TOKEN_MAX_SESSION_COUNT,
        TOKEN_PIN_LEN,
        TOKEN_MIN_PIN_LEN,
        TL_HW_VERSION_MAJOR,
        TL_HW_VERSION_MINOR,
        TL_FW_VERSION_MAJOR,
        TL_FW_VERSION_MINOR,
        0, /* maxAuthSpan */
        0, /* maxUsageNum */
        0, /* maxLocalLoginAttempts */
        0, /* maxRemoteLoginAttempts */
        1, /* allowLocalToken */
        UINT32_MAX, /* challengeLifespan */
        1, /* remoteResetUnlockLocal */
        0, /* unlockAtBoot */
        0, /* localUnlockTime */
        0, /* remoteUnlockTime */
        TL_ENABLED_FUNCTIONS,
        TL_SECURITY_EXTENSIONS,
        TL_MIN_ITER_PBKDF2_SK
    },
    {0}
};

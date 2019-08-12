/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef TLP11INIT_H_
#define TLP11INIT_H_


#include "stdint.h"

#define TL_CKF_ENCRYPT            0x00000100
#define TL_CKF_DECRYPT            0x00000200
#define TL_CKF_DIGEST             0x00000400
#define TL_CKF_SIGN               0x00000800
#define TL_CKF_SIGN_RECOVER       0x00001000
#define TL_CKF_VERIFY             0x00002000
#define TL_CKF_VERIFY_RECOVER     0x00004000
#define TL_CKF_GENERATE           0x00008000
#define TL_CKF_GENERATE_KEY_PAIR  0x00010000
#define TL_CKF_WRAP               0x00020000
#define TL_CKF_UNWRAP             0x00040000
#define TL_CKF_DERIVE             0x00080000

#define TL_SE_STICKY_ATTRIBUTES   0x00000001
#define TL_SE_CONFLICTS_CHECK     0x00000002
#define TL_SE_SECURE_TEMPLATES    0x00000004
#define TL_SE_ITER_PBKDF2_SK      0x00000008
#define TL_SE_WEAKER_KEY          0x00000010
#define TL_SE_SECURE_MECH         0x00000020
#define TL_SE_SECURE_WRAP_ONLY    0x00000040
#define TL_SE_TRUSTED_WRAP_ONLY   0x00000080

#define TL_MAGIC_0  0x31315051  // "QP11"
#define TL_MAGIC_1  0x51503131  // "11PQ"
#define TL_CONFIG_VERSION_MAJOR 1
#define TL_CONFIG_VERSION_MINOR 2
#define TL_HW_VERSION_MAJOR 0
#define TL_HW_VERSION_MINOR 1
#define TL_FW_VERSION_MAJOR 0
#define TL_FW_VERSION_MINOR 1
#define TL_CONFIG_SIZE 0x100 /* 256 bytes */
#define TL_ENABLED_FUNCTIONS (TL_CKF_ENCRYPT|TL_CKF_DECRYPT| \
                              TL_CKF_DIGEST| \
                              TL_CKF_SIGN|TL_CKF_SIGN_RECOVER| \
                              TL_CKF_VERIFY|TL_CKF_VERIFY_RECOVER| \
                              TL_CKF_GENERATE|TL_CKF_GENERATE_KEY_PAIR| \
                              TL_CKF_UNWRAP|\
                              TL_CKF_DERIVE)

#define TL_SECURITY_EXTENSIONS (TL_SE_STICKY_ATTRIBUTES| \
                                TL_SE_CONFLICTS_CHECK| \
                                TL_SE_SECURE_TEMPLATES| \
                                TL_SE_ITER_PBKDF2_SK| \
                                TL_SE_WEAKER_KEY| \
                                TL_SE_SECURE_MECH| \
                                TL_SE_SECURE_WRAP_ONLY| \
                                TL_SE_TRUSTED_WRAP_ONLY)

#define TL_MIN_ITER_PBKDF2_SK 10000


typedef struct {
  unsigned char   label[32];           /* blank padded */
  unsigned char   manufacturerID[32];  /* blank padded */
  unsigned char   model[16];           /* blank padded */
  unsigned char   serialNumber[16];    /* blank padded */
  uint32_t      flags;               /* see below */

  uint32_t      ulMaxSessionCount;     /* max open sessions */
  uint32_t      ulMaxRwSessionCount;   /* max R/W sessions */
  uint32_t      ulMaxPinLen;           /* in bytes */
  uint32_t      ulMinPinLen;           /* in bytes */

  uint8_t       hardwareVersion_major;  /* integer portion of version number */
  uint8_t       hardwareVersion_minor;  /* 1/100ths portion of version number */

  uint8_t       firmwareVersion_major;  /* integer portion of version number */
  uint8_t       firmwareVersion_minor;  /* 1/100ths portion of version number */

  uint32_t maxAuthSpan;
  uint32_t maxUsageNum;
  uint32_t maxLocalLoginAttempts;
  uint32_t maxRemoteLoginAttempts;
  uint32_t allowLocalToken;
  uint32_t challengeLifespan;
  /* behaviour on lock */
  uint32_t remoteResetUnlockLocal;
  uint32_t unlockAtBoot;
  uint32_t localUnlockTime;
  uint32_t remoteUnlockTime;
  /* cryptographic behaviour */
  uint32_t enabledFunctions;
  uint32_t securityExtensions;
  uint32_t minIterationsPBKDF2_SK;
} tlp11_config_arg_t;

typedef struct {
  uint32_t magic0;
  uint16_t       major;  /* integer portion of version number */
  uint16_t       minor;  /* 1/100ths portion of version number */
  tlp11_config_arg_t arg;
  uint8_t reserved[TL_CONFIG_SIZE - 8 - sizeof(tlp11_config_arg_t)];
} tlp11_config_t;


#endif /* TLP11INIT_H_ */

/*
 * Copyright(c) 2013 Qualcomm Technologies, Inc.All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#pragma once

#include <stdint.h>

#include "mcSo.h"
#include "sse_defs.h"
#include "sse_mc_defs.h"
#include <InputStatus.h>

#ifdef TL_BUILD
  #pragma anon_unions
#endif

typedef enum {
  TL_SECURETOUCH_TEARDOWN_CMD_COMPLETE  = 0,
  TL_SECURETOUCH_TEARDOWN_CMD_ABORT     = 1,
  TL_SECURETOUCH_TEARDOWN_CMD_MAX       = 0x7FFFFFFF,
} tlSecureTouch_Teardown_Cmd_t;

typedef struct {
  uint32_t nInputFields;
  DECLARE_ENC_START();
  uint32_t nonce;
  mcUuid_t srcUuid;
  mcSpid_t srcSpid;
} tlSecureTouchRequest_clear_t;

#define SECURETOUCH_REQUEST_SO_SIZE MC_SO_SIZE(PLAINLEN(tlSecureTouchRequest_clear_t),ENCRYPTEDLEN(tlSecureTouchRequest_clear_t))

#define SECURETOUCH_RESPONSE_SO_SIZE MC_SO_SIZE(PLAINLEN(SecureTouchResponse_t),ENCRYPTEDLEN(SecureTouchResponse_t))

typedef union {
  tlSecureTouchRequest_clear_t clear;
  uint8_t so[SECURETOUCH_REQUEST_SO_SIZE];
} tlSecureTouchRequest_t;

typedef union {
  SecureTouchResponse_t clear;
  uint8_t so[SECURETOUCH_RESPONSE_SO_SIZE];
} tlSecureTouchResponse_t;


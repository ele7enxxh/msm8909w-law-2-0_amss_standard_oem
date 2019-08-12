/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

//#define TL_LOG_FILE
#include "tlStd.h"
#include "TlApi/TlApi.h"
#include "tlP11Api.h"
#include "tlP11Utils.h"
#include "tlP11Validation.h"
#include "tlLog.h"
#include "tlP11Mechanisms.h"

/** Process a GenerateKey command
 */
/*lint -e{715,818} Unused and possibly const parameters*/
DECLARE_GENERATE_KEY_FUNC(generic_generate)
{
  CK_RV rv = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  size_t randomLen = 0;
  uint8_t *dst;
  size_t dst_len;
  TL_LOG_I("generic_generate+");
  do {
    if (!key) {
      TL_LOG_E(" NULL input!");
      rv = CKR_GENERAL_ERROR;
      break;
    }

    if (IS_TYPE(key,tlp11_type_generic_key)) {
      dst = key->secret_key_simple.value;
      dst_len = key->secret_key_simple.len;
    } else if (IS_TYPE(key,tlp11_type_otp_key)) {
      dst = key->otp.value;
      dst_len = key->otp.len;
    } else {
      TL_LOG_E(" Can only generate generic and OTP secret keys");
      rv = CKR_TEMPLATE_INCONSISTENT;
      break;
    }

    // done for good, fill the content
    randomLen = dst_len;
    result = tlApiRandomGenerateData(TLAPI_ALG_PSEUDO_RANDOM,dst,&randomLen);
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiRandomGenerateData: 0x%08X",result);
      rv = TL2CK(result);
      break;
    }
    if (randomLen != dst_len) {
      TL_LOG_E("Failed to generate enough random data: %d vs %d",
          randomLen,dst_len);
      rv = CKR_FUNCTION_FAILED;
      break;
    }
    // fill the CKA_CHECH_VALUE field
    // now stubbed
    result = computeChecksum(
        key->secret_key.check_value,
        dst,
        dst_len,
        key->header.object_type);
    if (!IS_OK(result)) {
      TL_LOG_E(" Failed to compute checksum");
      rv = TL2CK(result);
      break;
    }
  } while(0);
  TL_LOG_I("generic_generate-: %08X",rv);
  return rv;
}

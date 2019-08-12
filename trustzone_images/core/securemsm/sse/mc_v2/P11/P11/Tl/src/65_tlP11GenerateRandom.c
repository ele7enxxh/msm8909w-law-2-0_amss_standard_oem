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
#include "tlP11Glob.h"

/** Process a GenerateRandom command
 *
 * @param shared buffer containing the command and our working area
 *
 * @return RET_OK if operation has been successfully completed.
 */
tciReturnCode_t processCmdGenerateRandom(
  tciMessage_t* tci_message
) {
  tciReturnCode_t ret = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  size_t randomLen = 0;

  do {
    TL_LOG_I("processCmdGenerateRandom+");

    // safeguard against TLC on buffers
    if ((tci_message->rspGenerateRandom.params.data == NULL) ||
        (tci_message->cmdGenerateRandom.params.dataLen == 0)) {
      TL_LOG_E("Bad buffer");
      ret = CKR_ARGUMENTS_BAD;
      break;
    }
    if (CKR_OK != validateBufferLocation(tci_message->rspGenerateRandom.params.data)) {
      TL_LOG_E("data ouside valid range!");
      ret = CKR_ARGUMENTS_BAD;
      break;
    }
    TL_LOG_I("Generating %d bytes of random data @ 0x%08X",
        tci_message->cmdGenerateRandom.params.dataLen,
        tci_message->rspGenerateRandom.params.data
        );

    randomLen = tci_message->cmdGenerateRandom.params.dataLen;
    result = tlApiRandomGenerateData(
        TLAPI_ALG_SECURE_RANDOM,
        tci_message->rspGenerateRandom.params.data,
        &randomLen);

    if (!IS_OK(result)) {
      TL_LOG_E("tlApiRandomGenerateData: 0x%08X",result);
      ret = TL2CK(result);
      break;
    }
    if (randomLen != tci_message->cmdGenerateRandom.params.dataLen) {
      TL_LOG_E("Failed to generate enough random data: %d vs %d",
          randomLen,tci_message->cmdGenerateRandom.params.dataLen);
      ret = CKR_FUNCTION_FAILED;
      break;
    }
  } while(0);

  TL_LOG_I("processCmdGenerateRandom- ret:0x%08X",ret);
  return ret;
}

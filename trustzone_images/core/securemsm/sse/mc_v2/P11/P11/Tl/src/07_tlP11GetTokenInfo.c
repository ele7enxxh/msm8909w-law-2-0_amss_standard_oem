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
#include "tlLog.h"
#include "tlP11Glob.h"

/** Process a GetTokenInfo command
 * This function can get an existing token as input. If this is the case, it
 * will read the token informations from it. Otherwise, default values are used.
 *
 * @param shared buffer containing the command
 *
 * @return CKR_OK if operation has been successfully completed.
 */
tciReturnCode_t processCmdGetTokenInfo(
  tciMessage_t* tci_message
) {
  tciReturnCode_t ret = CKR_OK;

  do {
    TL_LOG_I("processCmdGetTokenInfo+");

    if (tci_message->rspGetTokenInfo.params.pInfo == NULL) {
      TL_LOG_E("Null pointer!");
      ret = CKR_ARGUMENTS_BAD;
      break;
    }
    if (CKR_OK != validateBufferLocation(tci_message->rspGetTokenInfo.params.pInfo)) {
      TL_LOG_E("pInfo ouside valid range!");
      ret = CKR_ARGUMENTS_BAD;
      break;
    }

    // fill in the default values
    memset(tci_message->rspGetTokenInfo.params.pInfo->label,' ',TOKEN_LABEL_LEN);
    fillTokenInfo(tci_message->rspGetTokenInfo.params.pInfo);

  } while(0);

	TL_LOG_I("processCmdGetTokenInfo- ret:0x%08X",ret);
  return ret;
}

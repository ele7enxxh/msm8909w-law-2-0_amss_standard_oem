/*
 * Copyright (c) 2012 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary.
 */

#define TL_LOG_FILE

#include "stringl.h"
#include "tlStd.h"
#include "TlApi/TlApi.h"
#include "tlWrapperApi.h"
#include "tlLog.h"



/** Unwrap
 * Unwrap a secure object
 *
 * @param shared buffer containing the command and our working area
 *
 * @return RET_OK if operation has been successfully completed.
 */
tciReturnCode_t processCmdUnwrap(
    tciMessage_t* tci_message
)
{
  ENTERRV;
  tlApiResult_t result = TLAPI_OK;
#ifdef DEVLIB
  unsigned char original_aes32_secret_key[32];
  extern char aes32_tlt_secret_key[32];
  extern mcUuid_t tlUuid;
#endif

  do {
    if ((tci_message->cmdUnwrap.params.data == NULL) ||
        (tci_message->cmdUnwrap.params.dataLen == 0)) {
      TL_LOG_E("NULL/invalid input");
      rv = E_INVALID_INPUT;
      break;
    }
    if ((tci_message->rspUnwrap.params.data == NULL) ||
        (tci_message->rspUnwrap.params.dataLen == 0)) {
      TL_LOG_E("NULL/invalid output buffer");
      rv = E_INVALID_OUTPUT;
      break;
    }

    // trick the system
#ifdef DEVLIB
    memscpy(original_aes32_secret_key,sizeof(original_aes32_secret_key),aes32_tlt_secret_key,sizeof(original_aes32_secret_key));
    memset(aes32_tlt_secret_key,0,sizeof(aes32_tlt_secret_key));
    memscpy(aes32_tlt_secret_key,sizeof(aes32_tlt_secret_key),&tlUuid,sizeof(tlUuid));
#endif
    result = tlApiUnwrapObjectExt(
        (mcSoHeader_t *)tci_message->cmdUnwrap.params.data,
        tci_message->cmdUnwrap.params.dataLen,
        tci_message->rspUnwrap.params.data,
        &(tci_message->rspUnwrap.params.dataLen),
        TLAPI_UNWRAP_PERMIT_CONTEXT_MASK | TLAPI_UNWRAP_PERMIT_DELEGATED);
    if (result != TLAPI_OK) {
      TL_LOG_E("Failed to unwrap the passed object: 0x%08X",result);
      rv = (tciReturnCode_t)result;
    }
#ifdef DEVLIB
    memscpy(aes32_tlt_secret_key,sizeof(aes32_tlt_secret_key,original_aes32_secret_key,sizeof(aes32_tlt_secret_key));
#endif
  } while (0);
  EXITRV;
}


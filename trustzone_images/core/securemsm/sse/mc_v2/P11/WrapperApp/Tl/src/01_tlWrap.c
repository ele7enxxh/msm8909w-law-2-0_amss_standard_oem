/*
 * Copyright (c) 2012 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary.
 */

#define TL_LOG_FILE
#include "tlStd.h"
#include "TlApi/TlApi.h"
#include "tlWrapperApi.h"
#include "tlLog.h"

/** Wrap
 * Wrap a plaintext buffer for the consumption of a destination TL
 *
 * @param shared buffer containing the command and our working area
 *
 * @return RET_OK if operation has been successfully completed.
 */
tciReturnCode_t processCmdWrap(
    tciMessage_t* tci_message
)
{
  ENTERRV;
  tlApiResult_t result = TLAPI_OK;
  tlApiSpTrustletId_t consumer;

  do {
    TL_LOG_I("Delegate wrapping:");
    TL_LOG_I("src: @ 0x%08X",(unsigned int)tci_message->cmdWrap.params.data);
    TL_LOG_I("plainLen: %d",tci_message->cmdWrap.params.plaintext);
    TL_LOG_I("encryptedLen: %d",tci_message->cmdWrap.params.dataLen-tci_message->cmdWrap.params.plaintext);
    TL_LOG_I("dest: @ 0x%08X",(unsigned int)tci_message->rspWrap.params.data);
    TL_LOG_I("destLen: %d",tci_message->rspWrap.params.dataLen);
    TL_LOG_I("context: %d",tci_message->cmdWrap.params.context);
    TL_LOG_I("lifetime: %d",tci_message->cmdWrap.params.lifetime);

    if ((tci_message->cmdWrap.params.data == NULL) ||
        (tci_message->cmdWrap.params.dataLen == 0) ||
        (tci_message->rspWrap.params.dataLen < MC_SO_SIZE(tci_message->cmdWrap.params.plaintext,tci_message->cmdWrap.params.dataLen-tci_message->cmdWrap.params.plaintext))) {
      TL_LOG_E("NULL/invalid input");
      rv = E_INVALID_INPUT;
      break;
    }
    if ((tci_message->rspWrap.params.data == NULL) ||
        (tci_message->rspWrap.params.dataLen == 0)) {
      TL_LOG_E("NULL/invalid output buffer");
      rv = E_INVALID_OUTPUT;
      break;
    }
    consumer.spid = tci_message->cmdWrap.params.spid;
    consumer.uuid = tci_message->cmdWrap.params.uuid;
    dumpHex("Consumer",(const uint8_t*)(&consumer),sizeof(consumer));
    if (tci_message->cmdWrap.params.context == MC_SO_CONTEXT_DEVICE) {
      result = tlApiWrapObjectExt(
        tci_message->cmdWrap.params.data,
        tci_message->cmdWrap.params.plaintext,
        tci_message->cmdWrap.params.dataLen-tci_message->cmdWrap.params.plaintext,
        (mcSoHeader_t *)(tci_message->rspWrap.params.data),
        &(tci_message->rspWrap.params.dataLen),
        tci_message->cmdWrap.params.context,
        tci_message->cmdWrap.params.lifetime,
        NULL,
        TLAPI_WRAP_DEFAULT);
    } else {
      result = tlApiWrapObjectExt(
        tci_message->cmdWrap.params.data,
        tci_message->cmdWrap.params.plaintext,
        tci_message->cmdWrap.params.dataLen-tci_message->cmdWrap.params.plaintext,
        (mcSoHeader_t *)(tci_message->rspWrap.params.data),
        &(tci_message->rspWrap.params.dataLen),
        tci_message->cmdWrap.params.context,
        tci_message->cmdWrap.params.lifetime,
        &consumer,
        TLAPI_WRAP_DEFAULT);
    }
    if (result != TLAPI_OK) {
      TL_LOG_E("Failed to wrap the passed object: 0x%08X",result);
      rv = (tciReturnCode_t)result;
    }
    TL_LOG_I("destLen: %d",tci_message->rspWrap.params.dataLen);
  } while (0);
  EXITRV;
}

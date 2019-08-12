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

/** Process a OpenSession command
 *
 * @param tci shared buffer containing the command and our working area
 *
 * @return CKR_OK if operation has been successfully completed.
 */
tciReturnCode_t processCmdOpenSession(
  tciMessage_t* tci_message
) {
  tciReturnCode_t ret = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  uint32_t sessionCount = 0;
  uint32_t rwSessionCount = 0;
  uint32_t flags;
  uint32_t index;
  size_t randomLen = 0;
  g_token_0_ptr = (tlp11_token_t*)tci_message->cmdOpenSession.params.so_token.value;

  do {
    TL_LOG_I("processCmdOpenSession+");

    result = unwrapToken(&tci_message->cmdOpenSession.params.so_token,g_token_0_ptr);
    if (!IS_OK(result)) {
      TL_LOG_E("unwrapToken: %d",result);
      ret = TL2CK(result);
      break;
    }

    if (tci_message->cmdOpenSession.params.so_group.len == 0) {
      TL_LOG_I("Empty group passed, initializing, it");
      // empty session group, we need to initialize it
      memset(&g_session_group_0,0,sizeof(g_session_group_0));
      g_session_group_0.tokenStamp = g_token_0_ptr->tokenStamp;
      g_session_group_0.authInfo.authEntity = CKU_NONE;
      g_session_group_0.authInfo.lastAuthTime = 0;
      g_session_group_0.authInfo.usageNumLeft = 0;
    } else {
      result = unwrapSessionGroup(&tci_message->cmdOpenSession.params.so_group,&g_session_group_0);
      if (!IS_OK(result)) {
        TL_LOG_E("unwrapSessionGroup: 0x%08X",result);
        ret = TL2CK(result);
        break;
      }
    }

    ret = processTokenGroup(
        NULL,
        NULL, // we've unwrapped it ourselves
        g_token_0_ptr,
        &g_session_group_0
        );

    if (ret != CKR_OK) {
      break;
    }

    flags = tci_message->cmdOpenSession.params.flags;
    if ((flags & CKF_SERIAL_SESSION) == 0) {
      TL_LOG_E("Parallel sessions not supported");
      ret = CKR_SESSION_PARALLEL_NOT_SUPPORTED;
      break;
    }

    ret = getSessionCount(&g_session_group_0,&sessionCount,&rwSessionCount,&index);
    if (CKR_OK != ret) {
      TL_LOG_E("Error counting sessions");
      break;
    }
    TL_LOG_I("sessionCount: %d",sessionCount);
    TL_LOG_I("rwSessionCount: %d",rwSessionCount);
    TL_LOG_I("index: %d",index);

    if (sessionCount >= g_token_0_ptr->info.ulMaxSessionCount) {
      TL_LOG_E("Too many open sessions");
      ret = CKR_SESSION_COUNT;
      break;
    }

    // there should be at least an empty session, if not, it's serious
    if (index == TOKEN_MAX_SESSION_COUNT) {
      TL_LOG_E("No empty sessions!");
      ret = CKR_GENERAL_ERROR;
      break;
    }

    if (flags & CKF_RW_SESSION) {
      // RW session
      if (rwSessionCount >= g_token_0_ptr->info.ulMaxRwSessionCount) {
        TL_LOG_E("Too many open RW sessions");
        ret = CKR_SESSION_COUNT;
        break;
      }
      // fill in session
      g_session_group_0.sessions[index].access = SESSION_ACCESS_RW;
      TL_LOG_I("RW session");
    } else {
      // RO session
      g_session_group_0.sessions[index].access = 0; // SESSION_ACCESS_RO
      TL_LOG_I("RO session");
    }
    // no MC session open on it
    g_session_group_0.sessions[index].context.cryptoSession=CR_SID_INVALID;
    // generate the key used to bind objects to this session
    randomLen = sizeof(g_session_group_0.sessions[index].sessionStamp);
    result = tlApiRandomGenerateData(
        TLAPI_ALG_PSEUDO_RANDOM,
        (uint8_t*)(&g_session_group_0.sessions[index].sessionStamp),
        &randomLen);
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiRandomGenerateData: 0x%08X",result);
      ret = TL2CK(result);
      break;
    }
    if (randomLen != sizeof(g_session_group_0.sessions[index].sessionStamp)) {
      TL_LOG_E("Failed to generate enough random data: %d vs %d",
          randomLen,sizeof(g_session_group_0.sessions[index].sessionStamp));
      ret = CKR_FUNCTION_FAILED;
      break;
    }
    TL_LOG_I("Session stamp: 0x%08X",g_session_group_0.sessions[index].sessionStamp);
    g_session_group_0.sessions[index].sessionStamp &= ~TOKEN_STAMP_MASK; // those bits are reserved to token stamps

    // generate an handle for the session
    randomLen = sizeof(g_session_group_0.sessions[index].handle);
    result = tlApiRandomGenerateData(
            TLAPI_ALG_PSEUDO_RANDOM,
            (uint8_t*)(&g_session_group_0.sessions[index].handle),
            &randomLen);
    if (!IS_OK(result)) {
      TL_LOG_E("tlApiRandomGenerateData: 0x%08X",result);
      ret = TL2CK(result);
      break;
    }
    if (randomLen != sizeof(g_session_group_0.sessions[index].handle)) {
      TL_LOG_E("Failed to generate enough random data: %d vs %d",
          randomLen,sizeof(g_session_group_0.sessions[index].handle));
      ret = CKR_FUNCTION_FAILED;
      break;
    }
    SESSION_INDEX_TO_HANDLE(index,g_session_group_0.sessions[index].handle);

    // wrap the session group
    result = wrapSessionGroup(&g_session_group_0,&tci_message->rspOpenSession.params.so_group);
    if (!IS_OK(result)) {
      TL_LOG_E("wrapSessionGroup: 0x%08X",result);
      ret = TL2CK(result);
      break;
    }
    tci_message->rspOpenSession.params.sessionId = g_session_group_0.sessions[index].handle;

  } while(0);

	TL_LOG_I("processCmdOpenSession- 0x%08X",ret);
  return ret;
}

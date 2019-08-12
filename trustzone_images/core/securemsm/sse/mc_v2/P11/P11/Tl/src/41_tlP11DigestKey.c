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
#include "tlP11Mechanisms.h"

/** Process a CryptoInit command
 *
 * @param shared buffer containing the command and our working area
 *
 * @return RET_OK if operation has been successfully completed.
 */
tciReturnCode_t processCmdDigestKey(
  tciMessage_t* tci_message
) {
  tciReturnCode_t ret = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  uint32_t access = 0;
  tlp11_mechanism_t mech;
  uint32_t index = 0xFFFFFFFF;
  uint8_t *dataIn=NULL;
  tlp11_container_object_t * container_object_0_ptr = (tlp11_container_object_t *) tci_message->cmdDigestKey.params.so_object.value;
  g_token_0_ptr = (tlp11_token_t*) tci_message->cmdDigestKey.params.so_token.value;

  do {
    TL_LOG_I("processCmdDigestKey+");

    ret = processTokenGroupSessionObject(
        &tci_message->cmdDigestKey.params.so_token,
        &tci_message->cmdDigestKey.params.so_group,
        tci_message->cmdDigestKey.params.sessionId,
        &tci_message->cmdDigestKey.params.so_object,
        g_token_0_ptr,
        &g_session_group_0,
        &index,
        container_object_0_ptr
        );

    if (ret == CKR_OBJECT_HANDLE_INVALID)
      ret = CKR_KEY_HANDLE_INVALID;
    if (ret != CKR_OK) {
      break;
    }

    // now check access required for key
    access = 0; // RO
    if (container_object_0_ptr->storage.prvt) {
      access |= SESSION_ACCESS_PRIVATE;
    }
    if (container_object_0_ptr->storage.token) {
      access |= SESSION_ACCESS_TOKEN;
    }
    ret = checkAccess(g_token_0_ptr,&g_session_group_0,index,access);
    if (ret != CKR_OK) {
      break;
    }

    // is there a valid operation in this session?
    if (g_session_group_0.sessions[index].context.operation != CKF_DIGEST) {
      TL_LOG_E(" Wrong or no active operation in this session");
      ret = CKR_OPERATION_NOT_INITIALIZED;
      break;
    }

    ret = getMechanismData(
        g_session_group_0.sessions[index].context.mechanism,
        &mech);
    if (ret != CKR_OK) {
      TL_LOG_E(" Failed to get mechanism info");
      break;
    }

    // check the key
    if (!IS_TYPE(container_object_0_ptr,tlp11_type_secret_key)) {
      TL_LOG_E(" Only secret keys can be digested");
      ret = CKR_KEY_INDIGESTIBLE;
      break;
    }

    // proceed with the operation
    if (mech.functions.digest_update == NULL) {
      TL_LOG_E(" No associated update function");
      ret = CKR_GENERAL_ERROR;
      break;
    }
    dataIn = container_object_0_ptr->secret_key_simple.value;
    ret = mech.functions.digest_update(
        &g_session_group_0.sessions[index],
        mech.mechanism,
        dataIn,
        container_object_0_ptr->secret_key_simple.len);
    if (ret != CKR_OK) {
      TL_LOG_E(" Mechanism failed: 0x%08X",ret);
      break;
    }

  } while(0);

  // if for any reason we failed, the operation is reset
  if (index != 0xFFFFFFFF){
    if (ret != CKR_OK) {
      if (g_session_group_0.sessions[index].context.cryptoSession != CR_SID_INVALID)
        tlApiCrAbort(g_session_group_0.sessions[index].context.cryptoSession);
      memset(&g_session_group_0.sessions[index].context,0,sizeof(g_session_group_0.sessions[index].context));
      g_session_group_0.sessions[index].context.cryptoSession = CR_SID_INVALID;
      g_session_group_0.changed=1;
    }
  }

  do {
    // we return the session group
    if (g_session_group_0.changed) {
      result = wrapSessionGroup(&g_session_group_0,&(tci_message->rspDigestKey.params.so_group));
      if (!IS_OK(result)) {
        TL_LOG_E("wrapSessionGroup: 0x%08X",result);
        ret = TL2CK(result);
        break;
      }
    }
  } while(0);

  TL_LOG_I("processCmdDigestKey- ret:0x%08X",ret);
  return ret;
}

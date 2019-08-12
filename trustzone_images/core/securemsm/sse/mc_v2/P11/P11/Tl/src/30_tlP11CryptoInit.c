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
tciReturnCode_t processCmdCryptoInit(
  tciMessage_t* tci_message
) {
  tciReturnCode_t ret = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  uint32_t access = 0;
  tlp11_mechanism_t mech;
  uint32_t index;
  tlp11_container_object_t *container_object_0_ptr = (tlp11_container_object_t *) tci_message->cmdCryptoInit.params.so_object.value;
  g_token_0_ptr = (tlp11_token_t*) tci_message->cmdCryptoInit.params.so_token.value;

  do {
    TL_LOG_I("processCmdCryptoInit+");

    if (tci_message->cmdCryptoInit.params.operation != CKF_DIGEST) {

      ret = processTokenGroupSessionObject(
          &tci_message->cmdCryptoInit.params.so_token,
          &tci_message->cmdCryptoInit.params.so_group,
          tci_message->cmdCryptoInit.params.sessionId,
          &tci_message->cmdCryptoInit.params.so_object,
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

      // now check access required for base key
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
    } else {
      // digest doesn't take a key!
      ret = processTokenGroupSession(
          &tci_message->cmdCryptoInit.params.so_token,
          &tci_message->cmdCryptoInit.params.so_group,
          tci_message->cmdCryptoInit.params.sessionId,
          g_token_0_ptr,
          &g_session_group_0,
          &index
          );
      if (ret != CKR_OK) {
        break;
      }
    }

    // check that no other operation is being carried on
    if (g_session_group_0.sessions[index].context.operation != 0) {
      TL_LOG_E(" Another operation is active in this session");
      ret = CKR_OPERATION_ACTIVE;
      break;
    }

    ret = getMechanismData(
        tci_message->cmdCryptoInit.params.mechanism,
        &mech);
    if (ret != CKR_OK) {
      TL_LOG_E(" Failed to get mechanism info");
      break;
    }
    // check that mechanism supports requested operation
    if ((mech.info.flags & tci_message->cmdCryptoInit.params.operation) == 0) {
      TL_LOG_E(" Mechanism does not support requested operation 0x%08X",
          tci_message->cmdCryptoInit.params.operation);
      ret = CKR_MECHANISM_INVALID;
      break;
    }
    if ((g_token_0_ptr->enabledFunctions & tci_message->cmdCryptoInit.params.operation) == 0) {
      TL_LOG_E(" Token does not support requested operation 0x%08X",
          tci_message->cmdCryptoInit.params.operation);
      ret = CKR_FUNCTION_NOT_SUPPORTED;
      break;
    }

    if (tci_message->cmdCryptoInit.params.operation != CKF_DIGEST) {
      // does the mechanism support the key type
      // are mechanism and key compatible?
      if (((!IS_TYPE(container_object_0_ptr,mech.compatibleType))) &&
          ((!BELONGS_TO_TYPES(container_object_0_ptr,mech.compatibleType)))){
        TL_LOG_E(" Key inconsistent with supplied mechanism");
        ret = CKR_KEY_TYPE_INCONSISTENT;
        break;
      }
      // is this mechanism allowed to be used by this key
      ret = mechanismIsAllowed(mech.mechanism,
            &container_object_0_ptr->key);
      if (ret != CKR_OK)
        break;
    }

    // keep track of how much input data we process
    g_session_group_0.sessions[index].context.inputLen = 0;

    // go through supported operations and call the associated function
    tci_message->cmdCryptoInit.params.param.header.changed = 0;
    if ((tci_message->cmdCryptoInit.params.operation == CKF_ENCRYPT) ||
        (tci_message->cmdCryptoInit.params.operation == CKF_DECRYPT)) {
      if (mech.functions.cipher_init == NULL) {
        TL_LOG_E(" No associated function");
        ret = CKR_GENERAL_ERROR;
        break;
      }
      ret = mech.functions.cipher_init(
          &g_session_group_0.sessions[index],
          tci_message->cmdCryptoInit.params.mechanism,
          tci_message->cmdCryptoInit.params.operation,
          &tci_message->cmdCryptoInit.params.param,
          container_object_0_ptr);
    } else if (tci_message->cmdCryptoInit.params.operation == CKF_DIGEST) {
      if (mech.functions.digest_init == NULL) {
        TL_LOG_E(" No associated function");
        ret = CKR_GENERAL_ERROR;
        break;
      }
      ret = mech.functions.digest_init(
          &g_session_group_0.sessions[index],
          tci_message->cmdCryptoInit.params.mechanism);
    } else if ((tci_message->cmdCryptoInit.params.operation == CKF_SIGN) ||
               (tci_message->cmdCryptoInit.params.operation == CKF_VERIFY)) {
      if (mech.functions.sign_verify_init == NULL) {
        TL_LOG_E(" No associated function");
        ret = CKR_GENERAL_ERROR;
        break;
      }
      ret = mech.functions.sign_verify_init(
          &g_session_group_0.sessions[index],
          tci_message->cmdCryptoInit.params.mechanism,
          tci_message->cmdCryptoInit.params.operation,
          &tci_message->cmdCryptoInit.params.param,
          container_object_0_ptr);

    } else {
      TL_LOG_E(" No supported yet");
      ret = CKR_FUNCTION_NOT_SUPPORTED;
      break;
    }

    if (ret != CKR_OK) {
      TL_LOG_E(" Mechanism failed: 0x%08X",ret);
      break;
    }
    g_session_group_0.sessions[index].context.operation = tci_message->cmdCryptoInit.params.operation;
    g_session_group_0.sessions[index].context.mechanism = tci_message->cmdCryptoInit.params.mechanism;
    g_session_group_0.changed=1;
    TL_LOG_I("New crypto session %08X",
          g_session_group_0.sessions[index].context.cryptoSession);

  } while(0);

  do {
    // we return the session group
    if (g_session_group_0.changed) {
      result = wrapSessionGroup(&g_session_group_0,&(tci_message->rspCryptoInit.params.so_group));
      if (!IS_OK(result)) {
        TL_LOG_E("wrapSessionGroup: 0x%08X",result);
        ret = TL2CK(result);
        break;
      }
    }
    // and possibly the key if modified
    if (container_object_0_ptr->header.changed) {
      result = wrapObject(container_object_0_ptr,&(tci_message->rspCryptoInit.params.so_object));
      if (!IS_OK(result)) {
        TL_LOG_E("wrapObject: 0x%08X",result);
        ret = TL2CK(result);
        break;
      }
    }
  } while(0);

  TL_LOG_I("processCmdCryptoInit- ret:0x%08X",ret);
  return ret;
}

tciReturnCode_t processCmdCryptoAbort(
  tciMessage_t* tci_message
) {
  tciReturnCode_t ret = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  uint32_t index = 0xFFFFFFFF;

  do {
    TL_LOG_I("processCmdCryptoAbort+");

    result = unwrapSessionGroup(
        &tci_message->cmdCryptoAbort.params.so_group,
        &g_session_group_0);
    if (!IS_OK(result)) {
      TL_LOG_E("unwrapSessionGroup: %d",result);
      ret = TL2CK(result);
      break;
    }
    index = SESSION_HANDLE_TO_INDEX(tci_message->cmdCryptoAbort.params.sessionId);
    if (index >= TOKEN_MAX_SESSION_COUNT) {
      ret = CKR_SESSION_HANDLE_INVALID;
      index = 0xFFFFFFFF;
      break;
    }
    if (g_session_group_0.sessions[index].sessionStamp == 0) {
      ret =  CKR_SESSION_CLOSED;
      break;
    }
    // is there a valid operation on this session?
    if (g_session_group_0.sessions[index].context.operation == 0) {
      TL_LOG_E(" No active operation in this session");
      ret = CKR_OPERATION_NOT_INITIALIZED;
      break;
    }
    // abort the operation

    if (g_session_group_0.sessions[index].context.cryptoSession != CR_SID_INVALID) {
      TL_LOG_I("Aborting crypto session %08X",
          g_session_group_0.sessions[index].context.cryptoSession);
      result = tlApiCrAbort(g_session_group_0.sessions[index].context.cryptoSession);
      if (!IS_OK(result)) {
        TL_LOG_E("Failed to abort the session: 0x%08X",result);
      }
    }
    memset(&g_session_group_0.sessions[index].context,0,sizeof(g_session_group_0.sessions[index].context));
    g_session_group_0.sessions[index].context.cryptoSession = CR_SID_INVALID;
    g_session_group_0.changed=1;


  } while(0);

  do {
    // we return the session group
    if (g_session_group_0.changed) {
      result = wrapSessionGroup(&g_session_group_0,&(tci_message->rspCryptoAbort.params.so_group));
      if (!IS_OK(result)) {
        TL_LOG_E("wrapSessionGroup: 0x%08X",result);
        ret = TL2CK(result);
        break;
      }
    }
  } while(0);

  TL_LOG_I("processCmdCryptoAbort- ret:0x%08X",ret);
  return ret;
}

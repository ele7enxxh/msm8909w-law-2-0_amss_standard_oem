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

/** Process a CryptoUpdate command
 *
 * @param shared buffer containing the command and our working area
 *
 * @return RET_OK if operation has been successfully completed.
 */
tciReturnCode_t processCmdCryptoUpdate(
  tciMessage_t* tci_message
) {
  tciReturnCode_t ret = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  bool_t returningBuffer = 0;
  tlp11_mechanism_t mech;
  uint32_t index = 0xFFFFFFFF;

  do {
    TL_LOG_I("processCmdCryptoUpdate+");

    // safeguard against TLC on buffers
    if ((tci_message->cmdCryptoUpdate.params.data == NULL) &&
        (tci_message->cmdCryptoUpdate.params.dataLen > 0)) {
      TL_LOG_E("Inconsistent input");
      ret = CKR_ARGUMENTS_BAD;
      break;
    }
    if (CKR_OK != validateBufferLocation(tci_message->cmdCryptoUpdate.params.data)) {
      TL_LOG_E("data (IN) ouside valid range!");
      ret = CKR_ARGUMENTS_BAD;
      break;
    }
    if (CKR_OK != validateBufferLocation(tci_message->rspCryptoUpdate.params.data)) {
      TL_LOG_E("data (OUT) ouside valid range!");
      ret = CKR_ARGUMENTS_BAD;
      break;
    }

    result = unwrapSessionGroup(
        &tci_message->cmdCryptoUpdate.params.so_group,
        &g_session_group_0);
    if (!IS_OK(result)) {
      TL_LOG_E("unwrapSessionGroup: %d",result);
      ret = TL2CK(result);
      break;
    }
    index = SESSION_HANDLE_TO_INDEX(tci_message->cmdCryptoUpdate.params.sessionId);
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
    ret = getMechanismData(
        g_session_group_0.sessions[index].context.mechanism,
        &mech);
    if (ret != CKR_OK) {
      TL_LOG_E(" Failed to get mechanism info");
      break;
    }

    if (tci_message->rspCryptoUpdate.params.data == NULL) {
      // this is actually ok if it is a call to digest_update or verify (both update and final) or sign_update
      if (((g_session_group_0.sessions[index].context.operation != CKF_DIGEST) ||
           (tci_message->cmdCryptoUpdate.params.doFinal == 1)) &&
          (g_session_group_0.sessions[index].context.operation != CKF_VERIFY) &&
          ((g_session_group_0.sessions[index].context.operation != CKF_SIGN) ||
           (tci_message->cmdCryptoUpdate.params.doFinal == 1))){
        // we're asked an estimate of the output size
        if (g_session_group_0.sessions[index].context.mechanism == CKM_HOTP) {
          tci_message->rspCryptoUpdate.params.dataLen = sizeof(tlp11_CK_OTP_SIGNATURE_INFO_t);
        } else {
          tci_message->rspCryptoUpdate.params.dataLen =
            max(2*(tci_message->cmdCryptoUpdate.params.dataLen + g_session_group_0.sessions[index].context.inputLen),
                MODULUS_MAX_LEN);
        }
        TL_LOG_E("NULL output buffer, returning an estimate: %d bytes",tci_message->rspCryptoUpdate.params.dataLen);
        returningBuffer = 1;
        ret = CKR_OK;
        break;
      }
    }
    tci_message->rspCryptoUpdate.params.dataLen = tci_message->cmdCryptoUpdate.params.dataOutLen;

    // go through supported operations and call the associated function
    if ((g_session_group_0.sessions[index].context.operation == CKF_ENCRYPT) ||
        (g_session_group_0.sessions[index].context.operation == CKF_DECRYPT)) {
      if (tci_message->cmdCryptoUpdate.params.doFinal == 0) {
        if (mech.functions.cipher_update == NULL) {
          TL_LOG_E(" No associated update function");
          ret = CKR_GENERAL_ERROR;
          break;
        }
        ret = mech.functions.cipher_update(
            &g_session_group_0.sessions[index],
            mech.mechanism,
            tci_message->cmdCryptoUpdate.params.data,
            tci_message->cmdCryptoUpdate.params.dataLen,
            tci_message->rspCryptoUpdate.params.data,
            &tci_message->rspCryptoUpdate.params.dataLen);
      } else {
        if (mech.functions.cipher_final == NULL) {
          TL_LOG_E(" No associated final function");
          ret = CKR_GENERAL_ERROR;
          break;
        }
        ret = mech.functions.cipher_final(
            &g_session_group_0.sessions[index],
            mech.mechanism,
            tci_message->cmdCryptoUpdate.params.data,
            tci_message->cmdCryptoUpdate.params.dataLen,
            tci_message->rspCryptoUpdate.params.data,
            &tci_message->rspCryptoUpdate.params.dataLen);
      }
    } else if (g_session_group_0.sessions[index].context.operation == CKF_DIGEST) {
      if (tci_message->cmdCryptoUpdate.params.doFinal == 0) {
        if (mech.functions.digest_update == NULL) {
          TL_LOG_E(" No associated update function");
          ret = CKR_GENERAL_ERROR;
          break;
        }
        ret = mech.functions.digest_update(
            &g_session_group_0.sessions[index],
            mech.mechanism,
            tci_message->cmdCryptoUpdate.params.data,
            tci_message->cmdCryptoUpdate.params.dataLen);
      } else {
        if (mech.functions.digest_final == NULL) {
          TL_LOG_E(" No associated final function");
          ret = CKR_GENERAL_ERROR;
          break;
        }
        ret = mech.functions.digest_final(
            &g_session_group_0.sessions[index],
            mech.mechanism,
            tci_message->cmdCryptoUpdate.params.data,
            tci_message->cmdCryptoUpdate.params.dataLen,
            tci_message->rspCryptoUpdate.params.data,
            &tci_message->rspCryptoUpdate.params.dataLen);
      }
    } else if ((g_session_group_0.sessions[index].context.operation == CKF_SIGN) ||
               (g_session_group_0.sessions[index].context.operation == CKF_VERIFY)) {

      if (tci_message->cmdCryptoUpdate.params.doFinal == 0) {
        if (mech.functions.sign_verify_update == NULL) {
          TL_LOG_E(" No associated update function");
          ret = CKR_GENERAL_ERROR;
          break;
        }
        ret = mech.functions.sign_verify_update(
            &g_session_group_0.sessions[index],
            mech.mechanism,
            tci_message->cmdCryptoUpdate.params.data,
            tci_message->cmdCryptoUpdate.params.dataLen);
      } else {
        if (mech.functions.sign_verify_final == NULL) {
          TL_LOG_E(" No associated final function");
          ret = CKR_GENERAL_ERROR;
          break;
        }
        ret = mech.functions.sign_verify_final(
            &g_session_group_0.sessions[index],
            mech.mechanism,
            tci_message->cmdCryptoUpdate.params.data,
            tci_message->cmdCryptoUpdate.params.dataLen,
            tci_message->rspCryptoUpdate.params.data,
            &tci_message->rspCryptoUpdate.params.dataLen);
      }
    } else {
      TL_LOG_E(" No supported yet");
      ret = CKR_FUNCTION_NOT_SUPPORTED;
      break;
    }

    if (ret != CKR_OK) {
      TL_LOG_E(" Mechanism failed: 0x%08X",ret);
      break;
    }
    g_session_group_0.sessions[index].context.inputLen += tci_message->cmdCryptoUpdate.params.dataLen;
    g_session_group_0.changed = 1;

  } while(0);

  // if for any reason we failed, the operation is reset
  if ((returningBuffer == 0) &&
      (index != 0xFFFFFFFF)){
    if (((ret != CKR_OK) && (tci_message->cmdCryptoUpdate.params.doFinal == 0)) ||
        (tci_message->cmdCryptoUpdate.params.doFinal == 1)) {
      if (g_session_group_0.sessions[index].context.cryptoSession != CR_SID_INVALID)
        tlApiCrAbort(g_session_group_0.sessions[index].context.cryptoSession);
      memset(&g_session_group_0.sessions[index].context,0,sizeof(g_session_group_0.sessions[index].context));
      g_session_group_0.sessions[index].context.cryptoSession = CR_SID_INVALID;
      g_session_group_0.sessions[index].context.operation = 0;
      g_session_group_0.changed=1;
    }
  }

  if ((tci_message->cmdCryptoUpdate.params.doFinal == 1) &&
      (returningBuffer == 0) &&
      (ret == CKR_OK) &&
      (index < TOKEN_MAX_SESSION_COUNT)) {
    g_session_group_0.sessions[index].context.operation = 0;
    g_session_group_0.changed=1;
    TL_LOG_I("Operation done.");
  }

  do {
    // we return the session group
    if (g_session_group_0.changed) {
      result = wrapSessionGroup(&g_session_group_0,&(tci_message->rspCryptoUpdate.params.so_group));
      if (!IS_OK(result)) {
        TL_LOG_E("wrapSessionGroup: 0x%08X",result);
        ret = TL2CK(result);
        break;
      }
    }
  } while(0);

  TL_LOG_I("processCmdCryptoUpdate- ret:0x%08X",ret);
  return ret;
}

/*
 * Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

//#define TL_LOG_FILE

#include "stringl.h"
#include "tlStd.h"
#include "TlApi/TlApi.h"
#include "tlP11Api.h"
#include "tlP11Utils.h"
#include "tlP11Validation.h"
#include "tlLog.h"
#include "tlP11Glob.h"
#include "tlP11Mechanisms.h"

static uint32_t current_index=0;
static uint32_t password_min_len=0;

/** Process a GenerateKey command for a generic secret on the secure keypad
 * The object is passed in as clear, and returned as secured
 * Only symmetric secret keys can be created with this function
 *
 * @param shared buffer containing the command and our working area
 *
 * @return RET_OK if operation has been successfully completed.
 */
tciReturnCode_t processCmdGenerateKeySKInit(
  tciMessage_t* tci_message
) {
  tciReturnCode_t ret = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  uint32_t access = SESSION_ACCESS_RW;
  tlp11_mechanism_t mech;

  do {
    TL_LOG_I("processCmdGenerateKeySKInit+");

    ret = processTokenGroupSession(
        &tci_message->cmdGenerateKey.params.so_token,
        &tci_message->cmdGenerateKey.params.so_group,
        tci_message->cmdGenerateKey.params.sessionId,
        &g_token_0,
        &g_session_group_0,
        &current_index
        );

    if (ret != CKR_OK) {
      break;
    }

    // copy input locally, since we will modify it
    g_container_object_0=tci_message->cmdGenerateKeySKInit.params.object;

    // check access, before inspecting the object
    if (g_container_object_0.storage.prvt) {
      access |= SESSION_ACCESS_PRIVATE;
    }
    if (g_container_object_0.storage.token) {
      access |= SESSION_ACCESS_TOKEN;
    }
    ret = checkAccess(&g_token_0,&g_session_group_0,current_index,access);
    if (ret != CKR_OK) {
      break;
    }

    // check passed object and sanitize it
    // we do not rely on header.object_type being properly set
    if (g_container_object_0.object.cls != CKO_SECRET_KEY) {
      TL_LOG_E(" Can only create CKO_SECRET_KEY");
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    if (g_container_object_0.key.key_type != CKK_GENERIC_SECRET) {
      TL_LOG_E(" Can only create CKK_GENERIC_SECRET");
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }

    if (tci_message->cmdGenerateKeySKInit.params.mechanism != CKM_GENERIC_SECRET_KEY_GEN_KEYPAD) {
      TL_LOG_E(" Can only be called for CKM_GENERIC_SECRET_KEY_GEN_KEYPAD");
      ret = CKR_MECHANISM_INVALID;
      break;
    }

    ret = getMechanismData(
      tci_message->cmdGenerateKeySKInit.params.mechanism,
      &mech);
    if (ret != CKR_OK) {
      TL_LOG_E(" Failed to get mechanism info");
      break;
    }

    if ((mech.info.flags & CKF_GENERATE) == 0) {
      TL_LOG_E(" Mechanism does not support requested operation 0x%08X",
          CKF_GENERATE);
      ret = CKR_MECHANISM_INVALID;
      break;
    }
    if ((g_token_0.enabledFunctions & CKF_GENERATE) == 0) {
      TL_LOG_E(" Token does not support requested operation 0x%08X",
          CKF_GENERATE);
      ret = CKR_FUNCTION_NOT_SUPPORTED;
      break;
    }

    // check the template
    g_container_object_0.header.object_type = tlp11_type_generic_key;
    // length is not specified, but will be set by the user during the input process
    if (g_container_object_0.key.key_gen_mechanism != CK_UNAVAILABLE_INFORMATION) {
      TL_LOG_E("Inconsistent KEY_GEN_MECHANISM: 0x%08X",
          g_container_object_0.key.key_gen_mechanism);
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    g_container_object_0.key.key_gen_mechanism = CKM_GENERIC_SECRET_KEY_GEN_KEYPAD;

    if (g_container_object_0.secret_key.trusted == 1) {
      // can be set to true only by the SO
      TL_LOG_E("CKA_TRUSTED = 1 can be set only on objects imported as Secure Objects");
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    // coming from the secure keypad, we enforce some values
    g_container_object_0.storage.modifiable=0;
    g_container_object_0.storage.prvt=1;
    g_container_object_0.key.local=1;
    g_container_object_0.secret_key.always_sensitive=1;
    g_container_object_0.secret_key.sensitive=1;
    g_container_object_0.secret_key.extractable=0;
    g_container_object_0.secret_key.never_extractable=1;
    ret = validateGenericSecretKey(&g_container_object_0.secret_key_simple);
    if (ret != CKR_OK) {
      TL_LOG_E("Failed to validate the key");
      break;
    }
    password_min_len = max(g_container_object_0.secret_key_simple.len,PASSWORD_MIN_LEN);
    if (password_min_len > PASSWORD_MAX_LEN) {
      TL_LOG_E("Requested length exceed maximum supported length");
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }

    ret = validateSecureTemplateSecretKey(&g_container_object_0.secret_key,CKF_GENERATE);
    if (ret != CKR_OK) {
      TL_LOG_E("Key doesn't respect Secure Template");
      break;
    }
    // if we got to this point, we're good to go
    if ((g_config.arg.flags & CKF_PROTECTED_AUTHENTICATION_PATH) &&
        (g_token_0.info.flags & CKF_PROTECTED_AUTHENTICATION_PATH)){
      memset(pin,0,sizeof(pin));
      memset(pinVerification,0,sizeof(pinVerification));
      pinLen = 0;
      pinVerLen = 0;
      pinMaxLen = PASSWORD_MAX_LEN;
      pinFailed = 0;
      pinEntered = 0;
      // grab the keypad
      result = tlApiGrabKeypad();
      if (!IS_OK(result)) {
        TL_LOG_E("tlApiGrabKeypad: %d",result);
        ret = TL2CK(result);
        break;
      }
      tci_message->rspGenerateKeySKInit.params.event = tlp11_keypd_event_started;
      ret = CKR_SK_CONTINUE;
      // we're good to return
    } else {
      TL_LOG_E("This method can be called only on SK enabled tokens!");
      ret = CKR_FUNCTION_FAILED;
      break;
    }

  } while(0);

  do {
    // we return the session group
    if (g_session_group_0.changed) {
      result = wrapSessionGroup(&g_session_group_0,&(tci_message->rspGenerateKeySKInit.params.so_group));
      if (!IS_OK(result)) {
        TL_LOG_E("wrapSessionGroup: 0x%08X",result);
        if (ret == CKR_SK_CONTINUE) {
          // release keypad before exiting with error
          tlApiReleaseKeypad();
        }
        ret = TL2CK(result);
        break;
      }
    }
  } while(0);

	TL_LOG_I("processCmdGenerateKeySKInit- ret:0x%08X",ret);
  return ret;
}

tciReturnCode_t processCmdGenerateKeySKContinue(
  tciMessage_t* tci_message
) {
  tciReturnCode_t ret = CKR_SK_CONTINUE;
  tlApiResult_t result = TLAPI_OK;

  do {
    TL_LOG_I("processCmdGenerateKeySKContinue+");

    if (pinEntered == 0) {
      // get the PIN
      ret = getPIN(
          password_min_len,
          pinMaxLen,
          pin,
          &pinLen,
          &(tci_message->rspGenerateKeySKContinue.params.event),
          &pinFailed
          );
      if (ret != CKR_SK_CONTINUE) {
        break;
      }
      if (tci_message->rspGenerateKeySKContinue.params.event == tlp11_keypd_event_password_done) {
        pinEntered=1;
      }
    } else {
      // get the confirmation of the PIN already entered
      ret = getPIN(
          pinLen,
          pinLen,
          pinVerification,
          &pinVerLen,
          &(tci_message->rspGenerateKeySKContinue.params.event),
          &pinFailed
          );
      if (ret != CKR_SK_CONTINUE) {
        break;
      }
      if (tci_message->rspGenerateKeySKContinue.params.event == tlp11_keypd_event_password_done) {
        // verify the entered PIN
        tci_message->rspGenerateKeySKContinue.params.event = tlp11_keypd_event_confirmation_done;
        if (pinFailed == 1) {
          TL_LOG_E("Key too long");
          ret = CKR_TEMPLATE_INCONSISTENT;
          break;
        }
        if (pinLen < password_min_len) {
          TL_LOG_E("Key too short: %d vs %d",pinLen,password_min_len);
          ret = CKR_TEMPLATE_INCONSISTENT;
          break;
        }
        // compare the first PIN and what entered as confirmation
        if (pinLen != pinVerLen) {
          TL_LOG_E("PIN length != confirmation length");
          ret = CKR_PIN_INCORRECT;
          break;
        }
        if (0 != timesafe_memcmp(pin,pinVerification,pinLen)) {
          TL_LOG_E("PIN != confirmation");
          ret = CKR_PIN_INCORRECT;
          break;
        }
        // PIN is good
        g_container_object_0.secret_key_simple.len = pinLen;
        memscpy(g_container_object_0.secret_key_simple.value,SECRET_KEY_MAX_LEN,pin,pinLen);
        memset(pin,0,sizeof(pin));
        memset(pinVerification,0,sizeof(pinVerification));
        pinLen = 0;
        pinVerLen = 0;
        pinMaxLen = 0;
        pinFailed = 0;
        pinEntered = 0;
        ret = CKR_OK;
      }
    }
  } while(0);

  // now, tidy up object
  do {
    if (ret == CKR_OK) {
      result = computeChecksum(
        g_container_object_0.secret_key.check_value,
        g_container_object_0.secret_key_simple.value,
        g_container_object_0.secret_key_simple.len,
        tlp11_type_generic_key);
      if (!IS_OK(result)) {
        TL_LOG_E(" Failed to compute checksum");
        ret = TL2CK(result);
        break;
      }
      if (g_container_object_0.storage.token) {
        g_container_object_0.header.stamp = g_token_0.tokenStamp;
      } else {
        g_container_object_0.header.stamp = g_session_group_0.sessions[current_index].sessionStamp;
      }
      // wrap the object
      result = wrapObject(
          &g_container_object_0,
          &tci_message->rspGenerateKeySKContinue.params.so_object
          );

      if (!IS_OK(result)) {
        TL_LOG_E("wrapObject: 0x%08X",result);
        ret = TL2CK(result);
        break;
      }
    } else {
      // for all other error values we don't actually have to update anything
      break;
    }

  } while(0);

  TL_LOG_I("processCmdGenerateKeySKContinue- 0x%08X",ret);
  return ret;
}

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

/** Process a DeriveKey command
 * The object is passed in as clear, and returned as secured
 * Only symmetric secret keys can be created with this function
 *
 * @param shared buffer containing the command and our working area
 *
 * @return RET_OK if operation has been successfully completed.
 */
tciReturnCode_t processCmdDeriveKey(
  tciMessage_t* tci_message
) {
  tciReturnCode_t ret = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  uint32_t access = SESSION_ACCESS_RW;
  tlp11_mechanism_t mech;
  uint32_t index;
  tlp11_container_object_t *container_object_0_ptr = (tlp11_container_object_t *)tci_message->cmdDeriveKey.params.so_base_key.value;
  g_token_0_ptr = (tlp11_token_t*) tci_message->cmdDeriveKey.params.so_token.value;
  do {
    TL_LOG_I("processCmdDeriveKey+");

    ret = processTokenGroupSessionObject(
        &tci_message->cmdDeriveKey.params.so_token,
        &tci_message->cmdDeriveKey.params.so_group,
        tci_message->cmdDeriveKey.params.sessionId,
        &tci_message->cmdDeriveKey.params.so_base_key,
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

    // copy template locally, since we will modify it
    tlp11_container_object_t *container_object_1_ptr = &tci_message->cmdDeriveKey.params.key;

    // check access, before inspecting the object
    if (container_object_1_ptr->storage.prvt) {
      access |= SESSION_ACCESS_PRIVATE;
    }
    if (container_object_1_ptr->storage.token) {
      access |= SESSION_ACCESS_TOKEN;
    }
    ret = checkAccess(g_token_0_ptr,&g_session_group_0,index,access);
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

    // check passed object and sanitize it
    if (container_object_1_ptr->object.cls != CKO_SECRET_KEY) {
      TL_LOG_E(" Can only derive CKO_SECRET_KEY");
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    ret = getMechanismData(
        tci_message->cmdDeriveKey.params.mechanism,
        &mech);
    if (ret != CKR_OK) {
      TL_LOG_E(" Failed to get mechanism info");
      break;
    }
    // check that mechanism supports unwrapping
    if ((mech.info.flags & CKF_DERIVE) == 0) {
      TL_LOG_E(" Mechanism does not support key derivation");
      ret = CKR_MECHANISM_INVALID;
      break;
    }
    if ((g_token_0_ptr->enabledFunctions & CKF_DERIVE) == 0) {
      TL_LOG_E(" Token does not support key derivation");
      ret = CKR_MECHANISM_INVALID;
      break;
    }
    if (mech.functions.derive == NULL) {
      TL_LOG_E(" No associated function");
      ret = CKR_GENERAL_ERROR;
      break;
    }

    // check the base key
    if (container_object_0_ptr->object.cls == CKO_SECRET_KEY) {
      if (container_object_0_ptr->key.derive == 0) {
        TL_LOG_E(" Key doesn't support key derivation");
        ret = CKR_KEY_TYPE_INCONSISTENT;
        break;
      }
    } else {
      TL_LOG_E(" Among keys we support, only secret keys can be used for key derivation");
      ret = CKR_KEY_TYPE_INCONSISTENT;
      break;
    }

    // are mechanism and key compatible?
    if ((!IS_TYPE(container_object_0_ptr,mech.compatibleType)) &&
        (!BELONGS_TO_TYPES(container_object_0_ptr,mech.compatibleType))) {
      TL_LOG_E(" Key inconsistent with supplied mechanism");
      ret = CKR_KEY_TYPE_INCONSISTENT;
      break;
    }
    // is this mechanism allowed to be used by this key
    ret = mechanismIsAllowed(mech.mechanism,
          &container_object_0_ptr->key);
    if (ret != CKR_OK)
      break;

    // key will NOT be local, ignore input on this
    container_object_1_ptr->key.local = 0;

    if (container_object_1_ptr->key.key_gen_mechanism != CK_UNAVAILABLE_INFORMATION) {
      TL_LOG_E("Inconsistent KEY_GEN_MECHANISM: 0x%08X",
          container_object_0_ptr->key.key_gen_mechanism);
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }

    // set and check default values
    if (container_object_1_ptr->secret_key.trusted == 1) {
        // can be set to true only by the SO
        TL_LOG_E("CKA_TRUSTED = 1 can be set only on objects imported as Secure Objects");
        ret = CKR_TEMPLATE_INCONSISTENT;
        break;
      }
    // sensitivity and extractability are dealt with in the mechanism itself

    // check key type
    if (container_object_1_ptr->key.key_type == CKK_AES) {
      // AES
      TL_LOG_I(" -->AES key");
      container_object_1_ptr->header.object_type = tlp11_type_aes_key;
      // key length is variable, therefore it has to be set
      ret = validateAESSecretKey(&container_object_1_ptr->secret_key_simple);
    } else if (container_object_1_ptr->key.key_type == CKK_GENERIC_SECRET) {
      // Generic Secret key
      TL_LOG_I(" -->Generic Secret key");
      container_object_1_ptr->header.object_type = tlp11_type_generic_key;
      // key length is variable, therefore it has to be set
      ret = validateGenericSecretKey(&container_object_1_ptr->secret_key_simple);
    } else {
      ret = CKR_TEMPLATE_INCONSISTENT;
      TL_LOG_E(" Unsupported key type: 0x%08X",container_object_1_ptr->key.key_type);
      break;
    }

    if (ret != CKR_OK) {
      TL_LOG_E(" Validation failed: 0x%08X",ret);
      break;
    }

    ret = validateSecureTemplateSecretKey(&container_object_1_ptr->secret_key,CKF_DERIVE);
    if (ret != CKR_OK) {
      TL_LOG_E("Key doesn't respect Secure Template");
      break;
    }

    // delegate to specific function
    tci_message->cmdDeriveKey.params.param.header.changed = 0;
    ret = (*mech.functions.derive)(
        tci_message->cmdDeriveKey.params.mechanism,
        &tci_message->cmdDeriveKey.params.param,
        container_object_0_ptr,
        container_object_1_ptr,
        g_token_0_ptr,
        &g_session_group_0,
        index);
    if (ret != CKR_OK) {
      TL_LOG_E("Failure in underlying mechanism: 0x%08X",ret);
      break;
    }

    // object is good to go
    if (container_object_1_ptr->storage.token) {
      container_object_1_ptr->header.stamp = g_token_0_ptr->tokenStamp;
    } else {
      container_object_1_ptr->header.stamp = g_session_group_0.sessions[index].sessionStamp;
    }

    // wrap the object
    result = wrapObject(
        container_object_1_ptr,
        &tci_message->rspDeriveKey.params.so_key
        );

    if (!IS_OK(result)) {
      TL_LOG_E("wrapObject: 0x%08X",result);
      ret = TL2CK(result);
      break;
    }
  } while(0);

  do {
    // we return the session group
    if (g_session_group_0.changed) {
      result = wrapSessionGroup(&g_session_group_0,&(tci_message->rspDeriveKey.params.so_group));
      if (!IS_OK(result)) {
        TL_LOG_E("wrapSessionGroup: 0x%08X",result);
        ret = TL2CK(result);
        break;
      }
    }
  } while(0);

	TL_LOG_I("processCmdDeriveKey- ret:0x%08X",ret);
  return ret;
}

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
#include "tlP11Attributes.h"
#include "securityExt.h"

/** Process a WrapKey command
 * The object is passed in as clear, and returned as secured
 * Only symmetric secret keys can be created with this function
 *
 * @param shared buffer containing the command and our working area
 *
 * @return RET_OK if operation has been successfully completed.
 */
tciReturnCode_t processCmdWrapKey(
  tciMessage_t* tci_message
) {
  tciReturnCode_t ret = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  uint32_t access = 0; // RO access
  tlp11_mechanism_t mech;
  uint32_t index;
  uint32_t trusted = 0;
  uint32_t wrap_with_trusted = 0;
  uint32_t sensitive = 0;
  uint32_t extractable = 0;
  tlp11_attribute_template_t *wrap_template = NULL;
  tlp11_container_object_t *container_object_0_ptr = (tlp11_container_object_t *) tci_message->cmdWrapKey.params.so_wrapping_key.value;
  tlp11_container_object_t *container_object_1_ptr = (tlp11_container_object_t *) tci_message->cmdWrapKey.params.so_key.value;
  g_token_0_ptr = (tlp11_token_t*) tci_message->cmdWrapKey.params.so_token.value;

  do {
    TL_LOG_I("processCmdWrapKey+");

    ret = processTokenGroupSessionObject(
        &tci_message->cmdWrapKey.params.so_token,
        &tci_message->cmdWrapKey.params.so_group,
        tci_message->cmdWrapKey.params.sessionId,
        &tci_message->cmdWrapKey.params.so_wrapping_key,
        g_token_0_ptr,
        &g_session_group_0,
        &index,
        container_object_0_ptr
        );

    if (ret == CKR_OBJECT_HANDLE_INVALID)
      ret = CKR_WRAPPING_KEY_HANDLE_INVALID;
    if (ret != CKR_OK) {
      break;
    }

    // now unwrap the key we have to wrap
    result = unwrapObject(&tci_message->cmdWrapKey.params.so_key,container_object_1_ptr);
    if (!IS_OK(result)) {
      TL_LOG_E("unwrapObject: %d",result);
      return TL2CK(result);
    }
    if (container_object_1_ptr->storage.token) {
      if (container_object_1_ptr->header.stamp != g_token_0_ptr->tokenStamp) {
        TL_LOG_E(" key not bound to token");
        ret = CKR_KEY_HANDLE_INVALID;
        break;
      }
    } else { // session object
      ret = CKR_KEY_HANDLE_INVALID;
      for(unsigned int i=0; i<g_token_0_ptr->info.ulMaxSessionCount; i++) {
        if (container_object_1_ptr->header.stamp == g_session_group_0.sessions[i].sessionStamp) {
          ret = CKR_OK;
          break;
        }
      }
    }
    if (ret != CKR_OK) {
      break;
    }

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

    // now check access required for wrapping key
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

    // we can wrap only secret and private keys
    if ((container_object_1_ptr->object.cls != CKO_SECRET_KEY) &&
        (container_object_1_ptr->object.cls != CKO_PRIVATE_KEY) &&
        (container_object_1_ptr->object.cls != CKO_OTP_KEY)){
      TL_LOG_E(" Can only wrap CKO_SECRET_KEY, CKO_PRIVATE_KEY and CKO_OTP_KEY");
      ret = CKR_KEY_NOT_WRAPPABLE;
      break;
    }
    ret = getMechanismData(
        tci_message->cmdWrapKey.params.mechanism,
        &mech);
    if (ret != CKR_OK) {
      TL_LOG_E(" Failed to get mechanism info");
      break;
    }
    // check that mechanism supports unwrapping
    if ((mech.info.flags & CKF_WRAP) == 0) {
      TL_LOG_E(" Mechanism 0x%08X does not support wrapping",mech.mechanism);
      ret = CKR_MECHANISM_INVALID;
      break;
    }
    if ((g_token_0_ptr->enabledFunctions & CKF_WRAP) == 0) {
      TL_LOG_E(" Token does not support wrapping");
      ret = CKR_MECHANISM_INVALID;
      break;
    }
    if (mech.functions.wrap == NULL) {
      TL_LOG_E(" No associated function");
      ret = CKR_GENERAL_ERROR;
      break;
    }

    // check the wrapping key
    if (container_object_0_ptr->object.cls == CKO_SECRET_KEY) {
      if (container_object_0_ptr->secret_key.wrap == 0) {
        TL_LOG_E(" Key doesn't support wrap");
        ret = CKR_WRAPPING_KEY_TYPE_INCONSISTENT;
        break;
      }
      wrap_template = &container_object_0_ptr->secret_key.wrap_template;
      trusted = container_object_0_ptr->secret_key.trusted;
    } else if (container_object_0_ptr->object.cls == CKO_PUBLIC_KEY) {
      if (container_object_0_ptr->public_key.wrap == 0) {
        TL_LOG_E(" Key doesn't support wrap");
        ret = CKR_WRAPPING_KEY_TYPE_INCONSISTENT;
        break;
      }
      wrap_template = &container_object_0_ptr->public_key.wrap_template;
      trusted = container_object_0_ptr->public_key.trusted;
    } else {
      TL_LOG_E(" Only public and secret keys can be used to wrap");
      ret = CKR_WRAPPING_KEY_TYPE_INCONSISTENT;
      break;
    }

    // are mechanism and key compatible?
    if ((!IS_TYPE(container_object_0_ptr,mech.compatibleType)) &&
        (!BELONGS_TO_TYPES(container_object_0_ptr,mech.compatibleType))) {
      TL_LOG_E(" Key inconsistent with supplied mechanism");
      ret = CKR_WRAPPING_KEY_TYPE_INCONSISTENT;
      break;
    }
    // is this mechanism allowed to be used by this key
    ret = mechanismIsAllowed(mech.mechanism,
          &container_object_0_ptr->key);
    if (ret != CKR_OK)
      break;

    // check the key to be wrapped
    if ((container_object_1_ptr->object.cls == CKO_SECRET_KEY) ||
        (container_object_1_ptr->object.cls == CKO_OTP_KEY)){
      wrap_with_trusted = container_object_1_ptr->secret_key.wrap_with_trusted;
      sensitive = container_object_1_ptr->secret_key.sensitive;
      extractable = container_object_1_ptr->secret_key.extractable;
    } else {
      wrap_with_trusted = container_object_1_ptr->private_key.wrap_with_trusted;
      sensitive = container_object_1_ptr->private_key.sensitive;
      extractable = container_object_1_ptr->private_key.extractable;
    }
    if (extractable == 0) {
      TL_LOG_E(" Key unextractable");
      ret = CKR_KEY_UNEXTRACTABLE;
      break;
    }
    if (sensitive && !wrap_with_trusted) {
      TL_LOG_E(" Key sensitive and not allowing to be wrapped with a trusted key");
      ret = CKR_KEY_UNEXTRACTABLE;
      break;
    }
    if (wrap_with_trusted && !trusted) {
      TL_LOG_E(" Key requires a trusted wrapping key");
      ret = CKR_KEY_UNEXTRACTABLE;
      break;
    }

    // corner case: if the key to be wrapped is a private key, it can ONLY be
    // wrapped with a secret key
    if ((container_object_1_ptr->object.cls == CKO_PRIVATE_KEY) &&
        (container_object_0_ptr->object.cls != CKO_SECRET_KEY)) {
      TL_LOG_E(" A private key can only be wrapped with a secret key");
      ret = CKR_KEY_NOT_WRAPPABLE;
      break;
    }

    // check that the key we want to wrap matches the CKA_WRAP_TEMPLATE of the
    // wrapping key

    // start with the array
    if (wrap_template->array_attribute.type != CK_UNAVAILABLE_INFORMATION) {
      uint8_t value[SUBJECT_MAX_LEN];
      uint32_t len = SUBJECT_MAX_LEN;
      ret = getTemplateArrayAttributeValue(
        wrap_template->array_attribute.type,
        container_object_1_ptr,
        value,
        &len);
      if (ret != CKR_OK) {
        TL_LOG_E("Cannot find array attribute 0x%08X in target key!",
            wrap_template->array_attribute.type);
        ret = CKR_WRAPPING_KEY_HANDLE_INVALID;
        break;
      }
      if (len != wrap_template->array_attribute.len) {
        TL_LOG_E("Length mismatch in array attribute in wrap template: 0x%08X",
            wrap_template->array_attribute.type);
        ret = CKR_KEY_NOT_WRAPPABLE;
        break;
      }
      if (0 != timesafe_memcmp(value,wrap_template->array_attribute.value,len)) {
        TL_LOG_E("Value mismatch in array attribute in wrap template: 0x%08X",
            wrap_template->array_attribute.type);
        ret = CKR_KEY_NOT_WRAPPABLE;
        break;
      }
    }
    // now compare the CK_ULONG
    for(int i=0; i<MAX_ULONG_ATTRIBUTES_IN_TEMPLATE; i++) {
      if (wrap_template->ulong_attributes[i].type != CK_UNAVAILABLE_INFORMATION) {
        CK_ULONG value;
        ret = getTemplateULongAttributeValue(
          wrap_template->ulong_attributes[i].type,
          container_object_1_ptr,
          &value);
        if (ret != CKR_OK) {
          TL_LOG_E("Cannot find CK_ULONG attribute 0x%08X in target key!",
              wrap_template->ulong_attributes[i].type);
          ret = CKR_WRAPPING_KEY_HANDLE_INVALID;
          break;
        }
        if (value != wrap_template->ulong_attributes[i].value) {
          TL_LOG_E("Value mismatch in CK_ULONG attribute in wrap template: 0x%08X",
              wrap_template->ulong_attributes[i].type);
          ret = CKR_KEY_NOT_WRAPPABLE;
          break;
        }
      }
    }
    if (ret != CKR_OK) break;
    // now compare the CK_BBOOL
    for(int i=0; i<MAX_BBOOL_ATTRIBUTES_IN_TEMPLATE; i++) {
      if (wrap_template->bbool_attributes[i].type != CK_UNAVAILABLE_INFORMATION) {
        CK_BBOOL value;
        ret = getTemplateBoolAttributeValue(
          wrap_template->bbool_attributes[i].type,
          container_object_1_ptr,
          &value);
        if (ret != CKR_OK) {
          TL_LOG_E("Cannot find CK_BBOOL attribute 0x%08X in target key!",
              wrap_template->bbool_attributes[i].type);
          ret = CKR_WRAPPING_KEY_HANDLE_INVALID;
          break;
        }
        if (value != wrap_template->bbool_attributes[i].value) {
          TL_LOG_E("Value mismatch in CK_BBOOL attribute in wrap template: 0x%08X",
              wrap_template->bbool_attributes[i].type);
          ret = CKR_KEY_NOT_WRAPPABLE;
          break;
        }
      }
    }

    if (weakerKey(g_token_0_ptr)) {
      if (getKeyStrength(container_object_0_ptr) < getKeyStrength(container_object_1_ptr)) {
        // wrapping key is weaker than wrapped key
        TL_LOG_E("Wrapping key is weaker than wrapped key");
        ret = CKR_WRAPPING_KEY_SIZE_RANGE;
        break;
      }
    }

    // check output buffer
    tci_message->rspWrapKey.params.wrappedDataLen=min(
        WRAPPED_DATA_MAX_LEN,tci_message->cmdWrapKey.params.wrappedDataLen);

    // delegate to specific function
    tci_message->cmdWrapKey.params.param.header.changed = 0;
    ret = (*mech.functions.wrap)(
        tci_message->cmdWrapKey.params.mechanism,
        &tci_message->cmdWrapKey.params.param,
        container_object_0_ptr,
        container_object_1_ptr,
        tci_message->rspWrapKey.params.wrappedData,
        &tci_message->rspWrapKey.params.wrappedDataLen);
    if (ret != CKR_OK) {
      TL_LOG_E("Failure in underlying mechanism: 0x%08X",ret);
      break;
    }

  } while(0);

  do {
    // we return the session group
    if (g_session_group_0.changed) {
      result = wrapSessionGroup(&g_session_group_0,&(tci_message->rspWrapKey.params.so_group));
      if (!IS_OK(result)) {
        TL_LOG_E("wrapSessionGroup: 0x%08X",result);
        ret = TL2CK(result);
        break;
      }
    }
  } while(0);

	TL_LOG_I("processCmdWrapKey- ret:0x%08X",ret);
  return ret;
}

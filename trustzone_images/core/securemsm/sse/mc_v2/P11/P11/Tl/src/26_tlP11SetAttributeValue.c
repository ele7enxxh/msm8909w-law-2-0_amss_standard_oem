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
#include "tlP11Attributes.h"
#include "tlLog.h"
#include "tlP11Glob.h"
#include "tlP11Validation.h"


/** Process a SetAttributeValue command
 * @param shared buffer containing the command
 *
 * @return CKR_OK if operation has been successfully completed.
 */
tciReturnCode_t processCmdSetAttributeValue(
  tciMessage_t* tci_message
) {
  tciReturnCode_t ret = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  uint32_t access = 0;
  uint32_t index;
  tlp11_container_object_t *container_object_0_ptr = (tlp11_container_object_t *) tci_message->cmdSetAttributeValue.params.so_object.value;
  g_token_0_ptr = (tlp11_token_t*) tci_message->cmdSetAttributeValue.params.so_token.value;

  do {
    TL_LOG_I("processCmdSetAttributeValue+");

    ret = processTokenGroupSessionObject(
        &tci_message->cmdSetAttributeValue.params.so_token,
        &tci_message->cmdSetAttributeValue.params.so_group,
        tci_message->cmdSetAttributeValue.params.sessionId,
        &tci_message->cmdSetAttributeValue.params.so_object,
        g_token_0_ptr,
        &g_session_group_0,
        &index,
        container_object_0_ptr
        );

    if (ret != CKR_OK) {
      break;
    }

    // check access, before inspecting the object, in 2 steps
    // base access is RW
    access = SESSION_ACCESS_RW;
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

    ret = setAttributeValue(
        tci_message->cmdSetAttributeValue.params.attribute,
        container_object_0_ptr,
        tci_message->cmdSetAttributeValue.params.value
        );

    if (ret != CKR_OK) {
      break;
    }

    // after the attribute has been set, we perform an additional validation
    // on the new object which has been created by the change
    ret = validateObject(container_object_0_ptr);
    if (ret != CKR_OK) {
      break;
    }

    // wrap the object
    result = wrapObject(
        container_object_0_ptr,
        &tci_message->rspSetAttributeValue.params.so_object
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
      result = wrapSessionGroup(&g_session_group_0,&(tci_message->rspSetAttributeValue.params.so_group));
      if (!IS_OK(result)) {
        TL_LOG_E("wrapSessionGroup: 0x%08X",result);
        ret = TL2CK(result);
        break;
      }
    }
  } while(0);

	TL_LOG_I("processCmdSetAttributeValue- ret:0x%08X",ret);
  return ret;
}

/** Process a SetArrayAttributeValue command
 * @param shared buffer containing the command
 *
 * @return CKR_OK if operation has been successfully completed.
 */
tciReturnCode_t processCmdSetArrayAttributeValue(
  tciMessage_t* tci_message
) {
  tciReturnCode_t ret = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  uint32_t access = 0;
  uint32_t index;
  tlp11_container_object_t *container_object_0_ptr = (tlp11_container_object_t *) tci_message->cmdSetArrayAttributeValue.params.so_object.value;
  g_token_0_ptr = (tlp11_token_t*) tci_message->cmdSetArrayAttributeValue.params.so_token.value;

  do {
    TL_LOG_I("processCmdSetArrayAttributeValue+");

    ret = processTokenGroupSessionObject(
        &tci_message->cmdSetArrayAttributeValue.params.so_token,
        &tci_message->cmdSetArrayAttributeValue.params.so_group,
        tci_message->cmdSetArrayAttributeValue.params.sessionId,
        &tci_message->cmdSetArrayAttributeValue.params.so_object,
        g_token_0_ptr,
        &g_session_group_0,
        &index,
        container_object_0_ptr
        );

    if (ret != CKR_OK) {
      break;
    }

    // check access, before inspecting the object, in 2 steps
    // base access is RW
    access = SESSION_ACCESS_RW;
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

    ret = setArrayAttributeValue(
        tci_message->cmdSetArrayAttributeValue.params.attribute,
        container_object_0_ptr,
        tci_message->cmdSetArrayAttributeValue.params.value,
        tci_message->cmdSetArrayAttributeValue.params.len
        );

    if (ret != CKR_OK) {
      break;
    }

    // wrap the object
    result = wrapObject(
        container_object_0_ptr,
        &tci_message->rspSetArrayAttributeValue.params.so_object
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
      result = wrapSessionGroup(&g_session_group_0,&(tci_message->rspSetArrayAttributeValue.params.so_group));
      if (!IS_OK(result)) {
        TL_LOG_E("wrapSessionGroup: 0x%08X",result);
        ret = TL2CK(result);
        break;
      }
    }
  } while(0);

  TL_LOG_I("processCmdSetArrayAttributeValue- ret:0x%08X",ret);
  return ret;
}

/** Process a SetTemplateAttributeValue command
 * @param shared buffer containing the command
 *
 * @return CKR_OK if operation has been successfully completed.
 */
tciReturnCode_t processCmdSetTemplateAttributeValue(
  tciMessage_t* tci_message
) {
  tciReturnCode_t ret = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  uint32_t access = 0;
  uint32_t index;
  tlp11_container_object_t *container_object_0_ptr = (tlp11_container_object_t *) tci_message->cmdSetTemplateAttributeValue.params.so_object.value;
  g_token_0_ptr = (tlp11_token_t*) tci_message->cmdSetTemplateAttributeValue.params.so_token.value;

  do {
    TL_LOG_I("processCmdSetTemplateAttributeValue+");

    ret = processTokenGroupSessionObject(
        &tci_message->cmdSetTemplateAttributeValue.params.so_token,
        &tci_message->cmdSetTemplateAttributeValue.params.so_group,
        tci_message->cmdSetTemplateAttributeValue.params.sessionId,
        &tci_message->cmdSetTemplateAttributeValue.params.so_object,
        g_token_0_ptr,
        &g_session_group_0,
        &index,
        container_object_0_ptr
        );

    if (ret != CKR_OK) {
      break;
    }

    // check access, before inspecting the object, in 2 steps
    // base access is RW
    access = SESSION_ACCESS_RW;
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
    if (container_object_0_ptr->storage.modifiable == 0) {
      ret = CKR_ATTRIBUTE_READ_ONLY;
      break;
    }

    // we can be setting either a CKA_WRAP_TEMPLATE in a public or secret key
    // or a CKA_UNWRAP_TEMPLATE in a private or secret key
    if (IS_TYPE(container_object_0_ptr,tlp11_type_secret_key)) {
      if (tci_message->cmdSetTemplateAttributeValue.params.attribute == CKA_UNWRAP_TEMPLATE) {
        container_object_0_ptr->secret_key.unwrap_template = tci_message->cmdSetTemplateAttributeValue.params.value;
      } else if (tci_message->cmdSetTemplateAttributeValue.params.attribute == CKA_WRAP_TEMPLATE) {
        container_object_0_ptr->secret_key.wrap_template = tci_message->cmdSetTemplateAttributeValue.params.value;
      } else {
        ret = CKR_ATTRIBUTE_TYPE_INVALID;
        break;
      }
    } else if (IS_TYPE(container_object_0_ptr,tlp11_type_public_key)) {
      if (tci_message->cmdSetTemplateAttributeValue.params.attribute == CKA_WRAP_TEMPLATE) {
        container_object_0_ptr->public_key.wrap_template = tci_message->cmdSetTemplateAttributeValue.params.value;
      } else {
        ret = CKR_ATTRIBUTE_TYPE_INVALID;
        break;
      }
    } else if (IS_TYPE(container_object_0_ptr,tlp11_type_private_key)) {
      if (tci_message->cmdSetTemplateAttributeValue.params.attribute == CKA_UNWRAP_TEMPLATE) {
        container_object_0_ptr->private_key.unwrap_template = tci_message->cmdSetTemplateAttributeValue.params.value;
      } else {
        ret = CKR_ATTRIBUTE_TYPE_INVALID;
        break;
      }
    }

    // wrap the object
    result = wrapObject(
        container_object_0_ptr,
        &tci_message->rspSetTemplateAttributeValue.params.so_object
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
      result = wrapSessionGroup(&g_session_group_0,&(tci_message->rspSetTemplateAttributeValue.params.so_group));
      if (!IS_OK(result)) {
        TL_LOG_E("wrapSessionGroup: 0x%08X",result);
        ret = TL2CK(result);
        break;
      }
    }
  } while(0);

  TL_LOG_I("processCmdSetTemplateAttributeValue- ret:0x%08X",ret);
  return ret;
}

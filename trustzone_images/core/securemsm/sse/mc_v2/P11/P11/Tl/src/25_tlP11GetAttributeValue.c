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


/** Process a GetArrayAttribute command
 * @param shared buffer containing the command
 *
 * @return CKR_OK if operation has been successfully completed.
 */
tciReturnCode_t processCmdGetULongAttributeValue(
  tciMessage_t* tci_message
) {
  tciReturnCode_t ret = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  uint32_t access = 0;
  uint32_t index;
  tlp11_container_object_t *container_object_0_ptr = (tlp11_container_object_t *) tci_message->cmdGetULongAttributeValue.params.so_object.value;
  g_token_0_ptr = (tlp11_token_t*)tci_message->cmdGetULongAttributeValue.params.so_token.value;

  do {
    TL_LOG_I("processCmdGetULongAttributeValue+");

    ret = processTokenGroupSessionObject(
        &tci_message->cmdGetULongAttributeValue.params.so_token,
        &tci_message->cmdGetULongAttributeValue.params.so_group,
        tci_message->cmdGetULongAttributeValue.params.sessionId,
        &tci_message->cmdGetULongAttributeValue.params.so_object,
        g_token_0_ptr,
        &g_session_group_0,
        &index,
        container_object_0_ptr
        );

    if (ret != CKR_OK) {
      break;
    }

    // check access, before inspecting the object, in 2 steps
    // base access is RO
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

    ret = getULongAttributeValue(
        tci_message->cmdGetULongAttributeValue.params.attribute,
        container_object_0_ptr,
        &tci_message->rspGetULongAttributeValue.params.value);
    if (ret == CKR_ATTRIBUTE_TYPE_INVALID) {
      TL_LOG_E("Object does not have attribute or it is not encrypted: 0x%08X",
          tci_message->cmdGetULongAttributeValue.params.attribute
          );
      break;
    }

  } while(0);

  do {
    // we return the session group
    if (g_session_group_0.changed) {
      result = wrapSessionGroup(&g_session_group_0,&(tci_message->rspGetULongAttributeValue.params.so_group));
      if (!IS_OK(result)) {
        TL_LOG_E("wrapSessionGroup: 0x%08X",result);
        ret = TL2CK(result);
        break;
      }
    }
  } while(0);


	TL_LOG_I("processCmdGetULongAttributeValue- ret:0x%08X",ret);
  return ret;
}

tciReturnCode_t processCmdGetArrayAttributeValue(
  tciMessage_t* tci_message
) {
  tciReturnCode_t ret = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  uint32_t access = 0;
  uint32_t index;
  tlp11_container_object_t *container_object_0_ptr = (tlp11_container_object_t *) tci_message->cmdGetArrayAttributeValue.params.so_object.value;
  g_token_0_ptr = (tlp11_token_t*)tci_message->cmdGetArrayAttributeValue.params.so_token.value;

  do {
    TL_LOG_I("processCmdGetArrayAttributeValue+");

    ret = processTokenGroupSessionObject(
        &tci_message->cmdGetArrayAttributeValue.params.so_token,
        &tci_message->cmdGetArrayAttributeValue.params.so_group,
        tci_message->cmdGetArrayAttributeValue.params.sessionId,
        &tci_message->cmdGetArrayAttributeValue.params.so_object,
        g_token_0_ptr,
        &g_session_group_0,
        &index,
        container_object_0_ptr
        );

    if (ret != CKR_OK) {
      break;
    }

    // check access, before inspecting the object, in 2 steps
    // base access is RO
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

    if (tci_message->cmdGetArrayAttributeValue.params.len > FIELD_MAX_LEN) {
      TL_LOG_E("Buffer adventised as longer than maximum supported");
      // something wrong in the interface
      ret = CKR_GENERAL_ERROR;
      break;
    }

    tci_message->rspGetArrayAttributeValue.params.len = tci_message->cmdGetArrayAttributeValue.params.len;
    ret = getArrayAttributeValue(
        tci_message->cmdGetArrayAttributeValue.params.attribute,
        container_object_0_ptr,
        tci_message->rspGetArrayAttributeValue.params.value,
        &tci_message->rspGetArrayAttributeValue.params.len);
    if (ret == CKR_BUFFER_TOO_SMALL) {
      // buffer is too small!
      TL_LOG_E("Buffer too small calling getAttributeValueArray. %d bytes, attribute 0x%08X",
          tci_message->cmdGetArrayAttributeValue.params.len,
          tci_message->cmdGetArrayAttributeValue.params.attribute);
      break;
    }
    if (ret == CKR_ATTRIBUTE_TYPE_INVALID) {
      TL_LOG_E("Object does not have attribute or it is not encrypted: 0x%08X",
          tci_message->cmdGetArrayAttributeValue.params.attribute
          );
      break;
    }

  } while(0);

  do {
    // we return the session group
    if (g_session_group_0.changed) {
      result = wrapSessionGroup(&g_session_group_0,&(tci_message->rspGetArrayAttributeValue.params.so_group));
      if (!IS_OK(result)) {
        TL_LOG_E("wrapSessionGroup: 0x%08X",result);
        ret = TL2CK(result);
        break;
      }
    }
  } while(0);


  TL_LOG_I("processCmdGetArrayAttributeValue- ret:0x%08X",ret);
  return ret;
}

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
#include "tlP11Validation.h"

/** Process a CopyObject command
 * The source object is taken, unwrapped, and the passed attributes are applied
 * onto the unwrapped object, if applicable.
 *
 * @param shared buffer containing the command and our working area
 *
 * @return RET_OK if operation has been successfully completed.
 */
tciReturnCode_t processCmdCopyObject(
  tciMessage_t* tci_message
) {
  tciReturnCode_t ret = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  uint32_t access = SESSION_ACCESS_RW;
  uint32_t index;
  tlp11_container_object_t *container_object_0_ptr = (tlp11_container_object_t *)tci_message->cmdCopyObject.params.so_object.value;
  g_token_0_ptr = (tlp11_token_t*)tci_message->cmdCopyObject.params.so_token.value;

  do {
    TL_LOG_I("processCmdCopyObject+");

    ret = processTokenGroupSessionObject(
        &tci_message->cmdCopyObject.params.so_token,
        &tci_message->cmdCopyObject.params.so_group,
        tci_message->cmdCopyObject.params.sessionId,
        &tci_message->cmdCopyObject.params.so_object,
        g_token_0_ptr,
        &g_session_group_0,
        &index,
        container_object_0_ptr
        );

    if (ret != CKR_OK) {
      break;
    }

    // check access, before inspecting the object, in 2 steps
    // the destination object is RW
    if (tci_message->cmdCopyObject.params.prvt) {
      access |= SESSION_ACCESS_PRIVATE;
    }
    if (tci_message->cmdCopyObject.params.token) {
      access |= SESSION_ACCESS_TOKEN;
    }
    ret = checkAccess(g_token_0_ptr,&g_session_group_0,index,access);
    if (ret != CKR_OK) {
      break;
    }

    // the source object is RO
    access = 0;
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

    // additional security restriction:
    // in order to copy a token object into a session object the user must be
    // logged in
    if ((container_object_0_ptr->storage.token && (tci_message->cmdCopyObject.params.token == 0)) &&
        (g_session_group_0.authInfo.authEntity == CKU_NONE)){
      TL_LOG_E("SSE restriction: token objects cannot be made into session objects unless the user is logged in");
      ret = CKR_USER_NOT_LOGGED_IN;
      break;
    }

    //ok, enough permissions to do the operation.
    //check if the operation is ok for the type of object

    // CKA_PRIVATE can change from 0 to 1, not viceversa
    // this is an SSE enforced limitation
    if (container_object_0_ptr->storage.prvt == 0) {
      container_object_0_ptr->storage.prvt = (CK_BBOOL)tci_message->cmdCopyObject.params.prvt;
    } else if (tci_message->cmdCopyObject.params.prvt == 0) {
      ret = CKR_ATTRIBUTE_READ_ONLY;
      break;
    }

    // an object marked as not-modifiable cannot have a copy which is modifiable
    if (container_object_0_ptr->storage.modifiable == 1) {
      container_object_0_ptr->storage.modifiable = (CK_BBOOL)tci_message->cmdCopyObject.params.modifiable;
    } else if (tci_message->cmdCopyObject.params.modifiable == 1) {
      ret = CKR_ATTRIBUTE_READ_ONLY;
      break;
    }

    // no restrictions on having a copy of a token object
    container_object_0_ptr->storage.token = (CK_BBOOL)tci_message->cmdCopyObject.params.token;
    // change the stamp accodingly
    if (container_object_0_ptr->storage.token) {
      container_object_0_ptr->header.stamp = g_token_0_ptr->tokenStamp;
    } else {
      container_object_0_ptr->header.stamp = g_session_group_0.sessions[index].sessionStamp;
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
        &tci_message->rspCopyObject.params.so_object
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
      result = wrapSessionGroup(&g_session_group_0,&(tci_message->rspCopyObject.params.so_group));
      if (!IS_OK(result)) {
        TL_LOG_E("wrapSessionGroup: 0x%08X",result);
        ret = TL2CK(result);
        break;
      }
    }
  } while(0);

	TL_LOG_I("processCmdCopyObject- ret:0x%08X",ret);
  return ret;
}

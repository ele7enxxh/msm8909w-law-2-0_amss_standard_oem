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

/** Process a GenerateKey command
 * The object is passed in as clear, and returned as secured
 * Only symmetric secret keys can be created with this function
 *
 * @param shared buffer containing the command and our working area
 *
 * @return RET_OK if operation has been successfully completed.
 */
tciReturnCode_t processCmdGenerateKeyPair(
  tciMessage_t* tci_message
) {
  tciReturnCode_t ret = CKR_OK;
  tlApiResult_t result = TLAPI_OK;
  uint32_t access = SESSION_ACCESS_RW;
  tlp11_mechanism_t mech;
  uint32_t index;
  g_token_0_ptr = (tlp11_token_t*) tci_message->cmdGenerateKeyPair.params.so_token.value;

  do {
    TL_LOG_I("processCmdGenerateKeyPair+");

    ret = processTokenGroupSession(
        &tci_message->cmdGenerateKeyPair.params.so_token,
        &tci_message->cmdGenerateKeyPair.params.so_group,
        tci_message->cmdGenerateKeyPair.params.sessionId,
        g_token_0_ptr,
        &g_session_group_0,
        &index
        );

    if (ret != CKR_OK) {
      break;
    }

    // copy input locally, since we will modify it
    tlp11_container_object_t *container_object_0_ptr = &tci_message->cmdGenerateKeyPair.params.pub;
    tlp11_container_object_t *container_object_1_ptr = &tci_message->cmdGenerateKeyPair.params.prv;

    // check access, before inspecting the object
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

    access = SESSION_ACCESS_RW;
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

    // check the public template
    // we do not rely on header.object_type being properly set
    if ((container_object_0_ptr->object.cls != CKO_PUBLIC_KEY) ||
        (container_object_1_ptr->object.cls != CKO_PRIVATE_KEY)){
      TL_LOG_E(" Can only create CKO_PUBLIC_KEY/CKO_PRIVATE_KEY");
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }
    ret = getMechanismData(
      tci_message->cmdGenerateKeyPair.params.mechanism,
      &mech);
    if (ret != CKR_OK) {
      TL_LOG_E(" Failed to get mechanism info");
      break;
    }

    if ((mech.info.flags & CKF_GENERATE_KEY_PAIR) == 0) {
      TL_LOG_E(" Mechanism does not support requested operation 0x%08X",
          CKF_GENERATE_KEY_PAIR);
      ret = CKR_MECHANISM_INVALID;
      break;
    }
    if ((g_token_0_ptr->enabledFunctions & CKF_GENERATE_KEY_PAIR) == 0) {
      TL_LOG_E(" Token does not support requested operation 0x%08X",
          CKF_GENERATE_KEY_PAIR);
      ret = CKR_FUNCTION_NOT_SUPPORTED;
      break;
    }

    // key will be local, ignore input on this
    container_object_0_ptr->key.local = 1;
    container_object_1_ptr->key.local = 1;

    tci_message->cmdGenerateKeyPair.params.param.header.changed = 0;
    // now check the key type
    if ((container_object_0_ptr->key.key_type == CKK_RSA) &&
        (container_object_1_ptr->key.key_type == CKK_RSA)){
      // RSA
      TL_LOG_I(" -->RSA keys");
      container_object_0_ptr->header.object_type = tlp11_type_rsa_public_key;
      container_object_1_ptr->header.object_type = tlp11_type_rsa_private_key;
      if (tci_message->cmdGenerateKeyPair.params.mechanism != CKM_RSA_PKCS_KEY_PAIR_GEN) {
        TL_LOG_E(" Mechanism 0x%08X inconsistent with key type 0x%08X",
            tci_message->cmdGenerateKeyPair.params.mechanism,
            CKM_RSA_PKCS_KEY_PAIR_GEN);
        ret = CKR_TEMPLATE_INCONSISTENT;
        break;
      }
      if ((container_object_0_ptr->rsa_public_key.modulus_bits < mech.info.ulMinKeySize) ||
          (container_object_0_ptr->rsa_public_key.modulus_bits > mech.info.ulMaxKeySize)) {
        TL_LOG_E(" Key length outside supported range: %d vs %d-%d",
            container_object_0_ptr->rsa_public_key.modulus_bits,
            mech.info.ulMinKeySize,mech.info.ulMaxKeySize);
        ret = CKR_TEMPLATE_INCONSISTENT;
        break;
      }
      if (container_object_0_ptr->key.key_gen_mechanism != CK_UNAVAILABLE_INFORMATION) {
        TL_LOG_E("Inconsistent KEY_GEN_MECHANISM: 0x%08X",
            container_object_0_ptr->key.key_gen_mechanism);
        ret = CKR_TEMPLATE_INCONSISTENT;
        break;
      }
      container_object_0_ptr->key.key_gen_mechanism = CKM_RSA_PKCS_KEY_PAIR_GEN;
      if (container_object_1_ptr->key.key_gen_mechanism != CK_UNAVAILABLE_INFORMATION) {
        TL_LOG_E("Inconsistent KEY_GEN_MECHANISM: 0x%08X",
            container_object_1_ptr->key.key_gen_mechanism);
        ret = CKR_TEMPLATE_INCONSISTENT;
        break;
      }
      container_object_1_ptr->key.key_gen_mechanism = CKM_RSA_PKCS_KEY_PAIR_GEN;

      if (container_object_0_ptr->public_key.trusted == 1) {
        // can be set to true only by the SO
        TL_LOG_E("CKA_TRUSTED = 1 can be set only on objects imported as Secure Objects");
        ret = CKR_TEMPLATE_INCONSISTENT;
        break;
      }

      ret = validateSecureTemplateKeyPair(
          &container_object_0_ptr->public_key,
          &container_object_1_ptr->private_key,
          CKF_GENERATE_KEY_PAIR);
      if (ret != CKR_OK) {
        TL_LOG_E("Key pair doesn't respect Secure Templates");
        break;
      }
      if (mech.functions.generate_pair == NULL) {
        TL_LOG_E("No function associated with mechanism");
        ret = CKR_GENERAL_ERROR;
        break;
      }
      ret = (*mech.functions.generate_pair)(
          &tci_message->cmdGenerateKeyPair.params.param,
          container_object_0_ptr,
          container_object_1_ptr);
      if (ret != CKR_OK) {
        TL_LOG_E("Error generating key pair: 0x%08X",ret);
        break;
      }
    } else {
      TL_LOG_I(" -->Unsupported key pair type");
      ret = CKR_TEMPLATE_INCONSISTENT;
      break;
    }

    // objects are good to go
    if (container_object_0_ptr->storage.token) {
      container_object_0_ptr->header.stamp = g_token_0_ptr->tokenStamp;
    } else {
      container_object_0_ptr->header.stamp = g_session_group_0.sessions[index].sessionStamp;
    }

    if (container_object_1_ptr->storage.token) {
      container_object_1_ptr->header.stamp = g_token_0_ptr->tokenStamp;
    } else {
      container_object_1_ptr->header.stamp = g_session_group_0.sessions[index].sessionStamp;
    }

    // wrap the objects
    result = wrapObject(
        container_object_0_ptr,
        &tci_message->rspGenerateKeyPair.params.so_public
        );

    if (!IS_OK(result)) {
      TL_LOG_E("wrapObject pub: 0x%08X",result);
      ret = TL2CK(result);
      break;
    }

    result = wrapObject(
        container_object_1_ptr,
        &tci_message->rspGenerateKeyPair.params.so_private
        );

    if (!IS_OK(result)) {
      TL_LOG_E("wrapObject prv: 0x%08X",result);
      ret = TL2CK(result);
      break;
    }
  } while(0);

  do {
    // we return the session group
    if (g_session_group_0.changed) {
      result = wrapSessionGroup(&g_session_group_0,&(tci_message->rspGenerateKeyPair.params.so_group));
      if (!IS_OK(result)) {
        TL_LOG_E("wrapSessionGroup: 0x%08X",result);
        ret = TL2CK(result);
        break;
      }
    }
  } while(0);

	TL_LOG_I("processCmdGenerateKeyPair- ret:0x%08X",ret);
  return ret;
}
